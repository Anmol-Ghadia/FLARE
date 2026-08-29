// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_get_table at sqlite3.c:126741:16 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_vsnprintf at sqlite3.c:17488:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_get_table at sqlite3.c:126741:16 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
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
#include <stdarg.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t max_len) {
  size_t len = *n;
  if (len > max_len) len = max_len;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static void call_vsnprintf_wrapper(int n, char *buf, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  int rc;
  sqlite3 *db = NULL;
  char *errmsg = NULL;
  char **table = NULL;
  int nRow = 0, nCol = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "PRAGMA journal_mode=OFF;"
               "PRAGMA synchronous=OFF;"
               "CREATE TABLE IF NOT EXISTS t(a,b,c);"
               "CREATE TABLE IF NOT EXISTS u(x);",
               NULL, NULL, NULL);

  char *sql = make_cstring(&p, &n, 4096);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, sql, NULL, NULL, NULL);

  rc = sqlite3_get_table(db, sql, &table, &nRow, &nCol, &errmsg);
  sqlite3_log(rc, "sqlite3_get_table rc=%d rows=%d cols=%d err=%s sql=%s",
              rc, nRow, nCol,
              errmsg ? errmsg : "(null)",
              sql);

  {
    const char *errstr = sqlite3_errstr(rc);
    char stackbuf1[256];
    char stackbuf2[256];
    int errcode1 = read_int(&p, &n);
    int errcode2 = read_int(&p, &n);

    sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1,
                     "err=%d errstr=%s sql=%s rows=%d cols=%d",
                     errcode1,
                     sqlite3_errstr(errcode1),
                     sql,
                     nRow,
                     nCol);

    call_vsnprintf_wrapper((int)sizeof(stackbuf2), stackbuf2,
                           "rc=%d err2=%d errstr2=%s first=%c",
                           rc,
                           errcode2,
                           sqlite3_errstr(errcode2),
                           sql[0] ? sql[0] : '?');

    char *dyn1 = sqlite3_mprintf("SQL:[%q] ERR:[%s] LOG:[%s]",
                                 sql,
                                 errstr ? errstr : "(null)",
                                 stackbuf1);
    if (dyn1) {
      sqlite3_log(errcode1, "%s", dyn1);
      sqlite3_free(dyn1);
    }

    char *dyn2 = sqlite3_mprintf("BUF2:%s FILE:%Q SIZE:%d",
                                 stackbuf2,
                                 "./dummy_file",
                                 (int)Size);
    if (dyn2) {
      sqlite3_log(errcode2, "%s", dyn2);
      sqlite3_free(dyn2);
    }
  }

  {
    const char *fallback_sqls[] = {
      "SELECT 1;",
      "SELECT * FROM t;",
      "INSERT INTO t(a,b,c) VALUES(1,2,3); SELECT * FROM t;",
      "CREATE TABLE IF NOT EXISTS v(y); SELECT name FROM sqlite_master;",
      "ATTACH './dummy_file' AS aux; SELECT * FROM sqlite_master; DETACH aux;"
    };
    size_t idx = n ? (size_t)(p[0] % (sizeof(fallback_sqls)/sizeof(fallback_sqls[0]))) : 0;

    sqlite3_free_table(table);
    table = NULL;
    sqlite3_free(errmsg);
    errmsg = NULL;
    nRow = nCol = 0;

    rc = sqlite3_get_table(db, fallback_sqls[idx], &table, &nRow, &nCol, &errmsg);
    sqlite3_log(rc, "fallback rc=%d rows=%d cols=%d msg=%s",
                rc, nRow, nCol, errmsg ? errmsg : "(null)");
  }

  sqlite3_free_table(table);
  sqlite3_free(errmsg);
  free(sql);
  sqlite3_close(db);
  return 0;
}