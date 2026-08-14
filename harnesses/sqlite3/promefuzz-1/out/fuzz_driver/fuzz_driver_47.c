// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static void udf_value_inspect(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  int acc = 0;

  for (i = 0; i < argc; i++) {
    sqlite3_value *v = argv[i];
    int ty1, nty, iv;
    int n1, n2, n3;
    const void *b;
    const unsigned char *t;

    ty1 = sqlite3_value_type(v);
    nty = sqlite3_value_numeric_type(v);
    iv = sqlite3_value_int(v);

    b = sqlite3_value_blob(v);
    n1 = sqlite3_value_bytes(v);
    if (b && n1 > 0) {
      acc ^= ((const unsigned char *)b)[0];
      acc ^= ((const unsigned char *)b)[n1 - 1];
    }

    t = sqlite3_value_text(v);
    n2 = sqlite3_value_bytes(v);
    if (t && n2 > 0) {
      acc ^= t[0];
      acc ^= t[n2 - 1];
    }

    b = sqlite3_value_blob(v);
    n3 = sqlite3_value_bytes(v);
    if (b && n3 > 0) {
      acc ^= ((const unsigned char *)b)[0];
    }

    acc ^= ty1 ^ nty ^ iv ^ n1 ^ n2 ^ n3 ^ sqlite3_value_type(v);
  }

  sqlite3_result_int(ctx, acc);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      sqlite3_close(db);
    }
    return 0;
  }

  sqlite3_create_function(db, "fuzz_value", -1, SQLITE_UTF8, NULL,
                          udf_value_inspect, NULL, NULL);

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c,d,e);"
               "INSERT INTO t VALUES(NULL, 123, 45.67, 'text', x'001122');"
               "INSERT INTO t VALUES('999', '-42', '3.14159', '', x'');"
               "INSERT INTO t VALUES('notnum', 0, -1, 'abc\ndef', x'FF');",
               NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sqls[] = {
      "SELECT fuzz_value(?1);",
      "SELECT fuzz_value(?1, ?2, ?3, ?4, ?5);",
      "SELECT fuzz_value(a,b,c,d,e) FROM t;",
      "SELECT fuzz_value(?1, a, ?2, b, ?3) FROM t;",
      "SELECT fuzz_value(CAST(?1 AS TEXT), CAST(?2 AS BLOB), CAST(?3 AS INTEGER), CAST(?4 AS REAL), NULL);",
      "SELECT fuzz_value(zeroblob(abs(?1) % 32), substr(?2,1,16), hex(?3), length(?4), ?5);"
    };

    for (i = 0; i < (int)(sizeof(sqls) / sizeof(sqls[0])); i++) {
      sqlite3_int64 v = 0;
      double d = 0.0;
      size_t n;

      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if (rc != SQLITE_OK || stmt == NULL) {
        if (stmt) {
          sqlite3_finalize(stmt);
          stmt = NULL;
        }
        continue;
      }

      n = Size < sizeof(v) ? Size : sizeof(v);
      if (n > 0) {
        memcpy(&v, Data, n);
      }
      n = Size < sizeof(d) ? Size : sizeof(d);
      if (n > 0) {
        memcpy(&d, Data, n);
      }

      if (sqlite3_bind_parameter_count(stmt) >= 1) {
        sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 2) {
        sqlite3_bind_text(stmt, 2, (const char *)Data, (int)Size, SQLITE_STATIC);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 3) {
        sqlite3_bind_int64(stmt, 3, v);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 4) {
        sqlite3_bind_double(stmt, 4, d);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 5) {
        if (Size & 1) {
          sqlite3_bind_null(stmt, 5);
        } else {
          sqlite3_bind_text(stmt, 5, "", 0, SQLITE_STATIC);
        }
      }

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }

      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_close(db);
  return 0;
}