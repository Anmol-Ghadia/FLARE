// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_aggregate_count at sqlite3.c:72894:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void udf_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int n = 0;
  const char *msg = "";
  void *aux0 = NULL;

  if (argc > 0 && argv && argv[0]) {
    msg = (const char *)sqlite3_value_text(argv[0]);
    n = sqlite3_value_bytes(argv[0]);
  }

  aux0 = sqlite3_get_auxdata(ctx, 0);

  if (argc > 1 && argv && argv[1]) {
    int mode = sqlite3_value_int(argv[1]) & 3;
    switch (mode) {
      case 0:
        sqlite3_result_text(ctx, msg ? msg : "", n >= 0 ? n : -1, SQLITE_TRANSIENT);
        break;
      case 1:
        sqlite3_result_value(ctx, argv[1]);
        break;
      case 2:
        sqlite3_result_error(ctx, msg ? msg : "err", n >= 0 ? n : -1);
        break;
      default:
        if (aux0) {
          sqlite3_result_text(ctx, (const char *)aux0, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_text(ctx, "noaux", -1, SQLITE_STATIC);
        }
        break;
    }
  } else {
    if (aux0) {
      sqlite3_result_text(ctx, (const char *)aux0, -1, SQLITE_TRANSIENT);
    } else {
      sqlite3_result_text(ctx, msg ? msg : "", n >= 0 ? n : -1, SQLITE_TRANSIENT);
    }
  }
}

static void udf_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int *state = (int *)sqlite3_aggregate_context(ctx, (int)sizeof(int));
  if (state) {
    (*state)++;
  }

  (void)sqlite3_get_auxdata(ctx, 0);

  /* Aggregate step functions must not call sqlite3_result_*().
     Only accumulate state here. Touch inputs to exercise code paths. */
  if (argc > 0 && argv && argv[0]) {
    int sel = sqlite3_value_int(argv[0]) & 1;
    if (sel == 0) {
      (void)sqlite3_value_text(argv[0]);
      (void)sqlite3_value_bytes(argv[0]);
    } else {
      (void)sqlite3_value_type(argv[0]);
    }
  }
}

static void udf_agg_final(sqlite3_context *ctx) {
  int *state = (int *)sqlite3_aggregate_context(ctx, 0);
  int cnt = sqlite3_aggregate_count(ctx);

  if (state) {
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "%d:%d", *state, cnt);
    if (n < 0) n = 0;
    sqlite3_result_text(ctx, buf, n, SQLITE_TRANSIENT);
  } else {
    sqlite3_result_error(ctx, "nostate", -1);
  }
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  size_t split1, split2;
  char *sql1 = NULL, *sql2 = NULL, *blobhex = NULL;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_scalar", 2, SQLITE_UTF8, NULL, udf_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzz_agg", 1, SQLITE_UTF8, NULL, NULL, udf_agg_step, udf_agg_final);

  exec_sql(db, "CREATE TABLE t(x TEXT, y INTEGER);");
  exec_sql(db, "INSERT INTO t VALUES('alpha',1),('beta',2),('gamma',3),('',0);");

  split1 = Size > 0 ? (size_t)(Data[0] % (Size + 1)) : 0;
  split2 = Size > 1 ? (size_t)(Data[1] % (Size + 1)) : split1;
  if (split2 < split1) {
    size_t tmp = split1;
    split1 = split2;
    split2 = tmp;
  }

  sql1 = (char *)malloc(split1 + 1);
  sql2 = (char *)malloc((Size - split1) + 1);
  blobhex = (char *)malloc(Size * 2 + 1);

  if (sql1) {
    memcpy(sql1, Data, split1);
    sql1[split1] = '\0';
  }
  if (sql2) {
    memcpy(sql2, Data + split1, Size - split1);
    sql2[Size - split1] = '\0';
  }
  if (blobhex) {
    static const char hex[] = "0123456789ABCDEF";
    for (size_t i = 0; i < Size; i++) {
      blobhex[i * 2] = hex[(Data[i] >> 4) & 0xF];
      blobhex[i * 2 + 1] = hex[Data[i] & 0xF];
    }
    blobhex[Size * 2] = '\0';
  }

  if (sql1 && split1 > 0) {
    sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
    if (stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    const char *q = "SELECT fuzz_scalar(?1, ?2), fuzz_scalar(x, y) FROM t;";
    sqlite3_prepare_v2(db, q, -1, &stmt, NULL);
    if (stmt) {
      if (Size > 0) sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
      else sqlite3_bind_text(stmt, 1, "", 0, SQLITE_STATIC);
      sqlite3_bind_int(stmt, 2, Size > 0 ? Data[0] : 0);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_null(stmt, 1);
      sqlite3_bind_int(stmt, 2, Size > 1 ? Data[1] : 1);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    const char *q = "SELECT fuzz_agg(y) FROM t;";
    sqlite3_prepare_v2(db, q, -1, &stmt, NULL);
    if (stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  if (blobhex) {
    size_t qlen = strlen(blobhex) + 128;
    char *dynsql = (char *)malloc(qlen);
    if (dynsql) {
      snprintf(dynsql, qlen, "SELECT fuzz_scalar(CAST(X'%s' AS TEXT), %d), fuzz_agg(length(X'%s'));",
               blobhex, (Size > 2 ? Data[2] : 0), blobhex);
      sqlite3_prepare_v2(db, dynsql, -1, &stmt, NULL);
      if (stmt) {
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
      free(dynsql);
    }
  }

  if (sql2 && (Size - split1) > 0) {
    sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  free(sql1);
  free(sql2);
  free(blobhex);
  sqlite3_close(db);
  return 0;
}