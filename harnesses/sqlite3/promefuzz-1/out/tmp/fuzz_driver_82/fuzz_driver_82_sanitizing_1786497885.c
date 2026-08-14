#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

struct FuzzUserData {
  const uint8_t *data;
  size_t size;
};

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  const struct FuzzUserData *ud = (const struct FuzzUserData *)sqlite3_user_data(ctx);
  uint32_t len = 0;
  const uint8_t *bytes = NULL;
  size_t size = 0;
  const char *text = "";
  int mode = 0;

  (void)argv;

  if (!ud) {
    sqlite3_result_error_nomem(ctx);
    return;
  }

  bytes = ud->data;
  size = ud->size;

  if (!bytes || size == 0) {
    sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
    if (argc > 0) {
      (void)sqlite3_get_auxdata(ctx, 0);
    }
    return;
  }

  mode = bytes[0] % 5;

  if (size >= 5) {
    memcpy(&len, bytes + 1, sizeof(len));
    text = (const char *)(bytes + 5);
    if (len > size - 5) {
      len = (uint32_t)(size - 5);
    }
  } else {
    len = 0;
    text = "";
  }

  switch (mode) {
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
    (void)sqlite3_get_auxdata(ctx, (int)(bytes[0] & 0x7F));
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  struct FuzzUserData ud;
  FILE *fp;

  if (sqlite3_initialize() != SQLITE_OK) {
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
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

  ud.data = Data;
  ud.size = Size;

  (void)sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, &ud, udf_target, NULL, NULL);
  (void)sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  (void)sqlite3_exec(db, "INSERT INTO t(x) VALUES(1),(2),(3);", NULL, NULL, NULL);

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
          (void)sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
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

  if (Size > 0) {
    size_t sql_len = Size > 4096 ? 4096 : Size;
    stmt = NULL;
    rc = sqlite3_prepare_v2(db, (const char *)Data, (int)sql_len, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_close(db);
  return 0;
}