// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_bind_zeroblob64 at sqlite3.c:73460:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static sqlite3_uint64 read_u64(const uint8_t **p, size_t *n) {
  sqlite3_uint64 v = 0;
  size_t take = *n < 8 ? *n : 8;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int prepare_stmt(sqlite3 *db, const char *sql, sqlite3_stmt **stmt) {
  return sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
}

int LLVMFuzzerTestOneInput_70(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  sqlite3 *db = NULL;
  sqlite3_stmt *insert_stmt = NULL;
  sqlite3_stmt *insert_stmt2 = NULL;
  sqlite3_stmt *select_stmt = NULL;
  sqlite3_stmt *mismatch_stmt = NULL;
  sqlite3_stmt *expired_stmt = NULL;

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  {
    sqlite3_exec(db,
                 "CREATE TABLE t(a BLOB, b BLOB, c INTEGER);"
                 "CREATE TABLE u(x BLOB, y BLOB);",
                 NULL, NULL, NULL);
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  if (prepare_stmt(db, "INSERT INTO t(a,b,c) VALUES(?1,?2,?3);", &insert_stmt) != SQLITE_OK)
    goto cleanup;
  if (prepare_stmt(db, "INSERT INTO t(a,b,c) VALUES(?1,?2,?3);", &insert_stmt2) != SQLITE_OK)
    goto cleanup;
  if (prepare_stmt(db, "SELECT a,b,c FROM t;", &select_stmt) != SQLITE_OK)
    goto cleanup;
  if (prepare_stmt(db, "INSERT INTO u(x,y) VALUES(?1,?2);", &mismatch_stmt) != SQLITE_OK)
    goto cleanup;
  if (prepare_stmt(db, "SELECT ?1;", &expired_stmt) != SQLITE_OK)
    goto cleanup;

  for (int iter = 0; iter < 4; ++iter) {
    uint32_t op = read_u32(&p, &n);
    uint32_t blob_len32 = read_u32(&p, &n);
    sqlite3_uint64 zero_len = read_u64(&p, &n);
    int param1 = (int)((op % 5) - 1);
    int param2 = (int)(((op >> 3) % 5) - 1);
    int do_step = (op >> 8) & 1;
    int do_reset_first = (op >> 9) & 1;
    int do_transfer = (op >> 10) & 1;
    int use_mismatch = (op >> 11) & 1;
    int make_expired = (op >> 12) & 1;

    size_t blob_len = n ? (size_t)(blob_len32 % (n + 1)) : 0;
    const void *blob_ptr = blob_len ? (const void *)p : (const void *)"";
    if (blob_len > n) blob_len = n;

    if (do_reset_first) {
      sqlite3_reset(insert_stmt);
      sqlite3_reset(insert_stmt2);
    }

    sqlite3_bind_blob64(insert_stmt, param1, blob_ptr, (sqlite3_uint64)blob_len, SQLITE_STATIC);
    sqlite3_bind_zeroblob64(insert_stmt, param2, zero_len % 1024);

    {
      int intv = (int)read_u32(&p, &n);
      sqlite3_bind_int(insert_stmt, 3, intv);
    }

    if (do_transfer) {
      if (use_mismatch) {
        sqlite3_transfer_bindings(insert_stmt, mismatch_stmt);
      } else {
        sqlite3_transfer_bindings(insert_stmt, insert_stmt2);
      }
    }

    if (make_expired) {
      sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS z(q);", NULL, NULL, NULL);
      sqlite3_exec(db, "DROP TABLE IF EXISTS z;", NULL, NULL, NULL);
      sqlite3_transfer_bindings(expired_stmt, insert_stmt2);
    }

    if (do_step) {
      sqlite3_step(insert_stmt);
      sqlite3_reset(insert_stmt);

      sqlite3_step(insert_stmt2);
      sqlite3_reset(insert_stmt2);
    }

    if (blob_len <= n) {
      p += blob_len;
      n -= blob_len;
    } else {
      n = 0;
    }
  }

  sqlite3_reset(select_stmt);
  while (sqlite3_step(select_stmt) == SQLITE_ROW) {
    int cols = sqlite3_column_count(select_stmt);
    for (int i = -1; i <= cols; ++i) {
      const void *b = sqlite3_column_blob(select_stmt, i);
      int sz = sqlite3_column_bytes(select_stmt, i);
      (void)b;
      (void)sz;
    }
  }
  sqlite3_reset(select_stmt);

  sqlite3_reset(insert_stmt);
  sqlite3_reset(insert_stmt2);
  sqlite3_reset(mismatch_stmt);
  sqlite3_reset(expired_stmt);

cleanup:
  if (insert_stmt) sqlite3_finalize(insert_stmt);
  if (insert_stmt2) sqlite3_finalize(insert_stmt2);
  if (select_stmt) sqlite3_finalize(select_stmt);
  if (mismatch_stmt) sqlite3_finalize(mismatch_stmt);
  if (expired_stmt) sqlite3_finalize(expired_stmt);
  if (db) sqlite3_close(db);
  return 0;
}