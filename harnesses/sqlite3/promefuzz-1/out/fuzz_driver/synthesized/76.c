// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_create_window_function at sqlite3.c:154256:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v |= ((uint32_t)(*p)[i]) << (8 * i);
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static void *dup_bytes(const uint8_t *src, size_t len) {
  void *p = malloc(len ? len : 1);
  if (!p) return NULL;
  if (len) memcpy(p, src, len);
  return p;
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t max_len) {
  size_t len = *n;
  if (len > max_len) len = max_len;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static void scalar_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 sum = 0;
  for (int i = 0; i < argc; i++) {
    switch (sqlite3_value_type(argv[i])) {
      case SQLITE_INTEGER:
        sum += sqlite3_value_int64(argv[i]);
        break;
      case SQLITE_FLOAT:
        sum += (sqlite3_int64)sqlite3_value_double(argv[i]);
        break;
      case SQLITE_TEXT:
        sum += sqlite3_value_bytes(argv[i]);
        break;
      case SQLITE_BLOB:
        sum += sqlite3_value_bytes(argv[i]);
        break;
      default:
        break;
    }
  }
  sqlite3_result_int64(ctx, sum);
}

static void agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  if (!acc) {
    sqlite3_result_error_nomem(ctx);
    return;
  }
  for (int i = 0; i < argc; i++) {
    if (sqlite3_value_type(argv[i]) == SQLITE_INTEGER) {
      *acc += sqlite3_value_int64(argv[i]);
    } else {
      *acc += sqlite3_value_bytes(argv[i]);
    }
  }
}

static void agg_final(sqlite3_context *ctx) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int64(ctx, acc ? *acc : 0);
}

static void win_value(sqlite3_context *ctx) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int64(ctx, acc ? *acc : 0);
}

static void win_inverse(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  if (!acc) {
    sqlite3_result_error_nomem(ctx);
    return;
  }
  for (int i = 0; i < argc; i++) {
    if (sqlite3_value_type(argv[i]) == SQLITE_INTEGER) {
      *acc -= sqlite3_value_int64(argv[i]);
    } else {
      *acc -= sqlite3_value_bytes(argv[i]);
    }
  }
}

static void destroy_app(void *p) {
  free(p);
}

int LLVMFuzzerTestOneInput_76(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp = NULL;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
    fp = NULL;
  }

  {
    int flags_opts[] = {
      SQLITE_OPEN_READONLY,
      SQLITE_OPEN_READWRITE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX
    };
    int flags = flags_opts[read_u32(&p, &n) % (sizeof(flags_opts) / sizeof(flags_opts[0]))];
    const char *filename = (read_u32(&p, &n) & 1) ? "./dummy_file" : ":memory:";
    const char *vfs = NULL;
    sqlite3_open_v2(filename, &db, flags, vfs);
  }

  if (!db) {
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(a,b);"
               "INSERT INTO t(a,b) VALUES(1,2),(3,4),(5,6);",
               NULL, NULL, NULL);

  for (int iter = 0; iter < 8 && n > 0; iter++) {
    int op = read_u32(&p, &n) % 6;
    int nArgRaw = read_int(&p, &n);
    int nArgChoices[] = { -1, 0, 1, 2, 3, 127 };
    int nArg = nArgChoices[((unsigned)nArgRaw) % (sizeof(nArgChoices) / sizeof(nArgChoices[0]))];
    int encChoices[] = {
      SQLITE_UTF8,
      SQLITE_UTF16,
      SQLITE_UTF16LE,
      SQLITE_UTF16BE,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC,
      SQLITE_UTF8 | SQLITE_DIRECTONLY,
      SQLITE_UTF16 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
    };
    int eTextRep = encChoices[read_u32(&p, &n) % (sizeof(encChoices) / sizeof(encChoices[0]))];
    size_t name_len = n ? (read_u32(&p, &n) % (n + 1)) : 0;
    if (name_len > 300) name_len = 300;

    const uint8_t *name_src = p;
    if (name_len > n) name_len = n;
    p += name_len;
    n -= name_len;

    char *name8 = (char *)malloc(name_len + 1);
    if (!name8) break;
    if (name_len) memcpy(name8, name_src, name_len);
    name8[name_len] = '\0';

    void *app = dup_bytes(name_src, name_len);

    switch (op) {
      case 0: {
        sqlite3_create_function(
            db, name8, nArg, eTextRep, app,
            scalar_func, NULL, NULL);
        free(app);
        break;
      }
      case 1: {
        sqlite3_create_function(
            db, name8, nArg, eTextRep, NULL,
            NULL, agg_step, agg_final);
        break;
      }
      case 2: {
        sqlite3_create_function_v2(
            db, name8, nArg, eTextRep, app,
            scalar_func, NULL, NULL, destroy_app);
        break;
      }
      case 3: {
        size_t u16_units = name_len / 2;
        void *name16 = malloc(u16_units * 2 + 2);
        if (name16) {
          if (u16_units) memcpy(name16, name_src, u16_units * 2);
          ((unsigned char *)name16)[u16_units * 2] = 0;
          ((unsigned char *)name16)[u16_units * 2 + 1] = 0;
          sqlite3_create_function16(
              db, name16, nArg, eTextRep, app,
              scalar_func, NULL, NULL);
          free(name16);
        }
        free(app);
        break;
      }
      case 4: {
        sqlite3_create_window_function(
            db, name8, nArg, eTextRep, app,
            agg_step, agg_final, win_value, win_inverse, destroy_app);
        break;
      }
      case 5: {
        sqlite3_overload_function(db, name8, nArg);
        free(app);
        break;
      }
    }

    if (name_len > 0) {
      char *sql = sqlite3_mprintf(
          "SELECT \"%w\"(a) FROM t;"
          "SELECT \"%w\"(a,b) FROM t;"
          "SELECT \"%w\"() FROM t LIMIT 1;"
          "SELECT \"%w\"(a) OVER (ORDER BY a ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) FROM t;",
          name8, name8, name8, name8);
      if (sql) {
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
          while (sqlite3_step(stmt) == SQLITE_ROW) {
          }
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
        sqlite3_free(sql);
      }
    }

    free(name8);
  }

  {
    char *tail_sql = make_cstring(&p, &n, 512);
    if (tail_sql) {
      sqlite3_exec(db, tail_sql, NULL, NULL, NULL);
      free(tail_sql);
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}