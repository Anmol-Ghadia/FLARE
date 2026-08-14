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

static void udf_result_fuzz(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  if (!ctx) return;

  sqlite3_value *v0 = argc > 0 ? argv[0] : NULL;
  sqlite3_value *v1 = argc > 1 ? argv[1] : NULL;
  sqlite3_value *v2 = argc > 2 ? argv[2] : NULL;

  sqlite3_result_null(ctx);

  if (v0) {
    sqlite3_result_value(ctx, v0);
  }

  if (v0) {
    const unsigned char *t = sqlite3_value_text(v0);
    int n = sqlite3_value_bytes(v0);
    if (t && n >= 0) {
      sqlite3_result_text64(ctx, (const char *)t, (sqlite3_uint64)n, SQLITE_TRANSIENT, SQLITE_UTF8);
      sqlite3_result_text64(ctx, (const char *)t, (sqlite3_uint64)n, SQLITE_TRANSIENT, SQLITE_UTF16);
      sqlite3_result_text64(ctx, (const char *)t, (sqlite3_uint64)n, SQLITE_TRANSIENT, SQLITE_UTF16BE);
      sqlite3_result_text64(ctx, (const char *)t, (sqlite3_uint64)n, SQLITE_TRANSIENT, SQLITE_UTF16LE);
      sqlite3_result_blob64(ctx, t, (sqlite3_uint64)n, SQLITE_TRANSIENT);
      sqlite3_result_blob(ctx, t, n, SQLITE_TRANSIENT);
      sqlite3_result_text16be(ctx, t, n, SQLITE_TRANSIENT);
    }
  }

  if (v1) {
    const void *b = sqlite3_value_blob(v1);
    int n = sqlite3_value_bytes(v1);
    if (b && n >= 0) {
      sqlite3_result_blob(ctx, b, n, SQLITE_TRANSIENT);
      sqlite3_result_blob64(ctx, b, (sqlite3_uint64)n, SQLITE_TRANSIENT);
      sqlite3_result_text16be(ctx, b, n, SQLITE_TRANSIENT);
      sqlite3_result_text64(ctx, (const char *)b, (sqlite3_uint64)n, SQLITE_TRANSIENT, SQLITE_UTF8);
    }
    sqlite3_result_value(ctx, v1);
  }

  if (v2) {
    const unsigned char *t = sqlite3_value_text(v2);
    int n = sqlite3_value_bytes(v2);
    if (t && n >= 0) {
      sqlite3_result_text64(ctx, (const char *)t, (sqlite3_uint64)n, SQLITE_TRANSIENT, SQLITE_UTF8);
      sqlite3_result_blob64(ctx, t, (sqlite3_uint64)n, SQLITE_TRANSIENT);
    }
    sqlite3_result_value(ctx, v2);
  }

  {
    static const char kEmpty[] = "";
    sqlite3_result_text64(ctx, kEmpty, 0, SQLITE_STATIC, SQLITE_UTF8);
    sqlite3_result_blob(ctx, kEmpty, 0, SQLITE_STATIC);
    sqlite3_result_blob64(ctx, kEmpty, 0, SQLITE_STATIC);
    sqlite3_result_text16be(ctx, kEmpty, 0, SQLITE_STATIC);
    sqlite3_result_null(ctx);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

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

  sqlite3_create_function(db, "fuzz_result", 3, SQLITE_UTF8, NULL, udf_result_fuzz, NULL, NULL);

  sqlite3_exec(db,
               "CREATE TABLE t(a BLOB, b BLOB, c BLOB);"
               "INSERT INTO t VALUES(X'00', X'01', X'02');",
               NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sql = "SELECT fuzz_result(?1, ?2, ?3);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      size_t n1 = Size;
      size_t n2 = Size / 2;
      size_t n3 = (Size > 0) ? (Size - n2) : 0;

      sqlite3_bind_blob(stmt, 1, Data, (int)n1, SQLITE_TRANSIENT);
      sqlite3_bind_text(stmt, 2, (const char *)Data, (int)n2, SQLITE_TRANSIENT);
      sqlite3_bind_blob(stmt, 3, Data + (Size ? (Size / 2) : 0), (int)n3, SQLITE_TRANSIENT);

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    const char *sql = "SELECT fuzz_result(a, b, c) FROM t;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    const char *sql = "SELECT fuzz_result(readfile('./dummy_file'), ?1, zeroblob(?2));";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      int zlen = (Size > 1024) ? 1024 : (int)Size;
      sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
      sqlite3_bind_int(stmt, 2, zlen);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}