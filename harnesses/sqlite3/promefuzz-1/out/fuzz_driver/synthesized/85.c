// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sqlite3.h"

typedef struct {
  uint64_t state;
} FuzzCursor;

static uint8_t fuzz_u8(FuzzCursor *c) {
  return (uint8_t)(c->state = c->state * 6364136223846793005ULL + 1);
}

static uint32_t fuzz_u32(FuzzCursor *c) {
  uint32_t v = 0;
  v |= (uint32_t)fuzz_u8(c);
  v |= (uint32_t)fuzz_u8(c) << 8;
  v |= (uint32_t)fuzz_u8(c) << 16;
  v |= (uint32_t)fuzz_u8(c) << 24;
  return v;
}

static uint64_t fuzz_u64(FuzzCursor *c) {
  uint64_t hi = fuzz_u32(c);
  uint64_t lo = fuzz_u32(c);
  return (hi << 32) | lo;
}

static int32_t fuzz_i32(FuzzCursor *c) {
  return (int32_t)fuzz_u32(c);
}

static int64_t fuzz_i64(FuzzCursor *c) {
  return (int64_t)fuzz_u64(c);
}

static double fuzz_double(FuzzCursor *c) {
  union {
    uint64_t u;
    double d;
  } v;
  v.u = fuzz_u64(c);
  return v.d;
}

static void fuzz_sqlfunc(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;

  void *ud = sqlite3_user_data(ctx);
  FuzzCursor cur;
  memset(&cur, 0, sizeof(cur));

  if (ud) {
    memcpy(&cur, ud, sizeof(cur));
  }

  switch (fuzz_u8(&cur) % 6) {
    case 0:
      sqlite3_result_double(ctx, fuzz_double(&cur));
      break;
    case 1:
      sqlite3_result_int(ctx, fuzz_i32(&cur));
      break;
    case 2:
      sqlite3_result_int64(ctx, (sqlite3_int64)fuzz_i64(&cur));
      break;
    case 3:
      (void)sqlite3_result_zeroblob64(ctx, (sqlite3_uint64)(fuzz_u64(&cur) & 0xffffULL));
      break;
    case 4:
      sqlite3_result_subtype(ctx, fuzz_u32(&cur));
      sqlite3_result_double(ctx, fuzz_double(&cur));
      break;
    default:
      sqlite3_result_subtype(ctx, fuzz_u32(&cur));
      sqlite3_result_int64(ctx, (sqlite3_int64)fuzz_i64(&cur));
      (void)sqlite3_result_zeroblob64(ctx, (sqlite3_uint64)(fuzz_u64(&cur) & 0xfffULL));
      break;
  }
}

int LLVMFuzzerTestOneInput_85(const uint8_t *Data, size_t Size) {
  FILE *fp;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  FuzzCursor seed;
  char sql[512];
  int mode;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  memset(&seed, 0, sizeof(seed));
  {
    size_t n = Size < sizeof(seed.state) ? Size : sizeof(seed.state);
    if (n) {
      memcpy(&seed.state, Data, n);
    } else {
      seed.state = 0x123456789abcdef0ULL;
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_create_function(
      db, "fuzzfunc", -1, SQLITE_UTF8, &seed, fuzz_sqlfunc, NULL, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(x) VALUES(1),(2),(3);", NULL, NULL, NULL);

  mode = (int)(seed.state % 6);
  switch (mode) {
    case 0:
      snprintf(sql, sizeof(sql), "SELECT fuzzfunc();");
      break;
    case 1:
      snprintf(sql, sizeof(sql), "SELECT fuzzfunc(1, 2, 3);");
      break;
    case 2:
      snprintf(sql, sizeof(sql), "SELECT fuzzfunc(x) FROM t;");
      break;
    case 3:
      snprintf(sql, sizeof(sql), "SELECT fuzzfunc(randomblob(16), x, 'abc') FROM t;");
      break;
    case 4:
      snprintf(sql, sizeof(sql), "SELECT fuzzfunc(zeroblob(8));");
      break;
    default:
      snprintf(sql, sizeof(sql), "SELECT fuzzfunc(readfile('./dummy_file'));");
      break;
  }

  rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc == SQLITE_OK && stmt) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}