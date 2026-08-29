// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_vtab_collation at sqlite3.c:139787:24 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
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

static char *read_string(const uint8_t **p, size_t *n, size_t max_len) {
  size_t len = 0;
  if (*n > 0) {
    len = (**p) % (max_len + 1);
    (*p)++;
    (*n)--;
  }
  if (len > *n) len = *n;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int mode = argc > 0 ? sqlite3_value_type(argv[0]) : SQLITE_NULL;
  switch (mode % 5) {
    case 0:
      sqlite3_result_null(ctx);
      break;
    case 1:
      sqlite3_result_int(ctx, argc);
      break;
    case 2:
      if (argc > 0) {
        const unsigned char *t = sqlite3_value_text(argv[0]);
        if (t) sqlite3_result_text(ctx, (const char *)t, -1, SQLITE_TRANSIENT);
        else sqlite3_result_null(ctx);
      } else {
        sqlite3_result_text(ctx, "x", 1, SQLITE_STATIC);
      }
      break;
    case 3:
      sqlite3_result_int64(ctx, argc > 1 ? sqlite3_value_int64(argv[1]) : 0);
      break;
    default:
      sqlite3_result_error(ctx, "fuzz", 4);
      break;
  }
}

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, sizeof(int));
  if (!acc) return;
  *acc += argc;
  if (argc > 0 && sqlite3_value_type(argv[0]) == SQLITE_INTEGER) {
    *acc += sqlite3_value_int(argv[0]) & 0xff;
  }
}

static void fuzz_final(sqlite3_context *ctx) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, 0);
  if (acc) sqlite3_result_int(ctx, *acc);
  else sqlite3_result_null(ctx);
}

static int fuzz_collation(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  (void)pArg;
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  int n = n1 < n2 ? n1 : n2;
  for (int i = 0; i < n; ++i) {
    if (a[i] != b[i]) return (int)a[i] - (int)b[i];
  }
  return n1 - n2;
}

static void fuzz_destroy(void *p) {
  free(p);
}

int LLVMFuzzerTestOneInput_105(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  int open_mode = read_int_range(&p, &n, 0, 3);
  int flags;
  switch (open_mode) {
    case 0:
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
      break;
    case 1:
      flags = SQLITE_OPEN_READONLY;
      break;
    case 2:
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
      break;
    default:
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY;
      break;
  }

  char *vfs_name = NULL;
  if (n > 0 && (p[0] & 1)) {
    p++;
    n--;
    vfs_name = read_string(&p, &n, 32);
  }

  const char *filename = (open_mode == 3) ? ":memory:" : "./dummy_file";
  rc = sqlite3_open_v2(filename, &db, flags, vfs_name && vfs_name[0] ? vfs_name : NULL);
  free(vfs_name);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  char *func_name1 = read_string(&p, &n, 64);
  char *func_name2 = read_string(&p, &n, 64);
  char *coll_name = read_string(&p, &n, 64);
  if (!func_name1 || !func_name2 || !coll_name) {
    free(func_name1);
    free(func_name2);
    free(coll_name);
    sqlite3_close(db);
    return 0;
  }

  int overload_narg = read_int_range(&p, &n, -1, 10);
  sqlite3_overload_function(db, func_name1[0] ? func_name1 : "f", overload_narg);

  int encs[] = {
    SQLITE_UTF8,
    SQLITE_UTF16LE,
    SQLITE_UTF16BE,
    SQLITE_UTF16,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
    SQLITE_UTF8 | SQLITE_DIRECTONLY,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
  };
  int enc1 = encs[read_int_range(&p, &n, 0, (int)(sizeof(encs) / sizeof(encs[0])) - 1)];
  int enc2 = encs[read_int_range(&p, &n, 0, (int)(sizeof(encs) / sizeof(encs[0])) - 1)];
  int narg1 = read_int_range(&p, &n, -1, 10);
  int narg2 = read_int_range(&p, &n, 0, 10);

  sqlite3_create_function(
      db,
      func_name1[0] ? func_name1 : "f",
      narg1,
      enc1,
      NULL,
      fuzz_scalar,
      NULL,
      NULL);

  sqlite3_create_function(
      db,
      func_name2[0] ? func_name2 : "aggf",
      narg2,
      enc2,
      NULL,
      NULL,
      fuzz_step,
      fuzz_final);

  char *coll_arg = (char *)malloc(4);
  if (coll_arg) memcpy(coll_arg, "COL", 4);
  sqlite3_create_collation_v2(
      db,
      coll_name[0] ? coll_name : "C",
      SQLITE_UTF8,
      coll_arg,
      fuzz_collation,
      fuzz_destroy);

  sqlite3_create_collation_v2(
      db,
      coll_name[0] ? coll_name : "C",
      SQLITE_UTF16,
      NULL,
      fuzz_collation,
      NULL);

  sqlite3_index_info idx;
  memset(&idx, 0, sizeof(idx));
  idx.nConstraint = read_int_range(&p, &n, 0, 4);
  if (idx.nConstraint > 0) {
    idx.aConstraint = (struct sqlite3_index_constraint *)
        calloc((size_t)idx.nConstraint, sizeof(*idx.aConstraint));
    idx.aConstraintUsage = (struct sqlite3_index_constraint_usage *)
        calloc((size_t)idx.nConstraint, sizeof(*idx.aConstraintUsage));
    if (idx.aConstraint) {
      for (int i = 0; i < idx.nConstraint; ++i) {
        idx.aConstraint[i].iColumn = read_int_range(&p, &n, -1, 3);
        idx.aConstraint[i].op = (unsigned char)read_int_range(&p, &n, 1, 255);
        idx.aConstraint[i].usable = (unsigned char)read_int_range(&p, &n, 0, 1);
        idx.aConstraint[i].iTermOffset = read_int_range(&p, &n, 0, 100);
      }
    }
  }
  idx.nOrderBy = read_int_range(&p, &n, 0, 2);
  if (idx.nOrderBy > 0) {
    idx.aOrderBy = (struct sqlite3_index_orderby *)
        calloc((size_t)idx.nOrderBy, sizeof(*idx.aOrderBy));
    if (idx.aOrderBy) {
      for (int i = 0; i < idx.nOrderBy; ++i) {
        idx.aOrderBy[i].iColumn = read_int_range(&p, &n, -1, 3);
        idx.aOrderBy[i].desc = (unsigned char)read_int_range(&p, &n, 0, 1);
      }
    }
  }
  (void)sqlite3_vtab_collation(&idx, read_int_range(&p, &n, -2, idx.nConstraint + 2));

  size_t sql_len = n;
  char *sql = (char *)malloc(sql_len + 1);
  if (sql) {
    if (sql_len) memcpy(sql, p, sql_len);
    sql[sql_len] = '\0';

    sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, &tail);
    if (stmt) {
      int steps = 0;
      while (steps++ < 8) {
        int s = sqlite3_step(stmt);
        if (s != SQLITE_ROW && s != SQLITE_DONE) break;
        if (s == SQLITE_DONE) break;
      }
      sqlite3_reset(stmt);
      sqlite3_finalize(stmt);
      stmt = NULL;
    }

    if (tail && *tail) {
      sqlite3_prepare_v2(db, tail, (int)strlen(tail), &stmt, NULL);
      if (stmt) {
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
    }
    free(sql);
  }

  free(idx.aConstraint);
  free(idx.aConstraintUsage);
  free(idx.aOrderBy);
  free(func_name1);
  free(func_name2);
  free(coll_name);
  sqlite3_close(db);
  return 0;
}