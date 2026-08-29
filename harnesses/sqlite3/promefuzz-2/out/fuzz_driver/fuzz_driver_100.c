// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_collation_needed16 at sqlite3.c:155824:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation16 at sqlite3.c:155773:16 in sqlite3.h
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

static int g_abort_after = -1;
static int g_callback_count = 0;

static int fuzz_exec_callback(void *opaque, int argc, char **argv, char **colnames) {
  (void)opaque;
  (void)argc;
  (void)argv;
  (void)colnames;
  g_callback_count++;
  if (g_abort_after >= 0 && g_callback_count > g_abort_after) {
    return 1;
  }
  return 0;
}

static void fuzz_coll_needed(void *pArg, sqlite3 *db, int eTextRep, const char *zName) {
  (void)pArg;
  if (!db || !zName) return;
  sqlite3_create_collation(db, zName, eTextRep, NULL, NULL);
}

static void fuzz_coll_needed16(void *pArg, sqlite3 *db, int eTextRep, const void *zName16) {
  (void)pArg;
  if (!db || !zName16) return;
  sqlite3_create_collation16(db, zName16, eTextRep, NULL, NULL);
}

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int choose_flags(uint32_t selector) {
  static const int flag_sets[] = {
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
    SQLITE_OPEN_READONLY,
    SQLITE_OPEN_READWRITE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE
  };
  return flag_sets[selector % (sizeof(flag_sets) / sizeof(flag_sets[0]))];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  uint32_t sel_flags = read_u32(&p, &remaining);
  uint32_t sel_wal = read_u32(&p, &remaining);
  uint32_t sel_cb = read_u32(&p, &remaining);
  uint32_t sel_vfs = read_u32(&p, &remaining);

  int flags = choose_flags(sel_flags);

  const char *filename = "./dummy_file";
  if ((sel_flags & 1U) && (flags & SQLITE_OPEN_MEMORY)) {
    filename = ":memory:";
  } else if (sel_flags & 2U) {
    filename = "";
  }

  const char *zVfs = NULL;
  if (sel_vfs & 1U) {
    zVfs = "unix";
  } else if (sel_vfs & 2U) {
    zVfs = "win32";
  }

  sqlite3 *db = NULL;
  int rc = sqlite3_open_v2(filename, &db, flags, zVfs);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      (void)sqlite3_errcode(db);
      sqlite3_close(db);
    }
    return 0;
  }

  (void)sqlite3_collation_needed(db, (void *)p, fuzz_coll_needed);
  (void)sqlite3_collation_needed16(db, (void *)p, fuzz_coll_needed16);

  int wal_n = (int)(sel_wal % 4097U) - 1024;
  (void)sqlite3_wal_autocheckpoint(db, wal_n);
  (void)sqlite3_wal_autocheckpoint(db, 0);
  (void)sqlite3_wal_autocheckpoint(db, 1 + (int)(sel_wal % 128U));

  char *sql = (char *)malloc(remaining + 1);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }
  memcpy(sql, p, remaining);
  sql[remaining] = '\0';

  char *errmsg = NULL;
  g_callback_count = 0;
  g_abort_after = (sel_cb & 1U) ? (int)(sel_cb % 8U) : -1;

  if (sel_cb & 2U) {
    rc = sqlite3_exec(db, sql, fuzz_exec_callback, db, &errmsg);
  } else {
    rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  }
  (void)rc;
  (void)sqlite3_errcode(db);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  rc = sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, &errmsg);
  (void)rc;
  (void)sqlite3_errcode(db);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  g_callback_count = 0;
  g_abort_after = (sel_cb & 4U) ? 0 : -1;
  rc = sqlite3_exec(db, sql, fuzz_exec_callback, NULL, &errmsg);
  (void)rc;
  (void)sqlite3_errcode(db);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  rc = sqlite3_exec(db, "", NULL, NULL, &errmsg);
  (void)rc;
  (void)sqlite3_errcode(db);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  free(sql);
  sqlite3_close(db);
  return 0;
}