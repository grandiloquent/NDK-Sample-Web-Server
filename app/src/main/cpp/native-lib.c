#include <jni.h>
#include "Utilities.h"
#include "mongoose/mongoose.h"
#include "strbuf.h"
#include "defines.h"

static sqlite3 *s_db;
static struct mg_serve_http_opts s_http_server_opts;

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
    return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}
static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
    return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

sqlite3 *create_database(const char *filename) {
    sqlite3 *db;
    int rc = sqlite3_open(filename, &db);
    if (rc) {
        LOGE("Can't open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    const char *sql = "CREATE TABLE IF NOT EXISTS note (\n" \
                      "    _id INTEGER PRIMARY KEY AUTOINCREMENT,\n" \
                      "    title VARCHAR(50) NOT NULL,\n" \
                      "    content TEXT NOT NULL,\n" \
                      "    created_at BIGINT NOT NULL,\n" \
                      "    updated_at BIGINT NOT NULL,\n" \
                      "    UNIQUE (created_at)\n" \
                      ");";
    char *errMsg;
    rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error:%s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return db;
}
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    static const struct mg_str api_index = MG_MK_STR("/");
    static const struct mg_str api_get = MG_MK_STR("/api/get/");
    static const struct mg_str api_update = MG_MK_STR("/api/update");
    struct http_message *hm = (struct http_message *) ev_data;
    if (ev == MG_EV_HTTP_REQUEST) {
        if (is_equal(&hm->uri, &api_index)) {
            index(nc, hm);
        } else if (has_prefix(&hm->uri, &api_get)) {
            GetJSON(nc, hm, atoi(hm->uri.p + api_get.len));
        } else if (is_equal(&hm->uri, &api_update)) {
            UpdateJSON(nc, hm);
        } else {
            LOGE("ev_handler: %s\n", s_http_server_opts.document_root);
            mg_serve_http(nc, hm, s_http_server_opts);
        }
    }
}
static void index(struct mg_connection *nc, const struct http_message *hm) {
    sqlite3_stmt *stmt = NULL;
    const char *data = NULL;
    int result;
    (void) hm;
    if (sqlite3_prepare_v2(s_db, "select _id,title from note order by updated_at desc", -1, &stmt,
                           NULL) == SQLITE_OK) {
        struct strbuf name = STRBUF_INIT;
        while (1) {
            result = sqlite3_step(stmt);
            if (result == SQLITE_ROW) {
                data = (char *) sqlite3_column_text(stmt, 1);
                strbuf_addf(
                        &name,
                        "<li class=\"nav-tree__item\"><a class=\"nav-tree__link\" href=\"#%d\">%s</a></li>",
                        sqlite3_column_int(stmt, 0), data);
            } else {
                break;
            }
        }
        sqlite3_finalize(stmt);
        mg_send_head(nc, 200, S_INDEX_T1 + S_INDEX_T2 + name.len,
                     "Content-Type: text/html; charset=utf-8");
        mg_send(nc, s_index_t1, S_INDEX_T1);
        mg_send(nc, name.buf, (int) name.len);
        strbuf_release(&name);
        mg_send(nc, s_index_t2, S_INDEX_T2);
    } else {
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Server Error\r\n"
                  "Content-Length: 0\r\n\r\n");
    }
}
void start_server(const char *address) {
    struct mg_mgr mgr;
    struct mg_connection *nc;
    int i;
    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, address, ev_handler);
    mg_set_protocol_http_websocket(nc);
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
}

JNIEXPORT jint JNICALL
Java_euphoria_psycho_server_MainActivity_startServer(JNIEnv *env, jclass type, jstring host_,
                                                     jint port, jstring staticDirectory_,
                                                     jstring databasePath_) {
    const char *host = (*env)->GetStringUTFChars(env, host_, 0);
    const char *staticDirectory = (*env)->GetStringUTFChars(env, staticDirectory_, 0);
    const char *databasePath = (*env)->GetStringUTFChars(env, databasePath_, 0);

    s_db = create_database(databasePath);


    (*env)->ReleaseStringUTFChars(env, host_, host);
    (*env)->ReleaseStringUTFChars(env, staticDirectory_, staticDirectory);
    (*env)->ReleaseStringUTFChars(env, databasePath_, databasePath);
}


