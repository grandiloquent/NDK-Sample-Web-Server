#include "lotos_epoll.h"
#include "connection.h"
#include "misc.h"
#include <sys/epoll.h>

struct epoll_event lotos_events[MAX_EVENTS];

int lotos_epoll_create(int flags) { return epoll_create1(flags); }

int lotos_epoll_add(int epoll_fd, connection_t *c, uint32_t events,
                    struct epoll_event *pev) {
    FILL_EPOLL_EVENT(pev, c, events);
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c->fd, pev);
}

int lotos_epoll_mod(int epoll_fd, connection_t *c, uint32_t events,
                    struct epoll_event *pev) {
    FILL_EPOLL_EVENT(pev, c, events);
    return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, c->fd, pev);
}

int lotos_epoll_del(int epoll_fd, connection_t *c, uint32_t events,
                    struct epoll_event *pev) {
    (void)pev;
    return epoll_ctl(epoll_fd, EPOLL_CTL_DEL, c->fd, NULL);
}

inline int lotos_epoll_wait(int epoll_fd, struct epoll_event *events,
                            int max_events, int timeout) {
    return epoll_wait(epoll_fd, events, max_events, timeout);
}
