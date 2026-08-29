// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_complete at sqlite3.c:151963:16 in sqlite3.h
// sqlite3_complete at sqlite3.c:151963:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static int fuzz_authorizer(void *pUserData, int action, const char *p1,
                           const char *p2, const char *dbName,
                           const char *triggerName) {
  const uint8_t *bytes = (const uint8_t *)pUserData;
  unsigned v = 0;

  if (bytes) {
    v ^= (unsigned)bytes[0];
    v ^= (unsigned)action;
    if (p1) v ^= (unsigned)(unsigned char)p1[0];
    if (p2) v ^= (unsigned)(unsigned char)p2[0];
    if (dbName) v ^= (unsigned)(unsigned char)dbName[0];
    if (triggerName) v ^= (unsigned)(unsigned char)triggerName[0];
  }

  switch (v % 3U) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    default: return SQLITE_DENY;
  }
}

static int fuzz_exec_callback(void *ctx, int argc, char **argv, char **colv) {
  const uint8_t *bytes = (const uint8_t *)ctx;
  unsigned v = (unsigned)argc;
  if (bytes) v ^= bytes[0];
  if (argv && argc > 0 && argv[0]) v ^= (unsigned)(unsigned char)argv[0][0];
  if (colv && argc > 0 && colv[0]) v ^= (unsigned)(unsigned char)colv[0][0];
  return (v & 7U) == 0U ? 1 : 0;
}

int LLVMFuzzerTestOneInput_15(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *mprintf_str = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  char *sql1 = NULL;
  char *sql2 = NULL;
  char *complete_buf = NULL;
  FILE *fp;
  size_t split1, split2;
  int rc;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  split1 = Size > 0 ? (size_t)(Data[0] % (Size + 1)) : 0;
  split2 = Size > 1 ? (size_t)(Data[1] % (Size + 1)) : split1;
  if (split2 < split1) {
    size_t tmp = split1;
    split1 = split2;
    split2 = tmp;
  }

  sql1 = (char *)sqlite3_malloc64(split1 + 1);
  sql2 = (char *)sqlite3_malloc64((Size - split1) + 1);
  complete_buf = (char *)sqlite3_malloc64((split2 - split1) + 2);

  if (sql1) {
    if (split1) memcpy(sql1, Data, split1);
    sql1[split1] = '\0';
  }
  if (sql2) {
    size_t n2 = Size - split1;
    if (n2) memcpy(sql2, Data + split1, n2);
    sql2[n2] = '\0';
  }
  if (complete_buf) {
    size_t n3 = split2 - split1;
    if (n3) memcpy(complete_buf, Data + split1, n3);
    complete_buf[n3] = ';';
    complete_buf[n3 + 1] = '\0';
  }

  sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);

  mprintf_str = sqlite3_mprintf("%s%s%s",
                                sql1 ? sql1 : "",
                                complete_buf ? complete_buf : "",
                                sql2 ? sql2 : "");

  if (complete_buf) {
    (void)sqlite3_complete(complete_buf);
  } else {
    (void)sqlite3_complete("");
  }

  sqlite3_exec(db,
               mprintf_str ? mprintf_str : "",
               fuzz_exec_callback,
               (void *)Data,
               &errmsg1);

  sqlite3_free(mprintf_str);
  sqlite3_free(errmsg1);

  sqlite3_exec(db,
               sql2 ? sql2 : "",
               fuzz_exec_callback,
               (void *)Data,
               &errmsg2);

  sqlite3_free(errmsg2);
  sqlite3_free(sql1);
  sqlite3_free(sql2);
  sqlite3_free(complete_buf);

  sqlite3_close(db);
  return 0;
}