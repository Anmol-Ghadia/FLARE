// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_complete at sqlite3.c:151963:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *dbName, const char *triggerName) {
  const uint8_t *bytes = (const uint8_t *)pUserData;
  unsigned selector = 0;

  if (bytes) {
    selector ^= bytes[0];
    selector ^= (unsigned)action;
    if (p1) selector ^= (unsigned char)p1[0];
    if (p2) selector ^= (unsigned char)p2[0];
    if (dbName) selector ^= (unsigned char)dbName[0];
    if (triggerName) selector ^= (unsigned char)triggerName[0];
  }

  switch (selector % 3) {
    case 0:
      return SQLITE_OK;
    case 1:
      return SQLITE_IGNORE;
    default:
      return SQLITE_DENY;
  }
}

static int fuzz_exec_callback(void *ctx, int argc, char **argv, char **colv) {
  volatile unsigned sink = 0;
  int i;
  const uint8_t *bytes = (const uint8_t *)ctx;

  if (bytes) sink ^= bytes[0];

  for (i = 0; i < argc; ++i) {
    if (argv && argv[i] && argv[i][0]) sink ^= (unsigned char)argv[i][0];
    if (colv && colv[i] && colv[i][0]) sink ^= (unsigned char)colv[i][0];
  }

  if (bytes && (bytes[0] & 1)) {
    return (sink & 1) ? 1 : 0;
  }
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *mprintf_str = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  char *sql_input = NULL;
  int complete_rc;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      sqlite3_close(db);
    }
    return 0;
  }

  sql_input = (char *)sqlite3_malloc64(Size + 1);
  if (!sql_input) {
    sqlite3_close(db);
    return 0;
  }
  if (Size > 0) {
    memcpy(sql_input, Data, Size);
  }
  sql_input[Size] = '\0';

  sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);

  mprintf_str = sqlite3_mprintf("%s", sql_input);

  complete_rc = sqlite3_complete(sql_input);
  (void)complete_rc;

  sqlite3_exec(db, sql_input, fuzz_exec_callback, (void *)Data, &errmsg1);

  sqlite3_free(mprintf_str);
  sqlite3_free(errmsg1);

  if (Data && Size > 0 && (Data[0] & 2)) {
    sqlite3_set_authorizer(db, NULL, NULL);
  }

  sqlite3_exec(db, "PRAGMA integrity_check; SELECT name FROM sqlite_master;", NULL, NULL, &errmsg2);

  sqlite3_close(db);

  sqlite3_free(errmsg2);
  sqlite3_free(sql_input);
  return 0;
}