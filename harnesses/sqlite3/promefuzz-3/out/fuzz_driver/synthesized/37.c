// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <sqlite3.h>

static void fuzz_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int nbytes = 0;
  void *agg = NULL;
  int len = 0;
  const unsigned char *txt = NULL;

  if (argc > 0 && argv && argv[0]) {
    nbytes = sqlite3_value_bytes(argv[0]);
  }

  agg = sqlite3_aggregate_context(ctx, nbytes);
  (void)agg;

  if (argc > 0 && argv && argv[0]) {
    len = sqlite3_value_bytes(argv[0]);
    txt = sqlite3_value_text(argv[0]);
    (void)txt;
    (void)len;
  }
}

static void fuzz_agg_final(sqlite3_context *ctx) {
  void *agg = sqlite3_aggregate_context(ctx, 0);
  (void)agg;
}

int LLVMFuzzerTestOneInput_37(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_stmt *ins = NULL;
  int rc;
  size_t i;
  int mode;

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) {
      sqlite3_close(db);
    }
    return 0;
  }

  rc = sqlite3_create_function(db, "fuzzagg", 1, SQLITE_UTF8, NULL, NULL,
                               fuzz_agg_step, fuzz_agg_final);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_prepare_v2(db, "INSERT INTO t(x) VALUES(?1);", -1, &ins, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  mode = (Size > 0) ? (Data[0] % 6) : 0;

  if (Size == 0) {
    sqlite3_bind_null(ins, 1);
    sqlite3_step(ins);
    sqlite3_reset(ins);
    sqlite3_clear_bindings(ins);
  } else {
    size_t limit = Size < 16 ? Size : 16;
    for (i = 0; i < limit; i++) {
      switch ((mode + Data[i]) % 6) {
        case 0:
          sqlite3_bind_null(ins, 1);
          break;
        case 1:
          sqlite3_bind_int64(ins, 1, (sqlite3_int64)(int8_t)Data[i]);
          break;
        case 2:
          sqlite3_bind_double(ins, 1, (double)Data[i] / 7.0);
          break;
        case 3:
          sqlite3_bind_text(ins, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
          break;
        case 4:
          sqlite3_bind_blob(ins, 1, Data, (int)Size, SQLITE_STATIC);
          break;
        default:
          sqlite3_bind_zeroblob(ins, 1, (int)(Data[i] % 64));
          break;
      }
      sqlite3_step(ins);
      sqlite3_reset(ins);
      sqlite3_clear_bindings(ins);
    }
  }

  sqlite3_finalize(ins);
  ins = NULL;

  rc = sqlite3_prepare_v2(db, "SELECT fuzzagg(x) FROM t;", -1, &stmt, NULL);
  if (rc == SQLITE_OK) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }
  sqlite3_finalize(stmt);
  stmt = NULL;

  rc = sqlite3_prepare_v2(db, "SELECT fuzzagg(?1);", -1, &stmt, NULL);
  if (rc == SQLITE_OK) {
    if (Size == 0) {
      sqlite3_bind_null(stmt, 1);
    } else {
      switch (Data[0] % 6) {
        case 0:
          sqlite3_bind_null(stmt, 1);
          break;
        case 1:
          sqlite3_bind_int64(stmt, 1, (sqlite3_int64)Size);
          break;
        case 2:
          sqlite3_bind_double(stmt, 1, (double)Size / 3.0);
          break;
        case 3:
          sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
          break;
        case 4:
          sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
          break;
        default:
          sqlite3_bind_zeroblob(stmt, 1, (int)(Size % 128));
          break;
      }
      sqlite3_step(stmt);
    }
  }
  sqlite3_finalize(stmt);

  sqlite3_close(db);
  return 0;
}