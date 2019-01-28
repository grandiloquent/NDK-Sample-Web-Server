#ifndef _SERVER_H__
#define _SERVER_H__

#include "mem_pool.h"
#include "misc.h"
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

typedef struct {
    uint16_t port;
    bool debug;
    int timeout;
    uint32_t worker;
    char *rootdir;
    int rootdir_fd;
} config_t;
extern config_t server_config;
extern int epoll_fd;
extern int listen_fd;

extern int config_parse(int argc, char *argv[]);

extern int server_setup(uint16_t port);

extern int server_shutdown();

extern int server_accept(int listen_fd);

extern int get_internet_address(char *host, int len, uint16_t *pport, struct sockaddr_in *paddr);

#if USE_MEM_POOL
extern mem_pool_t connection_pool;
#endif

#endif
