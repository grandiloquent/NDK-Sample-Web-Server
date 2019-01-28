#ifndef _CONNECTION_H__
#define _CONNECTION_H__

#include "buffer.h"
#include "misc.h"
#include "request.h"
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>

#define MAX_CONNECTION (10240)
struct connection {
    int fd;
    struct epoll_event event;
    struct sockaddr_in saddr;
    time_t active_time;
    int heap_idx;
    request_t req;
};
typedef struct connection connection_t;

extern connection_t *connection_accept(int fd, struct sockaddr_in *paddr);

extern connection_t *connection_init();

extern int connection_register(connection_t *c);

extern void connection_unregister(connection_t *c);

extern int connection_close(connection_t *c);

extern void connection_prune();

extern bool connecion_is_expired(connection_t *c);

extern void connecion_set_reactivated(connection_t *c);

extern void connecion_set_expired(connection_t *c);

extern int set_fd_nonblocking(int fd);

#endif