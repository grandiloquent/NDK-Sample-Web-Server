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
};
typedef struct request request_t;
#endif