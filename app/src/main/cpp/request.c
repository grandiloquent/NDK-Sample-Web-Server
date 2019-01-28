#define _GNU_SOURCE
#include "buffer.h"
#include "connection.h"
#include "dict.h"
#include "http_parser.h"
#include "lotos_epoll.h"
#include "misc.h"
#include "response.h"
#include "server.h"
#include "ssstr.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
static int request_handle_request_line(request_t *r);
static int request_handle_headers(request_t *r);
static int request_handle_body(request_t *r);
static int response_handle_send_buffer(struct request *r);
static int response_handle_send_file(struct request *r);
static int response_assemble_buffer(struct request *r);
static int response_assemble_err_buffer(struct request *r, int status_code);
typedef int (*header_handle_method)(request_t *, size_t);
static int request_handle_hd_base(request_t *r, size_t offset);
static int request_handle_hd_connection(request_t *r, size_t offset);
static int request_handle_hd_content_length(request_t *r, size_t offset);
static int request_handle_hd_transfer_encoding(request_t *r, size_t offset);
typedef struct {
    ssstr_t hd;
    header_handle_method func;
    size_t offset;
} header_func;
#define XX(hd, hd_mn, func)                                                    \
  { SSSTR(hd), func, offsetof(request_headers_t, hd_mn) }
static header_func hf_list[] = {
        XX("accept", accept, request_handle_hd_base),
        XX("accept-charset", accept_charset, request_handle_hd_base),
        XX("accept-encoding", accept_encoding, request_handle_hd_base),
        XX("accept-language", accept_language, request_handle_hd_base),
        XX("cache-control", cache_control, request_handle_hd_base),
        XX("content-length", content_length, request_handle_hd_content_length),
        XX("connection", connection, request_handle_hd_connection),
        XX("cookie", cookie, request_handle_hd_base),
        XX("date", date, request_handle_hd_base),
        XX("host", host, request_handle_hd_base),
        XX("if-modified-since", if_modified_since, request_handle_hd_base),
        XX("if-unmodified-since", if_unmodified_since, request_handle_hd_base),
        XX("max-forwards", max_forwards, request_handle_hd_base),
        XX("range", range, request_handle_hd_base),
        XX("referer", referer, request_handle_hd_base),
        XX("transfer-encoding", transfer_encoding,
           request_handle_hd_transfer_encoding),
        XX("user-agent", user_agent, request_handle_hd_base),
};
#undef XX
dict_t header_handler_dict;
void header_handler_dict_init() {
    dict_init(&header_handler_dict);
    size_t nsize = sizeof(hf_list) / sizeof(hf_list[0]);
    int i;
    for (i = 0; i < nsize; i++) {
        dict_put(&header_handler_dict, &hf_list[i].hd, (void *)&hf_list[i]);
    }
}
void header_handler_dict_free() { dict_free(&header_handler_dict); }
int request_init(request_t *r, connection_t *c) {
    assert(r != NULL);
    memset(r, 0, sizeof(request_t));
    r->c = c;
    r->ib = buffer_init();
    r->ob = buffer_init();
    if (r->ib == NULL || r->ob == NULL)
        return ERROR;
    parse_archive_init(&r->par, r->ib);
    r->resource_fd = -1;
    r->status_code = 200;
    r->req_handler = request_handle_request_line;
    r->res_handler = response_handle_send_buffer;
    return OK;
}
int request_reset(request_t *r) {
    buffer_t *ib = r->ib;
    buffer_t *ob = r->ob;
    connection_t *c = r->c;
    memset(r, 0, sizeof(request_t));
    r->ib = ib;
    r->ob = ob;
    r->c = c;
    buffer_clear(ib);
    buffer_clear(ob);
    parse_archive_init(&r->par, r->ib);
    r->resource_fd = -1;
    r->status_code = 200;
    r->req_handler = request_handle_request_line;
    r->res_handler = response_handle_send_buffer;
    return OK;
}
static int request_recv(request_t *r) {
    char buf[BUFSIZ];
    int len;
    while (TRUE) {
        len = recv(r->c->fd, buf, sizeof(buf), 0);

        if (len == 0) {
            return OK;
        }
        if (len == ERROR) {
            if (errno != EAGAIN) {
                lotos_log(LOG_ERR, "recv: %s", strerror(errno));
                return ERROR;
            } else
                return AGAIN;
        }
        buffer_cat(r->ib, buf, len);
    }
    return AGAIN;
}
int request_handle(connection_t *c) {
    request_t *r = &c->req;
    int status = request_recv(r);
    if (status == ERROR || status == OK)
        return ERROR;

    do {
        status = r->req_handler(r);
    } while (r->req_handler != NULL && status == OK);
    return status;
}
static int request_handle_request_line(request_t *r) {
    int status;
    status = parse_request_line(r->ib, &r->par);
    if (status == AGAIN)
        return AGAIN;
    if (status != OK) {

        return response_assemble_err_buffer(r, 400);
    }

    parse_archive *ar = &(r->par);

    if (ar->version.http_major > 1 || ar->version.http_minor > 1) {

        return response_assemble_err_buffer(r, 505);
    }
    ar->keep_alive = (ar->version.http_major == 1 && ar->version.http_minor == 1);

    char *p = ar->url.abs_path.str;
    while (*p != ' ' && *p != '?')
        p++;
    *p = '\0';

    const char *relative_path = NULL;
    relative_path = ar->url.abs_path.len == 1 && ar->url.abs_path.str[0] == '/'
                    ? "./"
                    : ar->url.abs_path.str + 1;
    int fd = openat(server_config.rootdir_fd, relative_path, O_RDONLY);
    if (fd == ERROR) {

        return response_assemble_err_buffer(r, 404);
    }
    struct stat st;
    fstat(fd, &st);
    if (S_ISDIR(st.st_mode)) {

        int html_fd = openat(fd, "index.html", O_RDONLY);
        close(fd);
        if (fd == ERROR) {

            return response_assemble_err_buffer(r, 404);
        }
        fd = html_fd;
        fstat(fd, &st);
        ssstr_set(&ar->url.mime_extension, "html");
    }
    r->resource_fd = fd;
    r->resource_size = st.st_size;
    r->req_handler = request_handle_headers;
    return OK;
}
static int request_handle_headers(request_t *r) {
    int status;
    buffer_t *b = r->ib;
    parse_archive *ar = &r->par;
    while (TRUE) {
        status = parse_header_line(b, ar);
        switch (status) {

            case AGAIN:
                return AGAIN;

            case INVALID_REQUEST:

                return response_assemble_err_buffer(r, 400);

            case CRLF_LINE:
                goto header_done;

            case OK:
                ssstr_tolower(&r->par.header[0]);

                header_func *hf = dict_get(&header_handler_dict, &r->par.header[0], NULL);
                if (hf == NULL)
                    break;
                header_handle_method func = hf->func;
                size_t offset = hf->offset;
                if (func != NULL) {
                    status = func(r, offset);
                    if (status != OK)
                        return OK;
                }
                break;
        }
    }
    header_done:;
    r->req_handler = request_handle_body;
    return OK;
}
static int request_handle_body(request_t *r) {
    int status;
    buffer_t *b = r->ib;
    parse_archive *ar = &r->par;
    switch (r->par.transfer_encoding) {
        case TE_IDENTITY:
            status = parse_header_body_identity(b, ar);
            break;
        default:
            status = ERROR;
            break;
    }
    switch (status) {
        case AGAIN:
            return AGAIN;
        case OK:
            connection_disable_in(epoll_fd, r->c);
            connection_enable_out(epoll_fd, r->c);
            r->req_handler = NULL;
            response_assemble_buffer(r);
            return OK;
        default:

            return response_assemble_err_buffer(r, 501);
    }
    return OK;
}
int request_handle_hd_base(request_t *r, size_t offset) {
    parse_archive *ar = &r->par;
    ssstr_t *header = (ssstr_t *)(((char *)(&ar->req_headers)) + offset);
    *header = ar->header[1];
    return OK;
}
int request_handle_hd_connection(request_t *r, size_t offset) {
    request_handle_hd_base(r, offset);
    ssstr_t *connection = &(r->par.req_headers.connection);
    if (ssstr_caseequal(connection, "keep-alive")) {
        r->par.keep_alive = TRUE;
    } else if (ssstr_caseequal(connection, "close")) {
        r->par.keep_alive = FALSE;
    } else {

        return response_assemble_err_buffer(r, 400);
    }
    return OK;
}
int request_handle_hd_content_length(request_t *r, size_t offset) {
    request_handle_hd_base(r, offset);
    ssstr_t *content_length = &(r->par.req_headers.content_length);
    int len = atoi(content_length->str);
    if (len <= 0) {

        return response_assemble_err_buffer(r, 400);
    }
    r->par.content_length = len;
    return OK;
}
int request_handle_hd_transfer_encoding(request_t *r, size_t offset) {
    request_handle_hd_base(r, offset);
    ssstr_t *transfer_encoding = &(r->par.req_headers.transfer_encoding);
    if (ssstr_caseequal(transfer_encoding, "chunked")) {

        r->par.transfer_encoding = TE_CHUNKED;
        return response_assemble_err_buffer(r, 501);
    } else if (ssstr_caseequal(transfer_encoding, "compress")) {

        r->par.transfer_encoding = TE_COMPRESS;
        return response_assemble_err_buffer(r, 501);
    } else if (ssstr_caseequal(transfer_encoding, "deflate")) {

        r->par.transfer_encoding = TE_DEFLATE;
        return response_assemble_err_buffer(r, 501);
    } else if (ssstr_caseequal(transfer_encoding, "gzip")) {

        r->par.transfer_encoding = TE_GZIP;
        return response_assemble_err_buffer(r, 501);
    } else if (ssstr_caseequal(transfer_encoding, "identity")) {
        r->par.transfer_encoding = TE_IDENTITY;
        return response_assemble_err_buffer(r, 501);
    } else {

        return response_assemble_err_buffer(r, 400);
    }
    return OK;
}
static int response_send(request_t *r) {
    int len = 0;
    buffer_t *b = r->ob;
    char *buf_beg;
    while (TRUE) {
        buf_beg = b->buf + r->par.buffer_sent;
        len = send(r->c->fd, buf_beg, buffer_end(b) - buf_beg, 0);
        if (len == 0) {
            buffer_clear(b);
            r->par.buffer_sent = 0;
            return OK;
        } else if (len < 0) {
            if (errno == EAGAIN)
                return AGAIN;
            lotos_log(LOG_ERR, "send: %s", strerror(errno));
            return ERROR;
        }
        r->par.buffer_sent += len;
    }
    return OK;
}
int response_handle(struct connection *c) {
    request_t *r = &c->req;
    int status;
    do {
        status = r->res_handler(r);
    } while (status == OK && r->par.response_done != TRUE);
    if (r->par.response_done) {
        if (r->par.keep_alive) {
            request_reset(r);
            connection_disable_out(epoll_fd, c);
            connection_enable_in(epoll_fd, c);
        } else
            return ERROR;
    }
    return status;
}
int response_handle_send_buffer(struct request *r) {
    int status;
    status = response_send(r);
    if (status != OK) {
        return status;
    } else {
        if (r->resource_fd != -1) {
            r->res_handler = response_handle_send_file;
            return OK;
        }
        r->par.response_done = TRUE;
        connection_disable_out(epoll_fd, r->c);
        return OK;
    }
    return OK;
}
int response_handle_send_file(struct request *r) {
    int len;
    connection_t *c = r->c;
    while (TRUE) {

        len = sendfile(c->fd, r->resource_fd, NULL, r->resource_size);
        if (len == 0) {
            r->par.response_done = TRUE;
            close(r->resource_fd);
            return OK;
        } else if (len < 0) {
            if (errno == EAGAIN) {
                return AGAIN;
            }
            lotos_log(LOG_ERR, "sendfile: %s", strerror(errno));
            return ERROR;
        }
    }
    return OK;
}
int response_assemble_buffer(struct request *r) {
    response_append_status_line(r);
    response_append_date(r);
    response_append_server(r);
    response_append_content_type(r);
    response_append_content_length(r);
    response_append_connection(r);
    response_append_crlf(r);
    return OK;
}
int response_assemble_err_buffer(struct request *r, int status_code) {
    r->req_handler = NULL;
    r->par.err_req = TRUE;
    r->status_code = status_code;
    response_append_status_line(r);
    response_append_date(r);
    response_append_server(r);
    response_append_content_type(r);
    response_append_content_length(r);
    r->par.keep_alive = FALSE;
    response_append_connection(r);
    response_append_crlf(r);

    r->ob = buffer_cat_cstr(r->ob, err_page_render_buf());
    connection_disable_in(epoll_fd, r->c);
    connection_enable_out(epoll_fd, r->c);
    r->par.response_done = TRUE;
    return OK;
}
