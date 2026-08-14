#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;

  void *ud = sqlite3_user_data(ctx);
  const unsigned char *bytes = (const unsigned char *)ud;
  if (!bytes) {
    sqlite3_result_error_nomem(ctx);
    return;
  }

  uint32_t len = 0;
  memcpy(&len, bytes, sizeof(len));
  len &= 0x0FFF;

  const char *text = (const char *)(bytes + 4);

  switch (bytes[0] % 5) {
    case 0:
      sqlite3_result_text(ctx, text, -1, SQLITE_TRANSIENT);
      break;
    case 1:
      sqlite3_result_text(ctx, text, (int)len, SQLITE_TRANSIENT);
      break;
    case 2:
      sqlite3_result_int(ctx, (int)len);
      break;
    case 3:
      sqlite3_result_text16le(ctx, (const void *)text, (int)(len & ~1U), SQLITE_TRANSIENT);
      break;
    default:
      sqlite3_result_error_nomem(ctx);
      break;
  }

  if (argc > 0) {
    (void)sqlite3_get_auxdata(ctx, 0);
    (void)sqlite3_get_auxdata(ctx, (int)(bytes[1] & 0x7F));
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  if (sqlite3_initialize() != SQLITE_OK) {
    return 0;
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, (void *)Data, udf_target, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(x) VALUES(1),(2),(3);", NULL, NULL, NULL);

  {
    const char *sqls[] = {
      "SELECT fuzzfunc('abc');",
      "SELECT fuzzfunc(?1);",
      "SELECT fuzzfunc(x) FROM t;",
      "SELECT fuzzfunc('constant') FROM t;",
      "SELECT fuzzfunc(zeroblob(8));"
    };
    size_t i;
    for (i = 0; i < sizeof(sqls) / sizeof(sqls[0]); i++) {
      stmt = NULL;
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        if (strstr(sqls[i], "?1") != NULL) {
          sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
        }
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
    }
  }

  {
    const char *tail = (const char *)Data;
    size_t sql_len = Size;
    if (sql_len > 4096) sql_len = 4096;
    stmt = NULL;
    rc = sqlite3_prepare_v2(db, tail, (int)sql_len, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  if (errmsg) {
    sqlite3_free(errmsg);
  }
  sqlite3_close(db);
  return 0;
}