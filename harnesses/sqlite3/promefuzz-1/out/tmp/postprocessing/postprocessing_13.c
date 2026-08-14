#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static char *wrap_vmprintf(const char *fmt, ...) {
  char *res = NULL;
  va_list ap;
  va_start(ap, fmt);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  char *formatted = NULL;
  char *sqlbuf = NULL;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) {
      sqlite3_close(db);
    }
    return 0;
  }

  sqlbuf = (char *)sqlite3_malloc(Size + 1);
  if (sqlbuf == NULL) {
    sqlite3_close(db);
    return 0;
  }
  if (Size > 0) {
    memcpy(sqlbuf, Data, Size);
  }
  sqlbuf[Size] = '\0';

  formatted = wrap_vmprintf("%s", sqlbuf);

  rc = sqlite3_prepare_v2(db,
                          formatted ? formatted : sqlbuf,
                          formatted ? (int)strlen(formatted) : (int)Size,
                          &stmt,
                          &tail);

  (void)sqlite3_extended_errcode(db);
  (void)sqlite3_errmsg(db);

  if (stmt != NULL) {
    int steps = 0;
    do {
      rc = sqlite3_step(stmt);
      steps++;
    } while (rc == SQLITE_ROW && steps < 8);

    (void)sqlite3_reset(stmt);
  }

  (void)sqlite3_extended_errcode(db);
  (void)sqlite3_errmsg(db);

  sqlite3_free(formatted);
  sqlite3_finalize(stmt);
  sqlite3_free(sqlbuf);
  sqlite3_close(db);
  return 0;
}