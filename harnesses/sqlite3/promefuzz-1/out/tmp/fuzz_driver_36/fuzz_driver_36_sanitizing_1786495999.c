#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 outv = 0;
  if (argc > 0 && argv[0]) {
    const unsigned char *txt = sqlite3_value_text(argv[0]);
    const void *blob = sqlite3_value_blob(argv[0]);
    int n = sqlite3_value_bytes(argv[0]);

    if (txt) {
      for (int i = 0; txt[i] != 0; ++i) {
        outv += txt[i];
      }
    }
    if (blob && n > 0) {
      const unsigned char *b = (const unsigned char *)blob;
      int limit = n > 64 ? 64 : n;
      for (int i = 0; i < limit; ++i) {
        outv ^= ((sqlite3_int64)b[i]) << ((i % 8) * 8);
      }
      outv += n;
    }
  }
  sqlite3_result_int64(ctx, outv);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

  sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);

  if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS f(y);", NULL, NULL, NULL) != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL) == SQLITE_OK) {
    if (Size == 0) {
      sqlite3_bind_null(stmt, 1);
    } else {
      switch (Data[0] % 5) {
        case 0:
          sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
          break;
        case 1:
          sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
          break;
        case 2: {
          sqlite3_int64 v = 0;
          size_t copy = Size < sizeof(v) ? Size : sizeof(v);
          memcpy(&v, Data, copy);
          sqlite3_bind_int64(stmt, 1, v);
          break;
        }
        case 3: {
          double d = 0.0;
          size_t copy = Size < sizeof(d) ? Size : sizeof(d);
          memcpy(&d, Data, copy);
          sqlite3_bind_double(stmt, 1, d);
          break;
        }
        default:
          sqlite3_bind_zeroblob(stmt, 1, (int)(Size > 4096 ? 4096 : Size));
          break;
      }
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS TEXT));", -1, &stmt, NULL) == SQLITE_OK) {
    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS BLOB));", -1, &stmt, NULL) == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_exec(db, "DROP TABLE IF EXISTS t;", NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);

  sqlite3_close(db);
  return 0;
}