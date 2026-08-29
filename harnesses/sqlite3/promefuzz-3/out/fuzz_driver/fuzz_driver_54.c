// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation16 at sqlite3.c:155773:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t rd32(const uint8_t *data, size_t size, size_t *off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < size; i++, (*off)++) {
    v = (v << 8) | data[*off];
  }
  return v;
}

static int pick_int(const uint8_t *data, size_t size, size_t *off, int minv, int maxv) {
  uint32_t span;
  uint32_t v;
  if (minv >= maxv) return minv;
  span = (uint32_t)(maxv - minv + 1);
  v = rd32(data, size, off);
  return minv + (int)(v % span);
}

static int pick_bool(const uint8_t *data, size_t size, size_t *off) {
  if (*off >= size) return 0;
  return (data[(*off)++] & 1) != 0;
}

static char *make_utf8_name(const uint8_t *data, size_t size, size_t *off, size_t maxlen) {
  size_t remain = (*off < size) ? (size - *off) : 0;
  size_t len = remain ? (data[(*off)++] % (maxlen + 1)) : 0;
  size_t i;
  char *s;

  if (len > remain - (remain ? 1 : 0)) len = remain ? (remain - 1) : 0;
  if (len == 0) len = 1;

  s = (char *)malloc(len + 1);
  if (!s) return NULL;

  for (i = 0; i < len; i++) {
    unsigned char c = (*off < size) ? data[(*off)++] : 0;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
      s[i] = (char)c;
    } else {
      s[i] = (char)('a' + (c % 26));
    }
  }
  s[len] = '\0';
  return s;
}

static void *make_utf16le_name_from_utf8(const char *s, size_t *out_bytes) {
  size_t len = strlen(s);
  size_t bytes = (len + 1) * 2;
  unsigned char *buf = (unsigned char *)malloc(bytes);
  size_t i;
  if (!buf) return NULL;
  for (i = 0; i < len; i++) {
    buf[2 * i] = (unsigned char)s[i];
    buf[2 * i + 1] = 0;
  }
  buf[2 * len] = 0;
  buf[2 * len + 1] = 0;
  if (out_bytes) *out_bytes = bytes;
  return buf;
}

static int choose_encoding(const uint8_t *data, size_t size, size_t *off, int for_func) {
  static const int func_encs[] = {
    SQLITE_UTF8,
    SQLITE_UTF16LE,
    SQLITE_UTF16BE,
    SQLITE_UTF16,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
    SQLITE_UTF8 | SQLITE_DIRECTONLY,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY,
    SQLITE_UTF16LE | SQLITE_DETERMINISTIC,
    SQLITE_UTF16BE | SQLITE_DIRECTONLY
  };
  static const int coll_encs[] = {
    SQLITE_UTF8,
    SQLITE_UTF16LE,
    SQLITE_UTF16BE,
    SQLITE_UTF16
#ifdef SQLITE_UTF16_ALIGNED
    ,SQLITE_UTF16_ALIGNED
#endif
  };
  if (for_func) {
    return func_encs[pick_int(data, size, off, 0, (int)(sizeof(func_encs) / sizeof(func_encs[0])) - 1)];
  }
  return coll_encs[pick_int(data, size, off, 0, (int)(sizeof(coll_encs) / sizeof(coll_encs[0])) - 1)];
}

static void scalar_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int mode = 0;
  int i;
  sqlite3_int64 sum = 0;
  int bytes = 0;

  if (sqlite3_user_data(ctx)) {
    mode = (*(unsigned char *)sqlite3_user_data(ctx)) & 7;
  }

  switch (mode) {
    case 0:
      sqlite3_result_null(ctx);
      break;
    case 1:
      for (i = 0; i < argc; i++) sum += sqlite3_value_int64(argv[i]);
      sqlite3_result_int64(ctx, sum);
      break;
    case 2:
      if (argc > 0) {
        const unsigned char *txt = sqlite3_value_text(argv[0]);
        if (txt) sqlite3_result_text(ctx, (const char *)txt, -1, SQLITE_TRANSIENT);
        else sqlite3_result_null(ctx);
      } else {
        sqlite3_result_text(ctx, "x", 1, SQLITE_STATIC);
      }
      break;
    case 3:
      if (argc > 0) {
        const void *blob = sqlite3_value_blob(argv[0]);
        bytes = sqlite3_value_bytes(argv[0]);
        sqlite3_result_blob(ctx, blob, bytes, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_blob(ctx, "", 0, SQLITE_STATIC);
      }
      break;
    case 4:
      sqlite3_result_int(ctx, argc);
      break;
    case 5:
      sqlite3_result_error(ctx, "fuzz", 4);
      break;
    case 6:
      if (argc > 0) sqlite3_result_double(ctx, sqlite3_value_double(argv[0]));
      else sqlite3_result_double(ctx, 0.0);
      break;
    default:
      sqlite3_result_text16(ctx, "ok", 4, SQLITE_STATIC);
      break;
  }
}

static void agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  int i;
  if (!acc) return;
  for (i = 0; i < argc; i++) {
    *acc += sqlite3_value_int64(argv[i]);
  }
}

static void agg_final(sqlite3_context *ctx) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  if (acc) sqlite3_result_int64(ctx, *acc);
  else sqlite3_result_null(ctx);
}

static int coll_compare(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  int mode = pArg ? ((*(unsigned char *)pArg) & 3) : 0;
  int min = n1 < n2 ? n1 : n2;
  int rc = 0;

  if (mode == 0) {
    rc = memcmp(s1, s2, (size_t)min);
    if (rc == 0) rc = (n1 > n2) - (n1 < n2);
    return rc;
  } else if (mode == 1) {
    return (n1 > n2) - (n1 < n2);
  } else if (mode == 2) {
    const unsigned char *a = (const unsigned char *)s1;
    const unsigned char *b = (const unsigned char *)s2;
    int i;
    for (i = 0; i < min; i++) {
      unsigned char ca = (unsigned char)(a[i] | 0x20);
      unsigned char cb = (unsigned char)(b[i] | 0x20);
      if (ca != cb) return (ca > cb) - (ca < cb);
    }
    return (n1 > n2) - (n1 < n2);
  }
  return 0;
}

static void destroy_ptr(void *p) {
  free(p);
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
}

static char *make_query_for_name(const char *name, int kind, int argc_hint) {
  char *sql;
  size_t need;
  const char *fmt0 = "SELECT \"%s\"();";
  const char *fmt1 = "SELECT \"%s\"(1);";
  const char *fmt2 = "SELECT \"%s\"(1,2);";
  const char *fmtv = "SELECT \"%s\"(a,b,c) FROM t;";
  const char *fmtc = "SELECT b FROM t ORDER BY b COLLATE \"%s\";";
  const char *fmt = NULL;

  if (kind == 0) {
    if (argc_hint == 0) fmt = fmt0;
    else if (argc_hint == 1) fmt = fmt1;
    else if (argc_hint == 2) fmt = fmt2;
    else fmt = fmtv;
  } else {
    fmt = fmtc;
  }

  need = strlen(fmt) + strlen(name) + 32;
  sql = (char *)malloc(need);
  if (!sql) return NULL;
  snprintf(sql, need, fmt, name);
  return sql;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  size_t off = 0;
  int i, ops;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);");
  exec_sql(db, "DELETE FROM t;");
  exec_sql(db, "INSERT INTO t VALUES(1,'one',x'01');");
  exec_sql(db, "INSERT INTO t VALUES(2,'two',x'0203');");
  exec_sql(db, "ATTACH DATABASE './dummy_file' AS aux;");

  ops = pick_int(Data, Size, &off, 1, 16);
  for (i = 0; i < ops; i++) {
    int which = pick_int(Data, Size, &off, 0, 5);
    char *name = make_utf8_name(Data, Size, &off, 32);
    int nArg = pick_int(Data, Size, &off, -1, 8);
    int enc;
    int rc;
    unsigned char *app = NULL;
    int app_owned_by_sqlite = 0;

    if (!name) break;
    enc = (which <= 2) ? choose_encoding(Data, Size, &off, 1)
                       : choose_encoding(Data, Size, &off, 0);

    app = (unsigned char *)malloc(1);
    if (app) {
      *app = (unsigned char)pick_int(Data, Size, &off, 0, 255);
    }

    switch (which) {
      case 0: {
        void (*xFunc)(sqlite3_context *, int, sqlite3_value **) = NULL;
        void (*xStep)(sqlite3_context *, int, sqlite3_value **) = NULL;
        void (*xFinal)(sqlite3_context *) = NULL;

        if (pick_bool(Data, Size, &off)) {
          xFunc = scalar_func;
        } else if (pick_bool(Data, Size, &off)) {
          xStep = agg_step;
          xFinal = agg_final;
        }
        rc = sqlite3_create_function(db, name, nArg, enc, app, xFunc, xStep, xFinal);
        if (rc == SQLITE_OK) {
          if (xFunc || (xStep && xFinal)) {
            app_owned_by_sqlite = 1;
          } else if (app) {
            free(app);
            app = NULL;
          }
        } else if (app) {
          free(app);
          app = NULL;
        }
        break;
      }
      case 1: {
        void (*xFunc)(sqlite3_context *, int, sqlite3_value **) = NULL;
        void (*xStep)(sqlite3_context *, int, sqlite3_value **) = NULL;
        void (*xFinal)(sqlite3_context *) = NULL;
        void (*xDestroy)(void *) = destroy_ptr;

        if (pick_bool(Data, Size, &off)) {
          xFunc = scalar_func;
        } else if (pick_bool(Data, Size, &off)) {
          xStep = agg_step;
          xFinal = agg_final;
        }
        rc = sqlite3_create_function_v2(db, name, nArg, enc, app, xFunc, xStep, xFinal, xDestroy);
        if (rc == SQLITE_OK) {
          app_owned_by_sqlite = 1;
        } else {
          app = NULL;
        }
        break;
      }
      case 2: {
        void *name16;
        size_t name16_bytes = 0;
        void (*xFunc)(sqlite3_context *, int, sqlite3_value **) = NULL;
        void (*xStep)(sqlite3_context *, int, sqlite3_value **) = NULL;
        void (*xFinal)(sqlite3_context *) = NULL;

        name16 = make_utf16le_name_from_utf8(name, &name16_bytes);
        (void)name16_bytes;
        if (pick_bool(Data, Size, &off)) {
          xFunc = scalar_func;
        } else if (pick_bool(Data, Size, &off)) {
          xStep = agg_step;
          xFinal = agg_final;
        }
        rc = sqlite3_create_function16(db, name16 ? name16 : (const void *)L"x", nArg, enc, app, xFunc, xStep, xFinal);
        if (rc == SQLITE_OK) {
          if (xFunc || (xStep && xFinal)) {
            app_owned_by_sqlite = 1;
          } else if (app) {
            free(app);
            app = NULL;
          }
        } else if (app) {
          free(app);
          app = NULL;
        }
        free(name16);
        break;
      }
      case 3: {
        int (*xCompare)(void *, int, const void *, int, const void *) = NULL;
        if (pick_bool(Data, Size, &off)) xCompare = coll_compare;
        rc = sqlite3_create_collation(db, name, enc, app, xCompare);
        if (rc == SQLITE_OK) {
          if (xCompare) {
            app_owned_by_sqlite = 1;
          } else if (app) {
            free(app);
            app = NULL;
          }
        } else if (app) {
          free(app);
          app = NULL;
        }
        break;
      }
      case 4: {
        int (*xCompare)(void *, int, const void *, int, const void *) = NULL;
        void (*xDestroy)(void *) = destroy_ptr;
        if (pick_bool(Data, Size, &off)) xCompare = coll_compare;
        rc = sqlite3_create_collation_v2(db, name, enc, app, xCompare, xDestroy);
        if (rc == SQLITE_OK) {
          app_owned_by_sqlite = 1;
        } else {
          app = NULL;
        }
        break;
      }
      case 5: {
        void *name16;
        size_t name16_bytes = 0;
        int (*xCompare)(void *, int, const void *, int, const void *) = NULL;
        name16 = make_utf16le_name_from_utf8(name, &name16_bytes);
        (void)name16_bytes;
        if (pick_bool(Data, Size, &off)) xCompare = coll_compare;
        rc = sqlite3_create_collation16(db, name16 ? name16 : (const void *)L"c", enc, app, xCompare);
        if (rc == SQLITE_OK) {
          if (xCompare) {
            app_owned_by_sqlite = 1;
          } else if (app) {
            free(app);
            app = NULL;
          }
        } else if (app) {
          free(app);
          app = NULL;
        }
        free(name16);
        break;
      }
    }

    if (pick_bool(Data, Size, &off)) {
      sqlite3_stmt *stmt = NULL;
      char *sql = NULL;

      if (which <= 2) {
        sql = make_query_for_name(name, 0, nArg);
      } else {
        sql = make_query_for_name(name, 1, nArg);
      }

      if (sql) {
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (stmt) {
          while (sqlite3_step(stmt) == SQLITE_ROW) {
          }
        }
        sqlite3_finalize(stmt);
        free(sql);
      }
    }

    if (!app_owned_by_sqlite && app) {
      free(app);
      app = NULL;
    }
    free(name);
  }

  exec_sql(db, "SELECT 1;");
  sqlite3_close(db);
  return 0;
}