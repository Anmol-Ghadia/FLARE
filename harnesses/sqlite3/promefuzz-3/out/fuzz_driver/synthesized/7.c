// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static int g_progress_mode = 0;

static int fuzz_progress(void *pArg) {
  const uint8_t *data = (const uint8_t *)pArg;
  if (data == NULL) return 0;
  switch (g_progress_mode & 3) {
    case 0:
      return 0;
    case 1:
      return (data[0] & 1) ? 1 : 0;
    case 2:
      return (data[0] == 0xFF) ? 1 : 0;
    default:
      return (data[0] % 3) == 0;
  }
}

int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  int rc;
  FILE *fp;
  int flags;
  const char *zVfs = NULL;
  int nOps;
  int limit_id1, limit_id2;
  int new_limit1, new_limit2;
  sqlite3_int64 heap_limit;
  int op_selector;
  int out_int = 0;
  char dbname_buf[32];

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (Size > 0) {
    if (Data[0] & 0x01) flags |= SQLITE_OPEN_URI;
    if (Data[0] & 0x02) flags |= SQLITE_OPEN_MEMORY;
    if (Data[0] & 0x04) flags |= SQLITE_OPEN_NOMUTEX;
    if (Data[0] & 0x08) flags |= SQLITE_OPEN_FULLMUTEX;
    if (Data[0] & 0x10) flags |= SQLITE_OPEN_SHAREDCACHE;
    if (Data[0] & 0x20) flags |= SQLITE_OPEN_PRIVATECACHE;
  }

  if (Size > 1 && (Data[1] & 1)) {
    zVfs = "";
  }

  rc = sqlite3_open_v2("./dummy_file", &db, flags, zVfs);
  (void)rc;
  if (db == NULL) {
    return 0;
  }

  g_progress_mode = (Size > 2) ? Data[2] : 0;
  nOps = (Size > 3) ? ((int)Data[3] - 64) : 1;
  if (Size > 4 && (Data[4] & 1)) {
    sqlite3_progress_handler(db, nOps, fuzz_progress, (void *)Data);
  } else {
    sqlite3_progress_handler(db, nOps, NULL, (void *)Data);
  }

#ifdef SQLITE_LIMIT_WORKER_THREADS
  limit_id1 = (Size > 5) ? (Data[5] % (SQLITE_LIMIT_WORKER_THREADS + 1)) : 0;
#else
  limit_id1 = (Size > 5) ? (Data[5] % 12) : 0;
#endif
  new_limit1 = (Size > 9)
      ? (int)((int32_t)(
            ((uint32_t)Data[6] << 24) |
            ((uint32_t)Data[7] << 16) |
            ((uint32_t)Data[8] << 8) |
            (uint32_t)Data[9]))
      : -1;
  (void)sqlite3_limit(db, limit_id1, new_limit1);

  heap_limit = -1;
  if (Size > 17) {
    heap_limit =
        (sqlite3_int64)(
          ((uint64_t)Data[10] << 56) |
          ((uint64_t)Data[11] << 48) |
          ((uint64_t)Data[12] << 40) |
          ((uint64_t)Data[13] << 32) |
          ((uint64_t)Data[14] << 24) |
          ((uint64_t)Data[15] << 16) |
          ((uint64_t)Data[16] << 8) |
          (uint64_t)Data[17]);
    if (Size > 18 && (Data[18] & 1)) {
      heap_limit = -heap_limit;
    }
  }
  (void)sqlite3_hard_heap_limit64(heap_limit);

#ifdef SQLITE_LIMIT_WORKER_THREADS
  limit_id2 = (Size > 19) ? (Data[19] % (SQLITE_LIMIT_WORKER_THREADS + 1)) : 0;
#else
  limit_id2 = (Size > 19) ? (Data[19] % 12) : 0;
#endif
  new_limit2 = (Size > 23)
      ? (int)((int32_t)(
            ((uint32_t)Data[20] << 24) |
            ((uint32_t)Data[21] << 16) |
            ((uint32_t)Data[22] << 8) |
            (uint32_t)Data[23]))
      : -1;
  (void)sqlite3_limit(db, limit_id2, new_limit2);

  op_selector = (Size > 24) ? (Data[24] % 6) : 0;
  switch (op_selector) {
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
    case 0:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY,
                              (Size > 25) ? (Data[25] & 1) : 0, &out_int);
      break;
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
    case 1:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER,
                              (Size > 25) ? (Data[25] & 1) : 0, &out_int);
      break;
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
    case 2:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW,
                              (Size > 25) ? (Data[25] & 1) : 0, &out_int);
      break;
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_ALTER_TABLE
    case 3:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_LEGACY_ALTER_TABLE,
                              (Size > 25) ? (Data[25] & 1) : 0, &out_int);
      break;
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
    case 4:
      snprintf(dbname_buf, sizeof(dbname_buf), "db_%u",
               (unsigned)((Size > 25) ? Data[25] : 0));
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, dbname_buf);
      break;
#endif
    default:
#ifdef SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE,
                              (Size > 25) ? (Data[25] & 1) : 0, &out_int);
#else
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY,
                              0, &out_int);
#endif
      break;
  }

  sqlite3_close(db);
  return 0;
}