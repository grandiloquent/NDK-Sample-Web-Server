#ifndef _PARSER_H__
#define _PARSER_H__
#include "buffer.h"
#include "misc.h"
#include "ssstr.h"
#include <string.h>
#define INVALID_REQUEST (-1)
#define CRLF_LINE (2)
#define MAX_ELEMENT_SIZE (2048)
typedef enum {
    HTTP_DELETE,
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT,
    HTTP_INVALID,
} http_method;
typedef struct {
    unsigned short http_major;
    unsigned short http_minor;
} http_version;
typedef struct {
    ssstr_t abs_path;
    ssstr_t query_string;
    ssstr_t mime_extension;
} req_url;
typedef enum {

    S_RL_BEGIN = 0,
    S_RL_METHOD,
    S_RL_SP_BEFORE_URL,
    S_RL_URL,
    S_RL_SP_BEFORE_VERSION,
    S_RL_VERSION_H,
    S_RL_VERSION_HT,
    S_RL_VERSION_HTT,
    S_RL_VERSION_HTTP,
    S_RL_VERSION_HTTP_SLASH,
    S_RL_VERSION_MAJOR,
    S_RL_VERSION_DOT,
    S_RL_VERSION_MINOR,
    S_RL_CR_AFTER_VERSION,
    S_RL_LF_AFTER_VERSION,

    S_HD_BEGIN,
    S_HD_NAME,
    S_HD_COLON,
    S_HD_SP_BEFORE_VAL,
    S_HD_VAL,
    S_HD_CR_AFTER_VAL,
    S_HD_LF_AFTER_VAL,

    S_URL_BEGIN,
    S_URL_ABS_PATH,
    S_URL_QUERY,
    S_URL_END,
} parser_state;
typedef enum {
    TE_IDENTITY = 0,
    TE_CHUNKED,
    TE_COMPRESS,
    TE_DEFLATE,
    TE_GZIP,
} transfer_encoding_t;
typedef struct {
    ssstr_t cache_control;
    ssstr_t connection;
    ssstr_t date;
    ssstr_t transfer_encoding;
    ssstr_t accept;
    ssstr_t accept_charset;
    ssstr_t accept_encoding;
    ssstr_t accept_language;
    ssstr_t cookie;
    ssstr_t host;
    ssstr_t if_modified_since;
    ssstr_t if_unmodified_since;
    ssstr_t max_forwards;
    ssstr_t range;
    ssstr_t referer;
    ssstr_t user_agent;
    ssstr_t content_length;
} request_headers_t;
typedef struct {

    http_method method;
    http_version version;
    ssstr_t request_url_string;
    req_url url;


    bool keep_alive;
    int content_length;
    int transfer_encoding;
    request_headers_t req_headers;
    int num_headers;
    ssstr_t header[2];

    char *next_parse_pos;
    int state;

    char *method_begin;
    char *url_begin;
    char *header_line_begin;
    char *header_colon_pos;
    char *header_val_begin;
    char *header_val_end;
    size_t body_received;
    int buffer_sent;
    bool isCRLF_LINE;
    bool response_done;
    bool err_req;
} parse_archive;
static inline void parse_archive_init(parse_archive *ar, buffer_t *b) {
    memset(ar, 0, sizeof(parse_archive));
    ar->next_parse_pos = b->buf;
    ar->isCRLF_LINE = TRUE;
    ar->content_length = -1;
}
extern int parse_request_line(buffer_t *b, parse_archive *ar);
extern int parse_header_line(buffer_t *b, parse_archive *ar);
extern int parse_header_body_identity(buffer_t *b, parse_archive *ar);
#endif
