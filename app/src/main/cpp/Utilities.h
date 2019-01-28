
#include <android/log.h>
#include "sqlite/sqlite3.h"
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, "TAG", __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, "TAG", __VA_ARGS__))

int
sqlite3_prepare_statement(sqlite3 *db, sqlite3_stmt **stmt, const char *query, unsigned retries);

bool sqlite3_command(sqlite3 *db, const char *query, unsigned retries);

bool sqlite3_exists(sqlite3 *db, const char *tableName, const char *keyName, const char *keyData,
                    unsigned retries);

int sqlite3_run_query(sqlite3 *db, sqlite3_stmt *stmt, unsigned retries);

bool sqlite3_single_lookup(sqlite3 *db, const char *tableName,
                           const char *keyName, unsigned keyData,
                           const char *valueName, char *valueData, unsigned retries);


int
sqlite3_prepare_statement(sqlite3 *db, sqlite3_stmt **stmt, const char *query, unsigned retries) {
  int src = SQLITE_BUSY;
  for (unsigned i = 0; i < retries; i++) {
    // https://www.sqlite.org/c3ref/prepare.html
    src = sqlite3_prepare_v2(db, query, (int) strlen(query), stmt, NULL);
    if (src != SQLITE_BUSY && src != SQLITE_LOCKED) {
      break;
    }
    usleep(200);
  }
  if (src) {
    fprintf(stderr, "sqlite3_prepare_v2 failed for \"%s\": %s\n", query, sqlite3_errmsg(db));
    sqlite3_finalize(*stmt);
  }
  return src;
}

bool sqlite3_command(sqlite3 *db, const char *query, unsigned retries) {
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_statement(db, &stmt, query, retries))return false;
  int src = sqlite3_run_query(db, stmt, retries);
  sqlite3_finalize(stmt);
  return (src == SQLITE_DONE || src == SQLITE_OK || src == SQLITE_ROW);
}

bool sqlite3_exists(sqlite3 *db, const char *tableName, const char *keyName, const char *keyData,
                    unsigned retries) {
  size_t stringSize = 100 + strlen(tableName) + strlen(keyName) + strlen(keyData);
  char query[stringSize];
  sprintf(query, "SELECT * FROM %s WHERE %s == \"%s\"", tableName, keyName, keyData);
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_statement(db, &stmt, query, retries))return false;
  int src = sqlite3_run_query(db, stmt, retries);
  sqlite3_finalize(stmt);
  return (src == SQLITE_ROW);
}

int sqlite3_run_query(sqlite3 *db, sqlite3_stmt *stmt, unsigned retries) {
  int src = SQLITE_BUSY;
  for (unsigned i = 0; i < retries; i++) {
    src = sqlite3_step(stmt);
    if (src != SQLITE_BUSY && src != SQLITE_LOCKED) {
      break;
    }
    usleep(200);
  }
  if ((src != SQLITE_DONE) && (src != SQLITE_ROW)) {
    fprintf(stderr, "sqlite3_run_query failed:%s: %s\n", sqlite3_sql(stmt), sqlite3_errmsg(db));
  }
  return src;
}

bool sqlite3_single_lookup(sqlite3 *db, const char *tableName,
                           const char *keyName, unsigned keyData,
                           const char *valueName, char *valueData, unsigned retries) {
  size_t stringSize = 100 + strlen(valueName) + strlen(tableName) + strlen(keyName) + 20;
  char query[stringSize];
  sprintf(query, "SELECT %s FROM %s WHERE %s==%u", valueName, tableName, keyName, keyData);
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_statement(db, &stmt, query, retries))return false;
  int src = sqlite3_run_query(db, stmt, retries);
  bool retVal = false;
  if (src == SQLITE_ROW) {
    const char *ptr = (const char *) sqlite3_column_text(stmt, 0);
    if (ptr)
      strcpy(valueData, ptr);
    retVal = true;
  }
  sqlite3_finalize(stmt);
  return retVal;
}
