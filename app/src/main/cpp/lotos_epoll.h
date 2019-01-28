#ifndef _LOTOS_EPOLL_H__
#define _LOTOS_EPOLL_H__

#include "connection.h"
#include "misc.h"
#include <sys/epoll.h>

#define MAX_EVENTS (10240)
#define FILL_EPOLL_EVENT(pev, pconn, e_events)                                 \
    do {                                                                       \
      struct epoll_event *ev = pev;                                            \
      ev->data.ptr = pconn;                                                    \
      ev->events = e_events;                                                   \
    } while (0)

#endif