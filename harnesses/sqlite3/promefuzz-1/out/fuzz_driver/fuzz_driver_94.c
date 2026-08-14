// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
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

static unsigned char pick_encoding(uint8_t b) {
  switch (b % 4) {
    case 0: return SQLITE_UTF8;
    case 1: return SQLITE_UTF16LE;
    case 2: return SQLITE_UTF16BE;
    default: return SQLITE_UTF16;
  }
}

static void noop_destructor(void *p) {
  (void)p;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_exec(db, "CREATE TABLE t(a TEXT, b INTEGER, c BLOB);", NULL, NULL, NULL);

  {
    const char *sql =
        "INSERT INTO t(a,b,c) VALUES(?1,?2,?3);"
        "SELECT ?1, ?2, ?3;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return 0;
    }
  }

  for (int iter = 0; iter < 4; ++iter) {
    int which = (n > 0) ? (*p++ % 6) : iter % 6;
    if (n > 0) n--;

    int index = (int)((n > 0 ? (*p++ % 6) : (uint8_t)(iter + 1)) + 1);
    if (n > 0) n--;

    switch (which) {
      case 0: {
        int len = (int)(n > 0 ? (*p++ % (n + 1)) : 0);
        if (n > 0) n--;
        if ((size_t)len > n) len = (int)n;
        (void)sqlite3_bind_text(stmt, index, (const char *)p, len, SQLITE_TRANSIENT);
        p += len;
        n -= (size_t)len;
        break;
      }
      case 1: {
        int v = (int)read_u32(&p, &n);
        (void)sqlite3_bind_int(stmt, index, v);
        break;
      }
      case 2: {
        sqlite3_uint64 len64 = (sqlite3_uint64)read_u32(&p, &n);
        unsigned char enc = pick_encoding(n > 0 ? *p++ : 0);
        if (n > 0) n--;
        if (len64 > (sqlite3_uint64)n) len64 = (sqlite3_uint64)n;
        (void)sqlite3_bind_text64(stmt, index, (const char *)p, len64, SQLITE_TRANSIENT, enc);
        p += (size_t)len64;
        n -= (size_t)len64;
        break;
      }
      case 3: {
        void *ptr = NULL;
        char *mem = NULL;
        size_t alloc_sz = n > 0 ? (size_t)(*p++ % 32) : 0;
        if (n > 0) n--;
        if (alloc_sz > 0) {
          mem = (char *)malloc(alloc_sz);
          if (mem) {
            size_t copy = alloc_sz < n ? alloc_sz : n;
            memcpy(mem, p, copy);
            if (copy < alloc_sz) memset(mem + copy, 0, alloc_sz - copy);
            ptr = mem;
            p += copy;
            n -= copy;
          }
        }
        rc = sqlite3_bind_pointer(stmt, index, ptr, "fuzz.ptr", free);
        if (rc != SQLITE_OK && mem) {
          /* If bind fails, SQLite should call destructor; avoid double free. */
          mem = NULL;
        }
        break;
      }
      case 4: {
        (void)sqlite3_step(stmt);
        (void)sqlite3_reset(stmt);
        break;
      }
      case 5: {
        (void)sqlite3_clear_bindings(stmt);
        break;
      }
    }
  }

  (void)sqlite3_step(stmt);
  (void)sqlite3_reset(stmt);

  if (Size > 0 && (Data[0] & 1)) {
    (void)sqlite3_bind_text(stmt, 1, "", -1, noop_destructor);
    (void)sqlite3_bind_int(stmt, 2, 0);
    (void)sqlite3_bind_text64(stmt, 3, (const char *)Data, (sqlite3_uint64)Size, SQLITE_TRANSIENT, SQLITE_UTF8);
    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);
  }

  (void)sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}