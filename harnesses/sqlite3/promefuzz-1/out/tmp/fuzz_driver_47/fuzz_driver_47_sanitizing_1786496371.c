#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void udf_value_inspect(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  int acc = 0;

  for (i = 0; i < argc; i++) {
    sqlite3_value *v = argv[i];
    const void *b1 = sqlite3_value_blob(v);
    int n1 = sqlite3_value_bytes(v);
    const unsigned char *t1 = sqlite3_value_text(v);
    int n2 = sqlite3_value_bytes(v);
    int iv = sqlite3_value_int(v);
    int ty1 = sqlite3_value_type(v);
    int nty = sqlite3_value_numeric_type(v);
    const void *b2 = sqlite3_value_blob(v);
    const unsigned char *t2 = sqlite3_value_text(v);
    int n3 = sqlite3_value_bytes(v);
    int ty2 = sqlite3_value_type(v);

    if (b1) acc ^= ((const unsigned char *)b1)[0];
    if (t1) acc ^= t1[0];
    if (b2) acc ^= ((const unsigned char *)b2)[0];
    if (t2) acc ^= t2[0];
    acc ^= n1 ^ n2 ^ n3 ^ iv ^ ty1 ^ ty2 ^ nty;
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
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
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
      "SELECT fuzz_value(zeroblob(?1 % 32), substr(?2,1,16), hex(?3), length(?4), ?5);"
    };

    for (i = 0; i < (int)(sizeof(sqls) / sizeof(sqls[0])); i++) {
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if (rc != SQLITE_OK || stmt == NULL) {
        if (stmt) {
          sqlite3_finalize(stmt);
          stmt = NULL;
        }
        continue;
      }

      if (sqlite3_bind_parameter_count(stmt) >= 1) {
        sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 2) {
        sqlite3_bind_text(stmt, 2, (const char *)Data, (int)Size, SQLITE_STATIC);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 3) {
        sqlite3_int64 v = 0;
        size_t n = Size < sizeof(v) ? Size : sizeof(v);
        memcpy(&v, Data, n);
        sqlite3_bind_int64(stmt, 3, v);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 4) {
        double d = 0.0;
        size_t n = Size < sizeof(d) ? Size : sizeof(d);
        memcpy(&d, Data, n);
        sqlite3_bind_double(stmt, 4, d);
      }
      if (sqlite3_bind_parameter_count(stmt) >= 5) {
        if (Size > 0) {
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