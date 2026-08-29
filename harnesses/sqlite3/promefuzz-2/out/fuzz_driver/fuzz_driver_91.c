// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_db_release_memory at sqlite3.c:153110:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_soft_heap_limit at sqlite3.c:15556:17 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_db_release_memory at sqlite3.c:153110:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v |= ((uint32_t)(*p)[i]) << (8 * i);
  }
  *p += take;
  *n -= take;
  return v;
}

static sqlite3_int64 read_i64(const uint8_t **p, size_t *n) {
  sqlite3_int64 v = 0;
  size_t take = *n < 8 ? *n : 8;
  for (size_t i = 0; i < take; ++i) {
    v |= ((sqlite3_int64)(*p)[i]) << (8 * i);
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_i32(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  int soft_limit = read_i32(&p, &n);
  int release_n1 = read_i32(&p, &n);
  int release_n2 = read_i32(&p, &n);
  int reset1 = read_i32(&p, &n) & 1;
  int reset2 = read_i32(&p, &n) & 1;
  sqlite3_int64 hard1 = read_i64(&p, &n);
  sqlite3_int64 hard2 = read_i64(&p, &n);
  int status_op1 = read_i32(&p, &n);
  int status_op2 = read_i32(&p, &n);
  int open_mode = read_i32(&p, &n) & 3;

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (open_mode == 1) {
    flags |= SQLITE_OPEN_URI;
  } else if (open_mode == 2) {
    flags |= SQLITE_OPEN_MEMORY;
  }

  sqlite3_soft_heap_limit(soft_limit);
  (void)sqlite3_memory_highwater(reset1);
  (void)sqlite3_hard_heap_limit64(hard1);
  (void)sqlite3_release_memory(release_n1);

  sqlite3_open_v2("./dummy_file", &db, flags, NULL);

  if (db) {
    char *errmsg = NULL;
    if (n > 0) {
      size_t sql_len = n;
      char sqlbuf[256];
      if (sql_len > sizeof(sqlbuf) - 1) sql_len = sizeof(sqlbuf) - 1;
      memcpy(sqlbuf, p, sql_len);
      sqlbuf[sql_len] = '\0';
      sqlite3_exec(db, sqlbuf, NULL, NULL, &errmsg);
      if (errmsg) sqlite3_free(errmsg);
    } else {
      sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);"
                       "INSERT INTO t VALUES(1);"
                       "SELECT * FROM t;",
                   NULL, NULL, &errmsg);
      if (errmsg) sqlite3_free(errmsg);
    }

    (void)sqlite3_db_release_memory(db);
  }

  sqlite3_int64 cur = 0, hi = 0;
  (void)sqlite3_status64(status_op1, &cur, &hi, reset2);
  (void)sqlite3_memory_highwater(0);
  (void)sqlite3_hard_heap_limit64(hard2);
  (void)sqlite3_release_memory(release_n2);
  (void)sqlite3_status64(status_op2, &cur, &hi, 0);

  if (db) {
    (void)sqlite3_db_release_memory(db);
    sqlite3_close(db);
  }

  return 0;
}