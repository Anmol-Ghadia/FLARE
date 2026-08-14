// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_complete at sqlite3.c:151963:16 in sqlite3.h
// sqlite3_complete at sqlite3.c:151963:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *dbName, const char *triggerName) {
  const unsigned char *bytes = (const unsigned char *)pUserData;
  unsigned char selector = 0;

  (void)action;
  (void)p1;
  (void)p2;
  (void)dbName;
  (void)triggerName;

  if (bytes) {
    selector = bytes[0] % 3;
  }

  if (selector == 0) return SQLITE_OK;
  if (selector == 1) return SQLITE_IGNORE;
  return SQLITE_DENY;
}

static int fuzz_exec_callback(void *ctx, int argc, char **argv, char **colNames) {
  const unsigned char *bytes = (const unsigned char *)ctx;
  int i;
  volatile size_t sink = 0;

  for (i = 0; i < argc; i++) {
    if (argv && argv[i]) sink += (unsigned char)argv[i][0];
    if (colNames && colNames[i]) sink += (unsigned char)colNames[i][0];
  }

  if (bytes && (bytes[0] & 1)) {
    return 1;
  }
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *mprintf_str = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  char *sql_buf = NULL;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sql_buf = (char *)malloc(Size + 1);
  if (!sql_buf) {
    return 0;
  }
  if (Size > 0) {
    memcpy(sql_buf, Data, Size);
  }
  sql_buf[Size] = '\0';

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      sqlite3_close(db);
    }
    free(sql_buf);
    return 0;
  }

  sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);

  mprintf_str = sqlite3_mprintf("%s", sql_buf);

  if (mprintf_str) {
    (void)sqlite3_complete(mprintf_str);
  } else {
    (void)sqlite3_complete(sql_buf);
  }

  (void)sqlite3_exec(db,
                     mprintf_str ? mprintf_str : sql_buf,
                     fuzz_exec_callback,
                     (void *)Data,
                     &errmsg1);

  sqlite3_free(mprintf_str);
  sqlite3_free(errmsg1);

  (void)sqlite3_exec(db,
                     sql_buf,
                     (Size > 1 && (Data[0] & 2)) ? fuzz_exec_callback : NULL,
                     (void *)Data,
                     &errmsg2);

  sqlite3_free(errmsg2);
  sqlite3_close(db);
  free(sql_buf);
  return 0;
}