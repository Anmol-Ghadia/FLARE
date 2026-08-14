// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void udf_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;

  static const char txt1[] = "fuzz";
  static const char txt2[] = "another-result";
  static const unsigned char err16le[] = {
      'E', 0, 'R', 0, 'R', 0, 0, 0
  };

  sqlite3_result_text(ctx, txt1, -1, SQLITE_STATIC);
  sqlite3_result_error(ctx, "scalar error", -1);
  sqlite3_result_error_toobig(ctx);
  sqlite3_result_error16(ctx, err16le, 6);
  sqlite3_result_error_code(ctx, SQLITE_ERROR);
  sqlite3_result_text(ctx, txt2, (int)sizeof(txt2) - 1, SQLITE_STATIC);
}

static void udf_scalar_from_arg(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  const unsigned char *txt = 0;
  int n = -1;
  if (argc > 0 && argv && argv[0]) {
    txt = sqlite3_value_text(argv[0]);
    n = sqlite3_value_bytes(argv[0]);
  }

  if (txt) {
    sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
    sqlite3_result_error(ctx, (const char *)txt, n);
  } else {
    sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
    sqlite3_result_error_code(ctx, 0);
  }

  sqlite3_result_error_toobig(ctx);
}

static void udf_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int *state = (int *)sqlite3_aggregate_context(ctx, (int)sizeof(int));
  void *same0 = sqlite3_aggregate_context(ctx, 0);
  void *sameNeg = sqlite3_aggregate_context(ctx, -1);
  (void)same0;
  (void)sameNeg;

  if (state) {
    (*state)++;
    if (argc > 0 && argv && argv[0]) {
      const unsigned char *txt = sqlite3_value_text(argv[0]);
      int n = sqlite3_value_bytes(argv[0]);
      if (txt && n > 0) {
        sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
      }
    }
    if ((*state & 1) == 0) {
      sqlite3_result_error_code(ctx, SQLITE_TOOBIG);
    } else {
      sqlite3_result_error(ctx, "agg step error", -1);
    }
  } else {
    sqlite3_result_error_toobig(ctx);
  }
}

static void udf_agg_final(sqlite3_context *ctx) {
  int *state = (int *)sqlite3_aggregate_context(ctx, 0);
  if (state) {
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%d", *state);
    if (n < 0) {
      sqlite3_result_error_code(ctx, SQLITE_ERROR);
      return;
    }
    sqlite3_result_text(ctx, buf, n, SQLITE_TRANSIENT);
    sqlite3_result_error_code(ctx, SQLITE_OK);
  } else {
    static const unsigned char err16le[] = {
        'N', 0, 'U', 0, 'L', 0, 'L', 0
    };
    sqlite3_result_error16(ctx, err16le, 8);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_scalar", 1, SQLITE_UTF8, NULL,
                          udf_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzz_scalar_arg", 1, SQLITE_UTF8, NULL,
                          udf_scalar_from_arg, NULL, NULL);
  sqlite3_create_function(db, "fuzz_agg", 1, SQLITE_UTF8, NULL,
                          NULL, udf_agg_step, udf_agg_final);

  sqlite3_exec(db,
               "CREATE TABLE t(x TEXT);"
               "INSERT INTO t(x) VALUES('a'),('bb'),('ccc'),(NULL);",
               NULL, NULL, NULL);

  {
    rc = sqlite3_prepare_v2(db, "SELECT fuzz_scalar(?1);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  {
    rc = sqlite3_prepare_v2(db, "SELECT fuzz_scalar_arg(?1);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  {
    rc = sqlite3_prepare_v2(db, "SELECT fuzz_agg(x) FROM t;", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  {
    rc = sqlite3_prepare_v2(db,
                            "SELECT fuzz_agg(?1) FROM "
                            "(SELECT 1 UNION ALL SELECT 2 UNION ALL SELECT 3);",
                            -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  {
    rc = sqlite3_exec(db,
                      "SELECT fuzz_scalar_arg(readfile('./dummy_file'));",
                      NULL, NULL, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }
    (void)rc;
  }

  sqlite3_close(db);
  return 0;
}