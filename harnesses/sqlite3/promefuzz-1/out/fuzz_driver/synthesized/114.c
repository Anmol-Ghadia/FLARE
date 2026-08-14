// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_int(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static int read_bool(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  int v = (**data) & 1;
  (*data)++;
  (*size)--;
  return v;
}

static char *read_string(const uint8_t **data, size_t *size) {
  if (*size == 0) {
    char *s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }
  size_t len = (**data) % (*size + 1);
  (*data)++;
  (*size)--;

  if (len > *size) len = *size;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  memcpy(s, *data, len);
  s[len] = '\0';
  *data += len;
  *size -= len;
  return s;
}

static int fuzz_authorizer(void *pUserData, int action, const char *p1,
                           const char *p2, const char *dbName,
                           const char *triggerName) {
  (void)action;
  (void)p1;
  (void)p2;
  (void)dbName;
  (void)triggerName;
  const uint8_t *bytes = (const uint8_t *)pUserData;
  if (!bytes) return SQLITE_OK;
  switch (bytes[0] % 4) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    case 2: return SQLITE_DENY;
    default: return 999;
  }
}

static int fuzz_progress(void *pArg) {
  const uint8_t *bytes = (const uint8_t *)pArg;
  if (!bytes) return 0;
  return (bytes[0] & 1) ? 1 : 0;
}

static int fuzz_trace(unsigned traceFlag, void *ctx, void *p, void *x) {
  (void)traceFlag;
  (void)ctx;
  (void)p;
  (void)x;
  return 0;
}

int LLVMFuzzerTestOneInput_114(const uint8_t *Data, size_t Size) {
  const uint8_t *ptr = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (read_bool(&ptr, &remaining)) flags |= SQLITE_OPEN_URI;
  if (read_bool(&ptr, &remaining)) flags |= SQLITE_OPEN_MEMORY;

  if (sqlite3_open_v2("./dummy_file", &db, flags, NULL) != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(a,b,c);"
               "INSERT INTO t(a,b,c) VALUES(1,2,3);"
               "CREATE VIEW IF NOT EXISTS v AS SELECT * FROM t;",
               NULL, NULL, NULL);

  for (int i = 0; i < 8 && remaining > 0; ++i) {
    int choice = read_int(&ptr, &remaining) % 6;
    switch (choice) {
      case 0: {
        int sub = read_int(&ptr, &remaining) % 5;
        if (sub == 0) {
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
          int onoff = read_int(&ptr, &remaining);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
        } else if (sub == 1) {
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
          int onoff = read_int(&ptr, &remaining);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, onoff, &out);
#endif
        } else if (sub == 2) {
#ifdef SQLITE_DBCONFIG_LOOKASIDE
          int sz = (read_int(&ptr, &remaining) % 4096);
          int cnt = (read_int(&ptr, &remaining) % 64);
          void *buf = NULL;
          if (read_bool(&ptr, &remaining) && sz > 0 && cnt > 0) {
            size_t total = (size_t)sz * (size_t)cnt;
            buf = malloc(total);
          }
          sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, buf, sz, cnt);
          free(buf);
#endif
        } else if (sub == 3) {
#ifdef SQLITE_DBCONFIG_MAINDBNAME
          char *name = read_string(&ptr, &remaining);
          if (name) {
            sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, name);
            free(name);
          }
#endif
        } else {
#ifdef SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION
          int onoff = read_int(&ptr, &remaining);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, onoff, &out);
#endif
        }
        break;
      }

      case 1: {
        if (read_bool(&ptr, &remaining)) {
          sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
        } else {
          sqlite3_set_authorizer(db, NULL, NULL);
        }
        break;
      }

      case 2: {
        int onoff = read_bool(&ptr, &remaining);
        sqlite3_enable_load_extension(db, onoff);
        break;
      }

      case 3: {
        int nOps = (read_int(&ptr, &remaining) % 32) - 8;
        if (read_bool(&ptr, &remaining)) {
          sqlite3_progress_handler(db, nOps, fuzz_progress, (void *)Data);
        } else {
          sqlite3_progress_handler(db, nOps, NULL, NULL);
        }
        break;
      }

      case 4: {
        int categories[] = {
#ifdef SQLITE_LIMIT_LENGTH
          SQLITE_LIMIT_LENGTH,
#endif
#ifdef SQLITE_LIMIT_SQL_LENGTH
          SQLITE_LIMIT_SQL_LENGTH,
#endif
#ifdef SQLITE_LIMIT_COLUMN
          SQLITE_LIMIT_COLUMN,
#endif
#ifdef SQLITE_LIMIT_EXPR_DEPTH
          SQLITE_LIMIT_EXPR_DEPTH,
#endif
#ifdef SQLITE_LIMIT_COMPOUND_SELECT
          SQLITE_LIMIT_COMPOUND_SELECT,
#endif
#ifdef SQLITE_LIMIT_VDBE_OP
          SQLITE_LIMIT_VDBE_OP,
#endif
#ifdef SQLITE_LIMIT_FUNCTION_ARG
          SQLITE_LIMIT_FUNCTION_ARG,
#endif
#ifdef SQLITE_LIMIT_ATTACHED
          SQLITE_LIMIT_ATTACHED,
#endif
#ifdef SQLITE_LIMIT_LIKE_PATTERN_LENGTH
          SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
#endif
#ifdef SQLITE_LIMIT_VARIABLE_NUMBER
          SQLITE_LIMIT_VARIABLE_NUMBER,
#endif
#ifdef SQLITE_LIMIT_TRIGGER_DEPTH
          SQLITE_LIMIT_TRIGGER_DEPTH,
#endif
#ifdef SQLITE_LIMIT_WORKER_THREADS
          SQLITE_LIMIT_WORKER_THREADS,
#endif
        };
        size_t ncat = sizeof(categories) / sizeof(categories[0]);
        if (ncat > 0) {
          int id = categories[read_u32(&ptr, &remaining) % ncat];
          int newVal;
          if (read_bool(&ptr, &remaining)) {
            newVal = -1;
          } else {
            newVal = read_int(&ptr, &remaining);
          }
          sqlite3_limit(db, id, newVal);
        }
        break;
      }

      case 5: {
        unsigned mask = 0;
#ifdef SQLITE_TRACE_STMT
        if (read_bool(&ptr, &remaining)) mask |= SQLITE_TRACE_STMT;
#endif
#ifdef SQLITE_TRACE_PROFILE
        if (read_bool(&ptr, &remaining)) mask |= SQLITE_TRACE_PROFILE;
#endif
#ifdef SQLITE_TRACE_ROW
        if (read_bool(&ptr, &remaining)) mask |= SQLITE_TRACE_ROW;
#endif
#ifdef SQLITE_TRACE_CLOSE
        if (read_bool(&ptr, &remaining)) mask |= SQLITE_TRACE_CLOSE;
#endif
        if (read_bool(&ptr, &remaining)) {
          sqlite3_trace_v2(db, mask, fuzz_trace, (void *)Data);
        } else {
          sqlite3_trace_v2(db, 0, NULL, NULL);
        }
        break;
      }
    }

    if (remaining > 0 && read_bool(&ptr, &remaining)) {
      char *sql = read_string(&ptr, &remaining);
      if (sql) {
        sqlite3_stmt *stmt = NULL;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK && stmt) {
          int step_count = 0;
          while (step_count++ < 8) {
            int rc = sqlite3_step(stmt);
            if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
            if (rc == SQLITE_DONE) break;
          }
        }
        if (stmt) sqlite3_finalize(stmt);
        free(sql);
      }
    }
  }

  sqlite3_set_authorizer(db, NULL, NULL);
  sqlite3_progress_handler(db, 0, NULL, NULL);
  sqlite3_trace_v2(db, 0, NULL, NULL);
  sqlite3_enable_load_extension(db, 0);
  sqlite3_close(db);
  return 0;
}