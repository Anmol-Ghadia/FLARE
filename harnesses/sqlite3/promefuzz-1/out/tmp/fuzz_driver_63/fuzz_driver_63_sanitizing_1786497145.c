#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void exercise_value(sqlite3 *db, sqlite3_value *v) {
  if (!v) return;

  (void)sqlite3_value_bytes(v);
  (void)sqlite3_value_bytes16(v);
  (void)sqlite3_value_text(v);
  (void)sqlite3_value_text16le(v);
  (void)sqlite3_value_text16be(v);
  (void)sqlite3_value_blob(v);
  (void)sqlite3_errcode(db);

  (void)sqlite3_value_text(v);
  (void)sqlite3_value_bytes(v);
  (void)sqlite3_value_blob(v);
  (void)sqlite3_value_bytes16(v);
  (void)sqlite3_value_text16le(v);
  (void)sqlite3_value_text16be(v);
  (void)sqlite3_errcode(db);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_value *dupv = NULL;
  int rc;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_exec(db,
                     "CREATE TABLE t(a,b,c,d,e,f);"
                     "INSERT INTO t VALUES(NULL, 123, -45.67, x'00112233', 'hello', zeroblob(8));",
                     NULL, NULL, NULL);

  rc = sqlite3_prepare_v2(
      db,
      "SELECT "
      "NULL, "
      "123, "
      "-45.67, "
      "x'00112233', "
      "'static text', "
      "zeroblob(4), "
      "?1, ?2, ?3, ?4, ?5, ?6 "
      "UNION ALL "
      "SELECT a,b,c,d,e,f FROM t;",
      -1, &stmt, NULL);
  if (rc != SQLITE_OK || stmt == NULL) {
    sqlite3_close(db);
    return 0;
  }

  if (Size > 0) {
    int mode = Data[0] % 6;
    switch (mode) {
      case 0:
        (void)sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
        break;
      case 1:
        (void)sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
        break;
      case 2:
        (void)sqlite3_bind_text16(stmt, 1, Data, (int)(Size & ~1u), SQLITE_TRANSIENT);
        break;
      case 3:
        (void)sqlite3_bind_int64(stmt, 1, (sqlite3_int64)Size);
        break;
      case 4:
        (void)sqlite3_bind_double(stmt, 1, (double)Size / 3.0);
        break;
      default:
        (void)sqlite3_bind_null(stmt, 1);
        break;
    }

    if (Size > 1) {
      int n2 = (int)(Data[1] % (Size + 1));
      (void)sqlite3_bind_blob(stmt, 2, Data, n2, SQLITE_TRANSIENT);
    } else {
      (void)sqlite3_bind_null(stmt, 2);
    }

    if (Size > 2) {
      (void)sqlite3_bind_text(stmt, 3, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
    } else {
      (void)sqlite3_bind_null(stmt, 3);
    }

    if (Size > 3) {
      (void)sqlite3_bind_text16(stmt, 4, Data, (int)(Size & ~1u), SQLITE_TRANSIENT);
    } else {
      (void)sqlite3_bind_null(stmt, 4);
    }

    if (Size > 4) {
      (void)sqlite3_bind_zeroblob(stmt, 5, (int)(Data[4]));
    } else {
      (void)sqlite3_bind_null(stmt, 5);
    }

    if (Size > 5) {
      (void)sqlite3_bind_int64(stmt, 6, (sqlite3_int64)Data[5]);
    } else {
      (void)sqlite3_bind_null(stmt, 6);
    }
  } else {
    (void)sqlite3_bind_null(stmt, 1);
    (void)sqlite3_bind_null(stmt, 2);
    (void)sqlite3_bind_null(stmt, 3);
    (void)sqlite3_bind_null(stmt, 4);
    (void)sqlite3_bind_null(stmt, 5);
    (void)sqlite3_bind_null(stmt, 6);
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int cols = sqlite3_column_count(stmt);
    int i;
    for (i = 0; i < cols; i++) {
      sqlite3_value *uv = sqlite3_column_value(stmt, i);
      if (!uv) continue;

      dupv = sqlite3_value_dup(uv);
      if (dupv) {
        exercise_value(db, dupv);
        sqlite3_value_free(dupv);
        dupv = NULL;
      }

      (void)sqlite3_column_bytes(stmt, i);
      (void)sqlite3_column_text(stmt, i);
      (void)sqlite3_column_blob(stmt, i);
      (void)sqlite3_column_type(stmt, i);
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}