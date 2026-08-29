// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static void trace_cb(void *pArg, const char *zSql) {
  (void)pArg;
  (void)zSql;
}

static int exec_cb(void *ctx, int argc, char **argv, char **colv) {
  int *mode = (int *)ctx;
  int i;
  volatile unsigned sink = 0;

  for (i = 0; i < argc; ++i) {
    if (argv && argv[i]) sink ^= (unsigned char)argv[i][0];
    if (colv && colv[i]) sink ^= (unsigned char)colv[i][0];
  }

  if (mode && (*mode & 1)) {
    return 1;
  }
  return (int)(sink & 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *errmsg = NULL;
  char *formatted = NULL;
  int rc;
  int cb_mode = 0;
  FILE *fp;
  char sqlbuf[4096];
  char textbuf[1024];
  size_t sql_len;
  size_t text_len;
  size_t i;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK && db == NULL) {
    return 0;
  }
  if (db == NULL) {
    return 0;
  }

  sqlite3_trace(db, trace_cb, NULL);

  text_len = Size;
  if (text_len > sizeof(textbuf) - 1) {
    text_len = sizeof(textbuf) - 1;
  }
  for (i = 0; i < text_len; ++i) {
    unsigned char c = Data[i];
    if (c == '\0') c = 'A';
    textbuf[i] = (char)c;
  }
  textbuf[text_len] = '\0';

  formatted = sqlite3_mprintf(
      "BEGIN;"
      "CREATE TABLE IF NOT EXISTS t(a,b,c);"
      "INSERT INTO t(a,b,c) VALUES(%d,%Q,%Q);"
      "SELECT * FROM t WHERE a=%d;"
      "COMMIT;",
      (int)(Size & 0x7fffffff),
      textbuf,
      textbuf,
      (int)(Size & 0x7fffffff));

  if (formatted) {
    sqlite3_exec(db, formatted, NULL, NULL, &errmsg);
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  sql_len = Size;
  if (sql_len > sizeof(sqlbuf) - 1) {
    sql_len = sizeof(sqlbuf) - 1;
  }
  for (i = 0; i < sql_len; ++i) {
    unsigned char c = Data[i];
    if (c == '\0') c = ' ';
    sqlbuf[i] = (char)c;
  }
  sqlbuf[sql_len] = '\0';

  cb_mode = (Size > 0) ? (Data[0] & 1) : 0;
  sqlite3_exec(db, sqlbuf, exec_cb, &cb_mode, &errmsg);
  sqlite3_free(errmsg);
  errmsg = NULL;

  sqlite3_exec(db, "", NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
  errmsg = NULL;

  sqlite3_exec(db, "SELECT 1; SELECT 2;", exec_cb, &cb_mode, &errmsg);
  sqlite3_free(errmsg);
  errmsg = NULL;

  sqlite3_free(formatted);
  sqlite3_trace(db, NULL, NULL);
  sqlite3_close(db);
  return 0;
}