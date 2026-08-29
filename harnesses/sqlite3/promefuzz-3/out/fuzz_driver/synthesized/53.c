// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
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

static void profile_cb(void *pArg, const char *zSql, sqlite3_uint64 nNs) {
  (void)pArg;
  (void)zSql;
  (void)nNs;
}

static int consume_u8(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static uint32_t consume_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  for (int i = 0; i < 4 && *n > 0; i++) {
    v = (v << 8) | **p;
    (*p)++;
    (*n)--;
  }
  return v;
}

static size_t consume_bytes(const uint8_t **p, size_t *n, char *out, size_t max_out) {
  if (max_out == 0) return 0;
  size_t want = *n;
  if (want > max_out - 1) want = max_out - 1;
  memcpy(out, *p, want);
  out[want] = '\0';
  *p += want;
  *n -= want;
  return want;
}

int LLVMFuzzerTestOneInput_53(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char sqlbuf[1024];
  int rc;

  int open_mode = consume_u8(&p, &n) % 3;
  if (open_mode == 0) {
    rc = sqlite3_open(":memory:", &db);
  } else if (open_mode == 1) {
    rc = sqlite3_open("./dummy_file", &db);
  } else {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if (consume_u8(&p, &n) & 1) flags |= SQLITE_OPEN_URI;
    rc = sqlite3_open_v2("./dummy_file", &db, flags, NULL);
  }
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    sqlite3_thread_cleanup();
    return 0;
  }

  sqlite3_db_mutex(db);
  sqlite3_extended_result_codes(db, consume_u8(&p, &n));
  sqlite3_profile(db, profile_cb, db);
  sqlite3_profile(db, NULL, NULL);
  sqlite3_profile(db, profile_cb, (void *)p);

  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);", NULL, NULL, NULL);
  sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);

  if (consume_u8(&p, &n) & 1) {
    sqlite3_interrupt(db);
  }

  if ((consume_u8(&p, &n) & 1) == 0) {
    const char *tail = NULL;
    consume_bytes(&p, &n, sqlbuf, sizeof(sqlbuf));
    if (sqlbuf[0] == '\0') {
      strcpy(sqlbuf, "SELECT 1;");
    }
    rc = sqlite3_prepare_v2(db, sqlbuf, -1, &stmt, &tail);
  } else {
    uint32_t choice = consume_u32(&p, &n) % 6;
    const char *sql = "SELECT 1;";
    switch (choice) {
      case 0: sql = "SELECT 1;"; break;
      case 1: sql = "CREATE TABLE IF NOT EXISTS x(y);"; break;
      case 2: sql = "INSERT INTO t(a,b,c) VALUES(1,2,3);"; break;
      case 3: sql = "SELECT * FROM t;"; break;
      case 4: sql = "UPDATE t SET a=a+1;"; break;
      case 5: sql = "DELETE FROM t WHERE a<0;"; break;
    }
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  }

  sqlite3_db_handle(NULL);

  if (rc == SQLITE_OK && stmt != NULL) {
    sqlite3 *owner = sqlite3_db_handle(stmt);
    if (owner) {
      sqlite3_db_mutex(owner);
      sqlite3_extended_result_codes(owner, consume_u8(&p, &n) & 1);
      if (consume_u8(&p, &n) & 1) {
        sqlite3_interrupt(owner);
      }
    }

    int steps = (consume_u8(&p, &n) % 8) + 1;
    for (int i = 0; i < steps; i++) {
      int s = sqlite3_step(stmt);
      if (s != SQLITE_ROW && s != SQLITE_DONE) {
        break;
      }
      if ((consume_u8(&p, &n) & 3) == 0) {
        sqlite3_interrupt(db);
      }
      if ((consume_u8(&p, &n) & 3) == 1) {
        sqlite3_extended_result_codes(db, consume_u8(&p, &n));
      }
      if (s == SQLITE_DONE) break;
    }

    sqlite3_reset(stmt);
    if (consume_u8(&p, &n) & 1) {
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
  sqlite3_profile(db, NULL, NULL);
  sqlite3_db_mutex(db);
  sqlite3_interrupt(db);
  sqlite3_thread_cleanup();
  sqlite3_close(db);
  sqlite3_thread_cleanup();
  return 0;
}