// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_create_window_function at sqlite3.c:154256:16 in sqlite3.h
// sqlite3_create_window_function at sqlite3.c:154256:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
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
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int_range(const uint8_t **p, size_t *n, int minv, int maxv) {
  if (maxv <= minv) return minv;
  uint32_t v = read_u32(p, n);
  return minv + (int)(v % (uint32_t)(maxv - minv + 1));
}

static void read_bytes(const uint8_t **p, size_t *n, uint8_t *out, size_t outlen) {
  size_t take = *n < outlen ? *n : outlen;
  if (take) memcpy(out, *p, take);
  if (outlen > take) memset(out + take, 0, outlen - take);
  *p += take;
  *n -= take;
}

static char *make_utf8_name(const uint8_t **p, size_t *n) {
  int len = read_int_range(p, n, 0, 255);
  char *s = (char *)malloc((size_t)len + 1);
  if (!s) return NULL;
  for (int i = 0; i < len; i++) {
    unsigned char c = (*n > 0) ? *(*p)++ : (unsigned char)i;
    if (*n > 0) (*n)--;
    if (c == 0) c = 'A';
    s[i] = (char)((c % 94) + 33);
  }
  s[len] = '\0';
  return s;
}

static void *make_utf16_name(const uint8_t **p, size_t *n, int be) {
  int chars = read_int_range(p, n, 0, 120);
  unsigned char *buf = (unsigned char *)malloc((size_t)(chars + 1) * 2);
  if (!buf) return NULL;
  for (int i = 0; i < chars; i++) {
    unsigned char c = (*n > 0) ? *(*p)++ : (unsigned char)('a' + (i % 26));
    if (*n > 0) (*n)--;
    if (c == 0) c = 'B';
    if (be) {
      buf[i * 2] = 0;
      buf[i * 2 + 1] = c;
    } else {
      buf[i * 2] = c;
      buf[i * 2 + 1] = 0;
    }
  }
  buf[chars * 2] = 0;
  buf[chars * 2 + 1] = 0;
  return buf;
}

static int choose_etextrep(const uint8_t **p, size_t *n) {
  static const int reps[] = {
    SQLITE_UTF8,
    SQLITE_UTF16,
    SQLITE_UTF16LE,
    SQLITE_UTF16BE,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
    SQLITE_UTF16 | SQLITE_DETERMINISTIC,
#ifdef SQLITE_DIRECTONLY
    SQLITE_UTF8 | SQLITE_DIRECTONLY,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY,
#endif
  };
  uint32_t idx = read_u32(p, n);
  return reps[idx % (sizeof(reps) / sizeof(reps[0]))];
}

static void scalar_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  if (argc <= 0 || argv == NULL) {
    sqlite3_result_null(ctx);
    return;
  }
  switch (sqlite3_value_type(argv[0])) {
    case SQLITE_INTEGER:
      sqlite3_result_int64(ctx, sqlite3_value_int64(argv[0]));
      break;
    case SQLITE_FLOAT:
      sqlite3_result_double(ctx, sqlite3_value_double(argv[0]));
      break;
    case SQLITE_TEXT: {
      const unsigned char *t = sqlite3_value_text(argv[0]);
      if (t) sqlite3_result_text(ctx, (const char *)t, -1, SQLITE_TRANSIENT);
      else sqlite3_result_null(ctx);
      break;
    }
    case SQLITE_BLOB: {
      const void *b = sqlite3_value_blob(argv[0]);
      int n = sqlite3_value_bytes(argv[0]);
      sqlite3_result_blob(ctx, b, n, SQLITE_TRANSIENT);
      break;
    }
    default:
      sqlite3_result_null(ctx);
      break;
  }
}

static void agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *sum = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  if (!sum) return;
  for (int i = 0; i < argc; i++) {
    if (sqlite3_value_type(argv[i]) == SQLITE_INTEGER) {
      *sum += sqlite3_value_int64(argv[i]);
    } else if (sqlite3_value_type(argv[i]) == SQLITE_FLOAT) {
      *sum += (sqlite3_int64)sqlite3_value_double(argv[i]);
    } else {
      *sum += sqlite3_value_bytes(argv[i]);
    }
  }
}

static void agg_final(sqlite3_context *ctx) {
  sqlite3_int64 *sum = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  if (sum) sqlite3_result_int64(ctx, *sum);
  else sqlite3_result_null(ctx);
}

static void win_value(sqlite3_context *ctx) {
  sqlite3_int64 *sum = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  if (sum) sqlite3_result_int64(ctx, *sum);
  else sqlite3_result_int64(ctx, 0);
}

static void win_inverse(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *sum = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  if (!sum) return;
  for (int i = 0; i < argc; i++) {
    if (sqlite3_value_type(argv[i]) == SQLITE_INTEGER) {
      *sum -= sqlite3_value_int64(argv[i]);
    } else if (sqlite3_value_type(argv[i]) == SQLITE_FLOAT) {
      *sum -= (sqlite3_int64)sqlite3_value_double(argv[i]);
    } else {
      *sum -= sqlite3_value_bytes(argv[i]);
    }
  }
}

static void app_destroy(void *p) {
  free(p);
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
}

static void prepare_and_step(sqlite3 *db, const char *sql) {
  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK && stmt) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
  }
  sqlite3_finalize(stmt);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA journal_mode=OFF;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);");
  exec_sql(db, "DELETE FROM t;");
  exec_sql(db, "INSERT INTO t VALUES(1,'x',3.14),(2,'yy',4.5),(3,NULL,-7);");

  for (int iter = 0; iter < 12 && n > 0; iter++) {
    int op = read_int_range(&p, &n, 0, 5);
    int nArg = read_int_range(&p, &n, -1, 8);
    int eTextRep = choose_etextrep(&p, &n);

    char *name = make_utf8_name(&p, &n);
    if (!name || name[0] == '\0') {
      free(name);
      name = (char *)malloc(8);
      if (!name) break;
      memcpy(name, "fuzzfn", 7);
    }

    switch (op) {
      case 0: {
        void *app = malloc((size_t)read_int_range(&p, &n, 0, 64) + 1);
        if (app) memset(app, 0x41, (size_t)read_int_range(&p, &n, 0, 64));
        int mode = read_int_range(&p, &n, 0, 2);
        if (mode == 0) {
          sqlite3_create_function_v2(db, name, nArg, eTextRep, app,
                                     scalar_func, NULL, NULL, app_destroy);
        } else if (mode == 1) {
          sqlite3_create_function_v2(db, name, nArg, eTextRep, app,
                                     NULL, agg_step, agg_final, app_destroy);
        } else {
          sqlite3_create_function_v2(db, name, nArg, eTextRep, app,
                                     NULL, NULL, NULL, app_destroy);
        }
        break;
      }
      case 1: {
        sqlite3_overload_function(db, name, nArg);
        break;
      }
      case 2: {
        int be = read_int_range(&p, &n, 0, 1);
        void *name16 = make_utf16_name(&p, &n, be);
        if (name16) {
          int mode = read_int_range(&p, &n, 0, 2);
          if (mode == 0) {
            sqlite3_create_function16(db, name16, nArg, eTextRep, NULL,
                                      scalar_func, NULL, NULL);
          } else if (mode == 1) {
            sqlite3_create_function16(db, name16, nArg, eTextRep, NULL,
                                      NULL, agg_step, agg_final);
          } else {
            sqlite3_create_function16(db, name16, nArg, eTextRep, NULL,
                                      NULL, NULL, NULL);
          }
          free(name16);
        }
        break;
      }
      case 3: {
        void *app = malloc((size_t)read_int_range(&p, &n, 0, 32) + 1);
        if (app) memset(app, 0x42, (size_t)read_int_range(&p, &n, 0, 32));
        int mode = read_int_range(&p, &n, 0, 1);
        if (mode == 0) {
          sqlite3_create_window_function(db, name, nArg, eTextRep, app,
                                         agg_step, agg_final,
                                         win_value, win_inverse,
                                         app_destroy);
        } else {
          sqlite3_create_window_function(db, name, nArg, eTextRep, app,
                                         agg_step, agg_final,
                                         NULL, NULL,
                                         app_destroy);
        }
        break;
      }
      case 4: {
        int mode = read_int_range(&p, &n, 0, 2);
        if (mode == 0) {
          sqlite3_create_function(db, name, nArg, eTextRep, NULL,
                                  scalar_func, NULL, NULL);
        } else if (mode == 1) {
          sqlite3_create_function(db, name, nArg, eTextRep, NULL,
                                  NULL, agg_step, agg_final);
        } else {
          sqlite3_create_function(db, name, nArg, eTextRep, NULL,
                                  NULL, NULL, NULL);
        }
        break;
      }
      case 5: {
        const char *keep_list[4];
        char *k1 = make_utf8_name(&p, &n);
        char *k2 = make_utf8_name(&p, &n);
        int mode = read_int_range(&p, &n, 0, 2);
        if (mode == 0) {
          sqlite3_drop_modules(db, NULL);
        } else if (mode == 1) {
          keep_list[0] = "fts5";
          keep_list[1] = NULL;
          sqlite3_drop_modules(db, keep_list);
        } else {
          keep_list[0] = (k1 && k1[0]) ? k1 : "x";
          keep_list[1] = (k2 && k2[0]) ? k2 : "y";
          keep_list[2] = "fts3";
          keep_list[3] = NULL;
          sqlite3_drop_modules(db, keep_list);
        }
        free(k1);
        free(k2);
        break;
      }
    }

    {
      char sql[1024];
      int which = read_int_range(&p, &n, 0, 4);
      switch (which) {
        case 0:
          snprintf(sql, sizeof(sql), "SELECT \"%s\"(a) FROM t;", name);
          break;
        case 1:
          snprintf(sql, sizeof(sql), "SELECT \"%s\"(a,b,c) FROM t;", name);
          break;
        case 2:
          snprintf(sql, sizeof(sql), "SELECT \"%s\"(*) FROM t;", name);
          break;
        case 3:
          snprintf(sql, sizeof(sql),
                   "SELECT \"%s\"(a) OVER (ORDER BY a ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) FROM t;",
                   name);
          break;
        default:
          snprintf(sql, sizeof(sql), "SELECT \"%s\"() FROM t;", name);
          break;
      }
      prepare_and_step(db, sql);
    }

    free(name);
  }

  sqlite3_close(db);
  return 0;
}