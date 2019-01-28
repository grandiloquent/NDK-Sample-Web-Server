#define _GUN_SOURCE

#include "connection.h"
#include "lotos_epoll.h"
#include "misc.h"
#include "server.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

config_t server_config = {
        .port=8888,
        .debug=FALSE,
        .timeout=60,
        .worker=4,
        .rootdir=NULL,
        .rootdir_fd=-1,
};
int epoll_fd = -1;
int list_fd = -1;
#if USE_MEM_POOL
mem_pool_t connection_pool;
#endif

static void sigint_handler(int signum);

static int make_server_socket(uint16_t port, int backlog);

static int add_listen_fd();

int config_parse(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "p:dt:w:r:")) != -1) {
        switch (c) {
            case 'p':
                server_config.port = atoi(optarg);
                break;
            case 'd':
                server_config.debug = TRUE;
                break;
            case 't':
                server_config.timeout = atoi(optarg);
                break;
            case 'w':
                server_config.worker = atoi(optarg);
                if (server_config.worker > sysconf(_SC_NPROCESSORS_ONLN)) {
                    fprintf(stderr, "Config ERROR: worker num greater than cpu available cores.\n");
                    return ERROR;
                }
                break;
            case 'r':
                server_config.rootdir = optarg;
                break;
            default:
                return ERROR;
        }
    }
    DIR *dirp = NULL;
    if (server_config.rootdir != NULL && (dirp = opendir(server_config.rootdir)) != NULL) {
        closedir(dirp);
        server_config.rootdir_fd = open(server_config.rootdir, O_RDONLY);
        ERR_ON(server_config.rootdir_fd == ERROR, server_config.rootdir);
        return OK;
    } else {
        perror(server_config.rootdir);
        return ERROR;
    }
}

static void sigint_handler(int signum) {
    if (signum == SIGINT) {

    }
}

int server_setup(uint16_t port) {
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

}

static int add_listen_fd() {
}

int get_internet_address(char *host, int len, uint16_t *pport, struct sockaddr_in *paddr) {
    strncpy(host, inet_ntoa(paddr->sin_addr), len);
    *pport = ntohs(paddr->sin_port);
    return 0;
}