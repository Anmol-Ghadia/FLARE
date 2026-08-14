// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
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

static int fuzz_exec_callback(void *ctx, int argc, char **argv, char **colnames) {
  (void)argc;
  (void)argv;
  (void)colnames;
  const uint8_t *data = (const uint8_t *)ctx;
  if (data == NULL) return 0;
  return (data[0] & 1) ? 1 : 0;
}

#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
static void fuzz_preupdate_hook(
    void *pCtx,
    sqlite3 *db,
    int op,
    char const *zDb,
    char const *zName,
    sqlite3_int64 iKey1,
    sqlite3_int64 iKey2) {
  (void)pCtx;
  (void)op;
  (void)zDb;
  (void)zName;
  (void)iKey1;
  (void)iKey2;

  /* Exercise query-style APIs from within the hook as well. */
  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes64(db);
  (void)sqlite3_last_insert_rowid(db);
}
#endif

int LLVMFuzzerTestOneInput_52(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *sql = NULL;
  char *errmsg = NULL;
  FILE *fp = NULL;
  int rc;
  int open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  if (Data == NULL) {
    return 0;
  }

  /* Create a file-backed database as requested. */
  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    fp = NULL;
  }

  if (Size > 0) {
    if (Data[0] & 0x01) open_flags |= SQLITE_OPEN_URI;
    if (Data[0] & 0x02) open_flags |= SQLITE_OPEN_NOMUTEX;
    if (Data[0] & 0x04) open_flags |= SQLITE_OPEN_FULLMUTEX;
    if (Data[0] & 0x08) open_flags |= SQLITE_OPEN_MEMORY;
  }

  rc = sqlite3_open_v2("./dummy_file", &db, open_flags, NULL);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) {
      sqlite3_close(db);
    }
    return 0;
  }

#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
  if (Size > 1 && (Data[1] & 1)) {
    (void)sqlite3_preupdate_hook(db, fuzz_preupdate_hook, (void *)Data);
  } else {
    (void)sqlite3_preupdate_hook(db, NULL, NULL);
  }
#else
  (void)sqlite3_preupdate_hook;
#endif

  /* Baseline API calls before executing fuzzed SQL. */
  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes64(db);
  (void)sqlite3_last_insert_rowid(db);

  sql = (char *)malloc(Size + 1);
  if (sql == NULL) {
#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
    (void)sqlite3_preupdate_hook(db, NULL, NULL);
#endif
    sqlite3_close(db);
    return 0;
  }

  if (Size > 0) {
    memcpy(sql, Data, Size);
  }
  sql[Size] = '\0';

  /* Seed some schema/data to enable more interesting states. */
  (void)sqlite3_exec(db,
                     "PRAGMA foreign_keys=ON;"
                     "CREATE TABLE IF NOT EXISTS t1(a INTEGER PRIMARY KEY, b TEXT);"
                     "CREATE TABLE IF NOT EXISTS t2(x INTEGER, y TEXT);"
                     "INSERT OR IGNORE INTO t1(a,b) VALUES(1,'seed');",
                     NULL, NULL, &errmsg);
  if (errmsg != NULL) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  /* Query state after setup. */
  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes64(db);
  (void)sqlite3_last_insert_rowid(db);

  /* Execute fuzz SQL without callback. */
  (void)sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg != NULL) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes64(db);
  (void)sqlite3_last_insert_rowid(db);

  /* Execute fuzz SQL again, optionally with aborting callback. */
  (void)sqlite3_exec(
      db,
      sql,
      (Size > 2 && (Data[2] & 1)) ? fuzz_exec_callback : NULL,
      (void *)Data,
      &errmsg);
  if (errmsg != NULL) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes64(db);
  (void)sqlite3_last_insert_rowid(db);

#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
  (void)sqlite3_preupdate_hook(db, NULL, NULL);
#endif

  free(sql);
  sqlite3_close(db);
  return 0;
}