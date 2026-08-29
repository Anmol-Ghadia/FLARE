// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
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

static int read_int_range(const uint8_t **data, size_t *size, int minv, int maxv) {
  if (maxv <= minv) return minv;
  uint32_t v = read_u32(data, size);
  return minv + (int)(v % (uint32_t)(maxv - minv + 1));
}

static size_t take_bytes(const uint8_t **data, size_t *size, uint8_t *out, size_t maxlen) {
  size_t n = *size;
  if (n > maxlen) n = maxlen;
  if (n > 0) {
    memcpy(out, *data, n);
    *data += n;
    *size -= n;
  }
  return n;
}

static char *make_string(const uint8_t **data, size_t *size, size_t maxlen) {
  size_t want = 0;
  if (*size > 0) {
    want = (*data)[0] % (maxlen + 1);
    (*data)++;
    (*size)--;
  }
  char *s = (char *)malloc(want + 1);
  if (!s) return NULL;
  size_t got = take_bytes(data, size, (uint8_t *)s, want);
  for (size_t i = 0; i < got; ++i) {
    if (s[i] == '\0') s[i] = 'A';
  }
  s[got] = '\0';
  return s;
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int mode = 0;
  void *p = sqlite3_user_data(ctx);
  if (p) mode = *(int *)p;

  if (argc == 0 || mode % 5 == 0) {
    sqlite3_result_null(ctx);
    return;
  }

  switch (mode % 5) {
    case 1:
      sqlite3_result_int(ctx, sqlite3_value_type(argv[0]));
      break;
    case 2:
      sqlite3_result_int64(ctx, argc > 0 ? sqlite3_value_int64(argv[0]) : 0);
      break;
    case 3: {
      const unsigned char *txt = argc > 0 ? sqlite3_value_text(argv[0]) : (const unsigned char *)"";
      sqlite3_result_text(ctx, (const char *)txt, -1, SQLITE_TRANSIENT);
      break;
    }
    default:
      sqlite3_result_double(ctx, argc > 0 ? sqlite3_value_double(argv[0]) : 0.0);
      break;
  }
}

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  if (!acc) return;
  for (int i = 0; i < argc; ++i) {
    *acc += sqlite3_value_bytes(argv[i]);
    *acc += sqlite3_value_type(argv[i]);
  }
}

static void fuzz_final(sqlite3_context *ctx) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  if (acc) sqlite3_result_int64(ctx, *acc);
  else sqlite3_result_null(ctx);
}

static void destroy_app(void *p) {
  free(p);
}

static int fuzz_collation(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  int mode = pArg ? *(int *)pArg : 0;
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  int n = n1 < n2 ? n1 : n2;
  int rc = memcmp(a, b, (size_t)n);
  if (rc == 0) {
    if (n1 < n2) rc = -1;
    else if (n1 > n2) rc = 1;
  }
  if (mode & 1) rc = -rc;
  return rc;
}

static void destroy_collation(void *p) {
  free(p);
}

static void rollback_cb(void *pArg) {
  volatile int *counter = (volatile int *)pArg;
  if (counter) {
    (*counter)++;
  }
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput_115(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (remaining > 0) {
    if (p[0] & 1) flags |= SQLITE_OPEN_MEMORY;
    p++;
    remaining--;
  }

  if (sqlite3_open_v2("./dummy_file", &db, flags, NULL) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    sqlite3_close(NULL);
    return 0;
  }

  volatile int rollback_counter = 0;
  (void)sqlite3_rollback_hook(db, rollback_cb, (void *)&rollback_counter);

  exec_sql(db, "PRAGMA foreign_keys=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b);");
  exec_sql(db, "DELETE FROM t;");
  exec_sql(db, "INSERT INTO t(a,b) VALUES(1,'x'),(2,'y'),(3,'z');");
  (void)sqlite3_changes(db);

  char *func_name1 = make_string(&p, &remaining, 32);
  char *func_name2 = make_string(&p, &remaining, 32);
  char *coll_name = make_string(&p, &remaining, 32);

  if (!func_name1 || func_name1[0] == '\0') {
    free(func_name1);
    func_name1 = (char *)malloc(8);
    if (func_name1) memcpy(func_name1, "fuzzfn1", 8);
  }
  if (!func_name2 || func_name2[0] == '\0') {
    free(func_name2);
    func_name2 = (char *)malloc(8);
    if (func_name2) memcpy(func_name2, "fuzzfn2", 8);
  }
  if (!coll_name || coll_name[0] == '\0') {
    free(coll_name);
    coll_name = (char *)malloc(9);
    if (coll_name) memcpy(coll_name, "fuzzcoll", 9);
  }

  int *app1 = (int *)malloc(sizeof(int));
  int *app2 = (int *)malloc(sizeof(int));
  int *carg = (int *)malloc(sizeof(int));
  if (app1) *app1 = read_int_range(&p, &remaining, 0, 1000);
  if (app2) *app2 = read_int_range(&p, &remaining, 0, 1000);
  if (carg) *carg = read_int_range(&p, &remaining, 0, 1000);

  int nArg1 = read_int_range(&p, &remaining, -1, 5);
  int nArg2 = read_int_range(&p, &remaining, -1, 5);

  int enc_choices[] = {
    SQLITE_UTF8,
    SQLITE_UTF16LE,
    SQLITE_UTF16BE,
    SQLITE_UTF16,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
#ifdef SQLITE_DIRECTONLY
    SQLITE_UTF8 | SQLITE_DIRECTONLY,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
#endif
  };
  int enc1 = enc_choices[read_int_range(&p, &remaining, 0, (int)(sizeof(enc_choices)/sizeof(enc_choices[0])) - 1)];
  int enc2 = enc_choices[read_int_range(&p, &remaining, 0, (int)(sizeof(enc_choices)/sizeof(enc_choices[0])) - 1)];
  int coll_enc = enc_choices[read_int_range(&p, &remaining, 0, 3)];

  if (func_name1) {
    (void)sqlite3_create_function_v2(
        db, func_name1, nArg1, enc1, app1,
        fuzz_scalar, NULL, NULL, destroy_app);
    app1 = NULL;
  } else {
    free(app1);
    app1 = NULL;
  }

  if (func_name2) {
    (void)sqlite3_create_function(
        db, func_name2, nArg2, enc2, app2,
        NULL, fuzz_step, fuzz_final);
    free(app2);
    app2 = NULL;
  } else {
    free(app2);
    app2 = NULL;
  }

  if (coll_name) {
    (void)sqlite3_create_collation_v2(
        db, coll_name, coll_enc, carg, fuzz_collation, destroy_collation);
    carg = NULL;
  } else {
    free(carg);
    carg = NULL;
  }

  if (func_name1) {
    sqlite3_stmt *stmt = NULL;
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT %s(a) FROM t;", func_name1);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
      while (sqlite3_step(stmt) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
  }

  if (func_name2) {
    sqlite3_stmt *stmt = NULL;
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT %s(a) FROM t;", func_name2);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
      while (sqlite3_step(stmt) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
  }

  if (coll_name) {
    sqlite3_stmt *stmt = NULL;
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT b FROM t ORDER BY b COLLATE %s;", coll_name);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
      while (sqlite3_step(stmt) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
  }

  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT INTO t(a,b) VALUES(99,'rollback-me');");
  (void)sqlite3_changes(db);
  exec_sql(db, "ROLLBACK;");
  (void)sqlite3_changes(db);

  if (func_name1) {
    (void)sqlite3_create_function(db, func_name1, nArg1, enc1, NULL, NULL, NULL, NULL);
  }
  if (func_name2) {
    (void)sqlite3_create_function(db, func_name2, nArg2, enc2, NULL, NULL, NULL, NULL);
  }

  free(func_name1);
  free(func_name2);
  free(coll_name);

  sqlite3_close(NULL);
  (void)sqlite3_close(db);

  return 0;
}