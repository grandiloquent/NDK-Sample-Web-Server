#ifndef _REQUEST_H__
#define _REQUEST_H__
#include "buffer.h"
#include "connection.h"
#include "http_parser.h"
#include "misc.h"
struct request {
    struct connection *c;
    buffer_t *ib;
    buffer_t *ob;
    parse_archive par;
    int resource_fd;
    int resource_size;
    int status_code;
    int (*req_handler)(struct request *);
    int (*res_handler)(struct request *);
};
typedef struct request request_t;
extern int request_init(request_t *r, struct connection *c);
extern int request_reset(request_t *r);
extern int request_handle(struct connection *c);
extern int response_handle(struct connection *c);
void header_handler_dict_init();
void header_handler_dict_free();
#endif
