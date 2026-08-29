// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
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
  char *res;
  va_list ap;
  va_start(ap, fmt);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

int LLVMFuzzerTestOneInput_13(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  char *sql = NULL;
  int rc;
  int ext1;
  int ext2;
  const char *msg1;
  const char *msg2;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) {
      sqlite3_close(db);
    }
    return 0;
  }

  (void)sqlite3_exec(db,
                     "CREATE TABLE IF NOT EXISTS t(a TEXT, b INTEGER);"
                     "INSERT INTO t(a,b) VALUES('x',1),('y',2);",
                     NULL, NULL, NULL);

  if (Size == 0) {
    sql = wrap_vmprintf("%s", "");
  } else {
    int mode = Data[0] % 4;
    const char *payload = (const char *)(Data + 1);
    int payload_len = (int)(Size - 1);

    switch (mode) {
      case 0:
        sql = wrap_vmprintf("%.*s", payload_len, payload);
        break;
      case 1:
        sql = wrap_vmprintf("SELECT * FROM t WHERE a='%.*q';", payload_len, payload);
        break;
      case 2:
        sql = wrap_vmprintf("ATTACH DATABASE './dummy_file' AS aux; %.*s", payload_len, payload);
        break;
      default:
        sql = wrap_vmprintf("CREATE TABLE IF NOT EXISTS x(c); %.*s", payload_len, payload);
        break;
    }
  }

  if (sql == NULL) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail);
  ext1 = sqlite3_extended_errcode(db);
  msg1 = sqlite3_errmsg(db);

  if (stmt != NULL) {
    rc = sqlite3_step(stmt);
    (void)rc;
    rc = sqlite3_reset(stmt);
    (void)rc;
  }

  ext2 = sqlite3_extended_errcode(db);
  msg2 = sqlite3_errmsg(db);

  (void)ext1;
  (void)ext2;
  (void)msg1;
  (void)msg2;
  (void)tail;

  sqlite3_free(sql);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}