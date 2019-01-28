#include "connection.h"
#include "lotos_epoll.h"
#include "misc.h"
#include "server.h"
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
connection_t *lotos_connections[MAX_CONNECTION] = {0};
static int heap_size = 0;
#define LCHILD(x) (((x) << 1) + 1)
#define RCHILD(x) (LCHILD(x) + 1)
#define PARENT(x) ((x - 1) >> 1)
#define INHEAP(n, x) (((-1) < (x)) && ((x) < (n)))
inline static void c_swap(int x, int y) {
    assert(x >= 0 && x < heap_size && y >= 0 && y < heap_size);
    connection_t *tmp = lotos_connections[x];
    lotos_connections[x] = lotos_connections[y];
    lotos_connections[y] = tmp;

    lotos_connections[x]->heap_idx = x;
    lotos_connections[y]->heap_idx = y;
}
static void heap_bubble_up(int idx) {
    while (PARENT(idx) >= 0) {
        int fidx = PARENT(idx);
        connection_t *c = lotos_connections[idx];
        connection_t *fc = lotos_connections[fidx];
        if (c->active_time >= fc->active_time)
            break;
        c_swap(idx, fidx);
        idx = fidx;
    }
}
static void heap_bubble_down(int idx) {
    while (TRUE) {
        int proper_child;
        int lchild = INHEAP(heap_size, LCHILD(idx)) ? LCHILD(idx) : (heap_size + 1);
        int rchild = INHEAP(heap_size, RCHILD(idx)) ? RCHILD(idx) : (heap_size + 1);
        if (lchild > heap_size && rchild > heap_size) {
            break;
        } else if (INHEAP(heap_size, lchild) && INHEAP(heap_size, rchild)) {
            proper_child = lotos_connections[lchild]->active_time <
                           lotos_connections[rchild]->active_time
                           ? lchild
                           : rchild;
        } else if (lchild > heap_size) {
            proper_child = rchild;
        } else {
            proper_child = lchild;
        }

        if (lotos_connections[idx]->active_time <=
            lotos_connections[proper_child]->active_time)
            break;
        assert(INHEAP(heap_size, proper_child));
        c_swap(idx, proper_child);
        idx = proper_child;
    }
}
static int heap_insert(connection_t *c) {
    if (heap_size >= MAX_CONNECTION) {
        return ERROR;
    }
    lotos_connections[heap_size++] = c;
    c->heap_idx = heap_size - 1;
    heap_bubble_up(heap_size - 1);
    return 0;
}
static void heap_print() {
    connection_t *c;
    int i;
    printf("----------------heap---------------\n");
    for (i = 0; i < heap_size; i++) {
        c = lotos_connections[i];
        printf("[%2d] %p fd: %2d heap_idx: %2d active_time: %lu\n", i, c, c->fd,
               c->heap_idx, c->active_time);
    }
    printf("----------------heap---------------\n");
}
static inline void connection_set_nodelay(connection_t *c) {
    static int enable = 1;
    setsockopt(c->fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
}
connection_t *connection_init() {
#if USE_MEM_POOL
    connection_t *c = pool_alloc(&connection_pool);
#else
    connection_t *c = malloc(sizeof(connection_t));
#endif

    if (c) {
        if (request_init(&c->req, c) == ERROR) {
#if USE_MEM_POOL
            pool_free(&connection_pool, c);
#else
            free(c);
#endif
            c = NULL;
        }
    }
    return c;
}
connection_t *connection_accept(int fd, struct sockaddr_in *paddr) {
    connection_t *c = connection_init();
    assert(c != NULL);
    if (c == NULL) {
        connection_close(c);
        return NULL;
    }

    c->fd = fd;
    if (paddr)
        c->saddr = *paddr;
    c->active_time = time(NULL);
    set_fd_nonblocking(c->fd);
    connection_set_nodelay(c);
    if (connection_register(c) == ERROR) {
        connection_close(c);
        return NULL;
    }
    if (lotos_epoll_add(epoll_fd, c, EPOLLIN | EPOLLET, &c->event) == ERROR) {
        connection_close(c);
        return NULL;
    }
#ifndef NDEBUG
    char ip_addr[32];
    uint16_t port;
    get_internet_address(ip_addr, 32, &port, &c->saddr);
    lotos_log(LOG_INFO, "fd: %2d %s:%u\n", fd, ip_addr, port);
    lotos_log(LOG_INFO, "malloc %p %d\n", c, heap_size);
    (void)heap_print;
#endif
    return c;
}
int connection_register(connection_t *c) {
    if (heap_size >= MAX_CONNECTION) {
        return ERROR;
    }
    return heap_insert(c);
}
void connection_unregister(connection_t *c) {
    assert(heap_size >= 1);
    lotos_connections[c->heap_idx] = lotos_connections[heap_size - 1];
    lotos_connections[c->heap_idx]->heap_idx = c->heap_idx;
    heap_size--;
    heap_bubble_down(c->heap_idx);
}
static inline void connection_free(connection_t *c) {
    if (c) {
        buffer_free(c->req.ib);
        c->req.ib = NULL;
        buffer_free(c->req.ob);
        c->req.ob = NULL;
#if USE_MEM_POOL
        pool_free(&connection_pool, c);
#else
        free(c);
#endif
    }
}
int connection_close(connection_t *c) {
    if (c == NULL)
        return OK;

    lotos_epoll_del(epoll_fd, c, 0, NULL);
    close(c->fd);
    connection_unregister(c);
    connection_free(c);
    return OK;
}
void connection_prune() {
    while (heap_size > 0) {
        connection_t *c = lotos_connections[0];
        if (time(NULL) - c->active_time >= server_config.timeout) {
#ifndef NDEBUG

            lotos_log(LOG_INFO, "prune %p %d\n", c, heap_size);
#endif
            connection_close(c);
        } else
            break;
    }
}
inline bool connecion_is_expired(connection_t *c) {
    return (time(NULL) - c->active_time > server_config.timeout);
}
void connecion_set_reactivated(connection_t *c) {
    c->active_time = time(NULL);
    heap_bubble_down(c->heap_idx);
}
void connecion_set_expired(connection_t *c) {
    c->active_time = 0;
    heap_bubble_up(c->heap_idx);
}
int set_fd_nonblocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    ABORT_ON(flag == ERROR, "fcntl: F_GETFL");
    flag |= O_NONBLOCK;
    ABORT_ON(fcntl(fd, F_SETFL, flag) == ERROR, "fcntl: FSETFL");
    return 0;
}
