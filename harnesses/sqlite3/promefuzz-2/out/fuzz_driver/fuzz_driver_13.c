// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static char *call_vmprintf(const char *fmt, ...) {
  char *out = NULL;
  va_list ap;
  va_start(ap, fmt);
  out = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  char *sql = NULL;
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

  sqlite3_extended_result_codes(db, 1);

  sql = call_vmprintf("%.*s", (int)Size, (const char *)Data);

  rc = sqlite3_prepare_v2(db, sql ? sql : "", -1, &stmt, &tail);
  (void)rc;

  (void)sqlite3_extended_errcode(db);
  (void)sqlite3_errmsg(db);

  if (stmt != NULL) {
    int steps = 0;
    do {
      rc = sqlite3_step(stmt);
      steps++;
    } while (steps < 4 && rc == SQLITE_ROW);
    (void)sqlite3_reset(stmt);
  } else {
    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);
  }

  (void)sqlite3_extended_errcode(db);
  (void)sqlite3_errmsg(db);

  sqlite3_free(sql);
  (void)sqlite3_finalize(stmt);

  sqlite3_close(db);
  return 0;
}