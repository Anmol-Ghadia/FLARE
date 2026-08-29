// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
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

static void update_cb(void *pArg, int op, const char *dbName, const char *tblName, sqlite3_int64 rowid) {
  volatile unsigned long long *state = (volatile unsigned long long *)pArg;
  if (state) {
    unsigned long long v = *state;
    v ^= (unsigned long long)op;
    if (dbName) v ^= (unsigned long long)(unsigned char)dbName[0] << 8;
    if (tblName) v ^= (unsigned long long)(unsigned char)tblName[0] << 16;
    v ^= (unsigned long long)rowid;
    *state = v;
  }
}

static int wal_cb(void *pArg, sqlite3 *db, const char *dbName, int nPages) {
  volatile unsigned long long *state = (volatile unsigned long long *)pArg;
  if (state) {
    unsigned long long v = *state;
    v ^= (unsigned long long)nPages;
    if (dbName) v ^= (unsigned long long)(unsigned char)dbName[0] << 24;
    v ^= (unsigned long long)sqlite3_changes(db) << 1;
    v ^= (unsigned long long)sqlite3_total_changes(db) << 2;
    *state = v;
  }
  return SQLITE_OK;
}

static uint32_t consume_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
  if (err) sqlite3_free(err);
  return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (sqlite3_open_v2("./dummy_file", &db, flags, NULL) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  volatile unsigned long long state1 = 0x12345678ULL;
  volatile unsigned long long state2 = 0x9abcdef0ULL;

  (void)sqlite3_update_hook(db, update_cb, (void *)&state1);
  (void)sqlite3_wal_hook(db, wal_cb, (void *)&state2);

  exec_sql(db, "PRAGMA journal_mode=WAL;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, v TEXT, u INTEGER UNIQUE);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(a INTEGER, b TEXT);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS wr(x TEXT PRIMARY KEY) WITHOUT ROWID;");
  exec_sql(db, "CREATE TRIGGER IF NOT EXISTS trg AFTER INSERT ON t1 BEGIN INSERT INTO t2(a,b) VALUES (new.id, new.v); END;");

  const uint8_t *p = Data;
  size_t remaining = Size;
  int iterations = (int)(remaining > 0 ? (remaining % 32) + 1 : 1);

  for (int i = 0; i < iterations; i++) {
    uint32_t op = consume_u32(&p, &remaining) % 14;
    uint32_t a = consume_u32(&p, &remaining);
    uint32_t b = consume_u32(&p, &remaining);

    char sql[512];
    sql[0] = '\0';

    switch (op) {
      case 0:
        snprintf(sql, sizeof(sql),
                 "INSERT INTO t1(v,u) VALUES('x%u',%u);",
                 a, b);
        break;
      case 1:
        snprintf(sql, sizeof(sql),
                 "UPDATE t1 SET v='y%u', u=%u WHERE id=%u;",
                 a, b, (a % 16) + 1);
        break;
      case 2:
        snprintf(sql, sizeof(sql),
                 "DELETE FROM t1 WHERE id=%u;",
                 (a % 16) + 1);
        break;
      case 3:
        snprintf(sql, sizeof(sql),
                 "INSERT OR REPLACE INTO t1(id,v,u) VALUES(%u,'r%u',%u);",
                 (a % 16) + 1, b, a);
        break;
      case 4:
        snprintf(sql, sizeof(sql),
                 "INSERT OR IGNORE INTO t1(v,u) VALUES('i%u',%u);",
                 a, b % 8);
        break;
      case 5:
        snprintf(sql, sizeof(sql),
                 "INSERT INTO wr(x) VALUES('w%u');",
                 a);
        break;
      case 6:
        snprintf(sql, sizeof(sql),
                 "UPDATE wr SET x='z%u' WHERE x='w%u';",
                 a, b);
        break;
      case 7:
        snprintf(sql, sizeof(sql),
                 "DELETE FROM wr WHERE x='w%u';",
                 a);
        break;
      case 8:
        snprintf(sql, sizeof(sql),
                 "BEGIN; INSERT INTO t1(v,u) VALUES('tx%u',%u); COMMIT;",
                 a, b);
        break;
      case 9:
        snprintf(sql, sizeof(sql),
                 "BEGIN; INSERT INTO t1(v,u) VALUES('rb%u',%u); ROLLBACK;",
                 a, b);
        break;
      case 10:
        snprintf(sql, sizeof(sql),
                 "SAVEPOINT s; INSERT INTO t1(v,u) VALUES('sp%u',%u); RELEASE s;",
                 a, b);
        break;
      case 11:
        snprintf(sql, sizeof(sql),
                 "ATTACH './dummy_file' AS aux; DETACH aux;");
        break;
      case 12:
        snprintf(sql, sizeof(sql),
                 "PRAGMA wal_checkpoint;");
        break;
      default:
        snprintf(sql, sizeof(sql),
                 "SELECT * FROM t1 WHERE id=%u;",
                 (a % 16) + 1);
        break;
    }

    (void)exec_sql(db, sql);

    if ((a & 1U) == 0) {
      (void)sqlite3_update_hook(db, update_cb, (void *)&state1);
    } else {
      (void)sqlite3_update_hook(db, NULL, (void *)&state1);
      (void)sqlite3_update_hook(db, update_cb, (void *)&state1);
    }

    if ((b & 1U) == 0) {
      (void)sqlite3_wal_hook(db, wal_cb, (void *)&state2);
    } else {
      (void)sqlite3_wal_hook(db, NULL, (void *)&state2);
      (void)sqlite3_wal_hook(db, wal_cb, (void *)&state2);
    }

    {
      volatile int c1 = sqlite3_changes(db);
      volatile sqlite3_int64 c2 = sqlite3_changes64(db);
      volatile sqlite3_int64 lr = sqlite3_last_insert_rowid(db);
      volatile int tc = sqlite3_total_changes(db);
      (void)c1;
      (void)c2;
      (void)lr;
      (void)tc;
    }
  }

  (void)sqlite3_update_hook(db, NULL, NULL);
  (void)sqlite3_wal_hook(db, NULL, NULL);
  sqlite3_close(db);
  return 0;
}