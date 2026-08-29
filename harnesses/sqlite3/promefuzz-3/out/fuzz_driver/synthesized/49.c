// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_set_last_insert_rowid at sqlite3.c:153320:17 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static int g_commit_mode = 0;
static uint8_t g_commit_byte = 0;

static int fuzz_commit_hook(void *pArg) {
  (void)pArg;
  if (g_commit_mode == 0) return 0;
  if (g_commit_mode == 1) return 1;
  return (g_commit_byte & 1) ? 1 : 0;
}

static uint64_t read_u64(const uint8_t **p, size_t *n) {
  uint64_t v = 0;
  size_t take = *n < 8 ? *n : 8;
  size_t i;
  for (i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

int LLVMFuzzerTestOneInput_49(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  int rc;
  const uint8_t *p = Data;
  size_t n = Size;
  char filename[512];
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  if (n == 0) {
    rc = sqlite3_open(":memory:", &db);
  } else {
    uint8_t mode = *p++;
    n--;

    if ((mode % 4) == 0) {
      rc = sqlite3_open(":memory:", &db);
    } else if ((mode % 4) == 1) {
      rc = sqlite3_open("", &db);
    } else if ((mode % 4) == 2) {
      rc = sqlite3_open("./dummy_file", &db);
    } else {
      size_t copy = n < sizeof(filename) - 1 ? n : sizeof(filename) - 1;
      size_t i;
      memcpy(filename, p, copy);
      filename[copy] = '\0';
      for (i = 0; i < copy; i++) {
        if (filename[i] == '\0') filename[i] = 'A';
      }
      rc = sqlite3_open(filename, &db);
      p += copy;
      n -= copy;
    }
  }

  if (db == NULL) {
    return 0;
  }

  (void)rc;

  g_commit_mode = (n > 0) ? (*p++ % 3) : 0;
  if (n > 0) n--;
  g_commit_byte = (n > 0) ? *p++ : 0;
  if (n > 0) n--;

  (void)sqlite3_commit_hook(db, fuzz_commit_hook, NULL);

  (void)sqlite3_changes(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);

  {
    sqlite3_int64 rowid = (sqlite3_int64)read_u64(&p, &n);
    sqlite3_set_last_insert_rowid(db, rowid);
    (void)sqlite3_last_insert_rowid(db);
  }

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER PRIMARY KEY, b TEXT);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS u(x INTEGER);");
  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT INTO t(b) VALUES('x');");
  (void)sqlite3_changes(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);
  exec_sql(db, "UPDATE t SET b='y' WHERE a=1;");
  (void)sqlite3_changes(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);
  exec_sql(db, "DELETE FROM t WHERE a=1;");
  (void)sqlite3_changes(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);
  exec_sql(db, "COMMIT;");

  if (n > 0) {
    size_t sql_len = n < 256 ? n : 256;
    char sqlbuf[257];
    size_t i;
    memcpy(sqlbuf, p, sql_len);
    sqlbuf[sql_len] = '\0';
    for (i = 0; i < sql_len; i++) {
      unsigned char c = (unsigned char)sqlbuf[i];
      if (c == '\0' || c == '\r' || c == '\n') sqlbuf[i] = ' ';
    }
    exec_sql(db, sqlbuf);
    (void)sqlite3_changes(db);
    (void)sqlite3_total_changes(db);
    (void)sqlite3_total_changes64(db);
  }

  (void)sqlite3_commit_hook(db, NULL, NULL);
  sqlite3_close(db);
  return 0;
}