// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *p3, const char *p4) {
  const uint8_t *data = (const uint8_t *)pUserData;
  unsigned v = 0;

  if (data) {
    v ^= (unsigned)action;
    if (p1) v ^= (unsigned char)p1[0];
    if (p2) v ^= (unsigned char)p2[0];
    if (p3) v ^= (unsigned char)p3[0];
    if (p4) v ^= (unsigned char)p4[0];
    v ^= data[0];
  }

  switch (v % 4) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    case 2: return SQLITE_DENY;
    default: return 999;
  }
}

static unsigned int pick_prep_flags(uint8_t b) {
  unsigned int flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
  if (b & 0x01) flags |= SQLITE_PREPARE_PERSISTENT;
#endif
#ifdef SQLITE_PREPARE_NORMALIZE
  if (b & 0x02) flags |= SQLITE_PREPARE_NORMALIZE;
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
  if (b & 0x04) flags |= SQLITE_PREPARE_NO_VTAB;
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
  if (b & 0x08) flags |= SQLITE_PREPARE_DONT_LOG;
#endif
  return flags;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  sqlite3_stmt *it = NULL;
  const char *tail1 = NULL;
  const char *tail2 = NULL;
  int rc;
  char *sqlbuf = NULL;
  size_t sql_len = 0;
  int nByte1, nByte2;
  unsigned int prepFlags = 0;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      (void)sqlite3_errmsg16(db);
      sqlite3_close(db);
    } else {
      (void)sqlite3_errmsg16(NULL);
    }
    return 0;
  }

  (void)sqlite3_exec(db,
                     "CREATE TABLE IF NOT EXISTS t(a,b,c);"
                     "CREATE VIEW IF NOT EXISTS v AS SELECT a,b,c FROM t;",
                     NULL, NULL, NULL);

  if (Size == 0) {
    static const char fallback[] = "SELECT 1;";
    sqlbuf = sqlite3_malloc((int)sizeof(fallback));
    if (!sqlbuf) {
      (void)sqlite3_errmsg16(db);
      sqlite3_close(db);
      return 0;
    }
    memcpy(sqlbuf, fallback, sizeof(fallback));
    sql_len = sizeof(fallback) - 1;
    prepFlags = 0;
    nByte1 = -1;
    nByte2 = -1;
  } else {
    prepFlags = pick_prep_flags(Data[0]);
    sql_len = Size - 1;
    sqlbuf = sqlite3_malloc((int)sql_len + 1);
    if (!sqlbuf) {
      (void)sqlite3_errmsg16(db);
      sqlite3_close(db);
      return 0;
    }
    if (sql_len) {
      memcpy(sqlbuf, Data + 1, sql_len);
    }
    sqlbuf[sql_len] = '\0';

    switch (Data[0] % 4) {
      case 0:
        nByte1 = -1;
        break;
      case 1:
        nByte1 = (int)sql_len;
        break;
      case 2:
        nByte1 = (int)(sql_len / 2);
        break;
      default:
        nByte1 = (int)((sql_len > 0) ? ((Data[0] % sql_len) + 1) : 0);
        break;
    }

    switch ((Data[0] >> 2) % 4) {
      case 0:
        nByte2 = -1;
        break;
      case 1:
        nByte2 = (int)sql_len;
        break;
      case 2:
        nByte2 = (int)(sql_len / 2);
        break;
      default:
        nByte2 = (int)((sql_len > 0) ? (((Data[0] >> 4) % sql_len) + 1) : 0);
        break;
    }
  }

  if (Size > 1) {
    if (Data[1] & 1) {
      (void)sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
    } else {
      (void)sqlite3_set_authorizer(db, NULL, NULL);
    }
  } else {
    (void)sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
  }

  rc = sqlite3_prepare_v2(db, sqlbuf, nByte1, &stmt1, &tail1);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg16(db);
  }
  if (stmt1) {
    (void)sqlite3_db_handle(stmt1);
  }

  rc = sqlite3_prepare_v3(db, sqlbuf, nByte2, prepFlags, &stmt2, &tail2);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg16(db);
  }
  if (stmt2) {
    (void)sqlite3_db_handle(stmt2);
  }

  it = NULL;
  while ((it = sqlite3_next_stmt(db, it)) != NULL) {
    sqlite3 *owner = sqlite3_db_handle(it);
    if (owner) {
      (void)sqlite3_errmsg16(owner);
    }
  }

  (void)sqlite3_set_authorizer(db, NULL, NULL);

  if (stmt1) sqlite3_finalize(stmt1);
  if (stmt2) sqlite3_finalize(stmt2);
  if (sqlbuf) sqlite3_free(sqlbuf);
  sqlite3_close(db);
  return 0;
}