// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static unsigned read_unsigned(const uint8_t **p, size_t *n) {
  return (unsigned)read_u32(p, n);
}

static int trace_cb(unsigned t, void *ctx, void *p, void *x) {
  volatile unsigned sink = t;
  sink ^= (unsigned)(uintptr_t)ctx;
  sink ^= (unsigned)(uintptr_t)p;
  sink ^= (unsigned)(uintptr_t)x;
  (void)sink;
  return 0;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void step_some(sqlite3 *db, const char *sql, int keep_open, sqlite3_stmt **out_stmt) {
  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    if (out_stmt) *out_stmt = NULL;
    return;
  }
  (void)sqlite3_step(stmt);
  if (keep_open) {
    if (out_stmt) *out_stmt = stmt;
  } else {
    sqlite3_finalize(stmt);
    if (out_stmt) *out_stmt = NULL;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_stmt *held_stmt = NULL;
  sqlite3_stmt *stmt2 = NULL;

  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (n) {
    uint8_t mode = *p++;
    n--;
    if (mode & 1) flags |= SQLITE_OPEN_URI;
    if (mode & 2) flags |= SQLITE_OPEN_MEMORY;
    if (mode & 4) flags |= SQLITE_OPEN_NOMUTEX;
    if (mode & 8) flags |= SQLITE_OPEN_FULLMUTEX;
  }

  if (sqlite3_open_v2("./dummy_file", &db, flags, NULL) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close_v2(db);
    return 0;
  }

  sqlite3_open_v2("./dummy_file", &db2, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  (void)sqlite3_busy_timeout(db, read_int(&p, &n));
  (void)sqlite3_get_autocommit(db);

  unsigned trace_mask = 0;
  if (n) {
    uint8_t m = *p++;
    n--;
    if (m & 1) trace_mask |= SQLITE_TRACE_STMT;
    if (m & 2) trace_mask |= SQLITE_TRACE_PROFILE;
    if (m & 4) trace_mask |= SQLITE_TRACE_ROW;
    if (m & 8) trace_mask |= SQLITE_TRACE_CLOSE;
  }
  (void)sqlite3_trace_v2(db, trace_mask, trace_cb, (void *)db);
  if (n && (*p & 1)) {
    (void)sqlite3_trace_v2(db, 0, NULL, NULL);
  } else {
    if (n) {
      p++;
      n--;
    }
  }

  (void)sqlite3_wal_autocheckpoint(db, read_int(&p, &n));

  exec_sql(db, "PRAGMA journal_mode=WAL;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x BLOB);");

  if (db2) {
    exec_sql(db2, "PRAGMA journal_mode=WAL;");
    (void)sqlite3_busy_timeout(db2, read_int(&p, &n));
  }

  if (n && (*p & 1)) {
    p++;
    n--;
    exec_sql(db, "BEGIN;");
    (void)sqlite3_get_autocommit(db);

    int loops = (n ? (*p % 8) : 0);
    if (n) {
      p++;
      n--;
    }

    for (int i = 0; i < loops; i++) {
      int v = read_int(&p, &n);
      sqlite3_stmt *ins = NULL;
      if (sqlite3_prepare_v2(db, "INSERT INTO t(a,b) VALUES(?1,?2);", -1, &ins, NULL) == SQLITE_OK && ins) {
        sqlite3_bind_int(ins, 1, v);
        if (n) {
          int len = (int)(*p % (n > 32 ? 32 : (n ? n : 1)));
          p++;
          n--;
          if ((size_t)len > n) len = (int)n;
          sqlite3_bind_text(ins, 2, (const char *)p, len, SQLITE_TRANSIENT);
          p += len;
          n -= (size_t)len;
        } else {
          sqlite3_bind_text(ins, 2, "x", -1, SQLITE_STATIC);
        }
        (void)sqlite3_step(ins);
        sqlite3_finalize(ins);
      }

      if ((i % 2) == 0) {
        (void)sqlite3_db_cacheflush(db);
      }
    }

    if (n && (*p & 1)) {
      p++;
      n--;
      step_some(db, "SELECT * FROM t;", 1, &held_stmt);
      (void)sqlite3_db_cacheflush(db);
    } else if (n) {
      p++;
      n--;
    }

    if (db2 && n && (*p & 1)) {
      p++;
      n--;
      exec_sql(db2, "BEGIN IMMEDIATE;");
      exec_sql(db2, "INSERT INTO t(a,b) VALUES(999,'lock');");
      (void)sqlite3_db_cacheflush(db);
      exec_sql(db2, "ROLLBACK;");
    } else if (n) {
      p++;
      n--;
    }

    if (n && (*p & 1)) {
      p++;
      n--;
      exec_sql(db, "SAVEPOINT s1;");
      exec_sql(db, "INSERT INTO t(a,b) VALUES(1,'sp');");
      (void)sqlite3_db_cacheflush(db);
      exec_sql(db, "RELEASE s1;");
    } else if (n) {
      p++;
      n--;
    }

    if (n && (*p & 1)) {
      p++;
      n--;
      exec_sql(db, "COMMIT;");
    } else {
      if (n) {
        p++;
        n--;
      }
      exec_sql(db, "ROLLBACK;");
    }
    (void)sqlite3_get_autocommit(db);
  }

  step_some(db, "SELECT count(*) FROM t;", 0, &stmt2);
  (void)stmt2;

  (void)sqlite3_trace_v2(db, SQLITE_TRACE_STMT | SQLITE_TRACE_ROW, trace_cb, (void *)p);
  exec_sql(db, "INSERT INTO t(a,b) VALUES(123,'final');");
  (void)sqlite3_db_cacheflush(db);
  (void)sqlite3_wal_autocheckpoint(db, read_int(&p, &n));
  (void)sqlite3_busy_timeout(db, read_int(&p, &n));
  (void)sqlite3_get_autocommit(db);

  if (held_stmt) {
    if (n && (*p & 1)) {
      p++;
      n--;
      (void)sqlite3_step(held_stmt);
    } else if (n) {
      p++;
      n--;
    }
  }

  if (held_stmt && n && (*p & 1)) {
    p++;
    n--;
    (void)sqlite3_close_v2(db);
    sqlite3_finalize(held_stmt);
    held_stmt = NULL;
    db = NULL;
  } else {
    if (n) {
      p++;
      n--;
    }
    if (held_stmt) {
      sqlite3_finalize(held_stmt);
      held_stmt = NULL;
    }
  }

  if (db2) {
    sqlite3_close_v2(db2);
    db2 = NULL;
  }

  if (db) {
    sqlite3_close_v2(db);
    db = NULL;
  }

  return 0;
}