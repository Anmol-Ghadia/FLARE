// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sqlite3.h>

typedef struct {
  const uint8_t *data;
  size_t size;
  size_t off;
} FuzzInput;

static uint8_t fi_u8(FuzzInput *in) {
  if (in->off >= in->size) return 0;
  return in->data[in->off++];
}

static uint32_t fi_u32(FuzzInput *in) {
  uint32_t v = 0;
  int i;
  for (i = 0; i < 4; i++) {
    v = (v << 8) | fi_u8(in);
  }
  return v;
}

static uint64_t fi_u64(FuzzInput *in) {
  uint64_t v = 0;
  int i;
  for (i = 0; i < 8; i++) {
    v = (v << 8) | (uint64_t)fi_u8(in);
  }
  return v;
}

static sqlite3_int64 fi_i64(FuzzInput *in) {
  return (sqlite3_int64)fi_u64(in);
}

static size_t fi_take_bytes(FuzzInput *in, const uint8_t **ptr) {
  uint32_t want = fi_u32(in);
  size_t remain = (in->off < in->size) ? (in->size - in->off) : 0;
  size_t n = remain ? (want % (remain + 1)) : 0;
  *ptr = in->data + in->off;
  in->off += n;
  return n;
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  if (argc > 0 && argv && argv[0]) {
    unsigned int st = sqlite3_value_subtype(argv[0]);
    int mode = (int)(st % 3U);
    if (mode == 0) {
      sqlite3_result_value(ctx, argv[0]);
    } else if (mode == 1) {
      sqlite3_result_int64(ctx, sqlite3_value_int64(argv[0]));
    } else {
      const unsigned char *txt = sqlite3_value_text(argv[0]);
      int n = sqlite3_value_bytes(argv[0]);
      sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
    }
    sqlite3_result_subtype(ctx, st ^ 0xA5U);
  } else {
    sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
    sqlite3_result_subtype(ctx, 0);
  }
}

static void fuzz_aggregate_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *sum = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, (int)sizeof(sqlite3_int64));
  if (!sum) return;
  if (argc > 0 && argv && argv[0]) {
    *sum += sqlite3_value_int64(argv[0]);
  } else {
    *sum += 1;
  }
}

static void fuzz_aggregate_final(sqlite3_context *ctx) {
  sqlite3_int64 *sum = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  if (sum) {
    sqlite3_result_int64(ctx, *sum);
  } else {
    sqlite3_result_int64(ctx, 0);
  }
  sqlite3_result_subtype(ctx, 0x5AU);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzInput in;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_stmt *stmt2 = NULL;
  char *errmsg = NULL;
  int rc;

  in.data = Data;
  in.size = Size;
  in.off = 0;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_scalar", 1, SQLITE_UTF8, NULL, fuzz_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzz_agg", 1, SQLITE_UTF8, NULL, NULL, fuzz_aggregate_step, fuzz_aggregate_final);

  sqlite3_exec(db,
               "CREATE TABLE t(x);"
               "INSERT INTO t VALUES(NULL),(0),(1),(-1),(1234567890123),('text'),(x'00112233');",
               NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sqls[] = {
      "SELECT fuzz_scalar(x) FROM t;",
      "SELECT fuzz_scalar(?1);",
      "SELECT fuzz_scalar(CAST(?1 AS TEXT));",
      "SELECT fuzz_scalar(CAST(?1 AS BLOB));",
      "SELECT fuzz_agg(x) FROM t;",
      "SELECT fuzz_agg(?1);",
      "SELECT fuzz_scalar(zeroblob(8));"
    };
    size_t idx = fi_u8(&in) % (sizeof(sqls) / sizeof(sqls[0]));
    rc = sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      const uint8_t *seg = NULL;
      size_t seglen = fi_take_bytes(&in, &seg);
      sqlite3_int64 i64v = fi_i64(&in);
      int bind_mode = (int)(fi_u8(&in) % 5);

      switch (bind_mode) {
        case 0:
          sqlite3_bind_null(stmt, 1);
          break;
        case 1:
          sqlite3_bind_int64(stmt, 1, i64v);
          break;
        case 2:
          sqlite3_bind_text(stmt, 1, (const char *)seg, (int)seglen, SQLITE_TRANSIENT);
          break;
        case 3:
          sqlite3_bind_blob(stmt, 1, seg, (int)seglen, SQLITE_TRANSIENT);
          break;
        default:
          sqlite3_bind_double(stmt, 1, (double)i64v);
          break;
      }

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        sqlite3_value *colv = sqlite3_column_value(stmt, 0);
        if (colv) {
          (void)sqlite3_value_subtype(colv);
        }
      }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  {
    const char *sql =
        "WITH RECURSIVE c(i) AS (VALUES(1) UNION ALL SELECT i+1 FROM c WHERE i<8) "
        "SELECT fuzz_agg(i) FROM c;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt2, NULL);
    if (rc == SQLITE_OK && stmt2) {
      while ((rc = sqlite3_step(stmt2)) == SQLITE_ROW) {
        sqlite3_value *v0 = sqlite3_column_value(stmt2, 0);
        if (v0) (void)sqlite3_value_subtype(v0);
      }
    }
    sqlite3_finalize(stmt2);
    stmt2 = NULL;
  }

  sqlite3_close(db);
  return 0;
}