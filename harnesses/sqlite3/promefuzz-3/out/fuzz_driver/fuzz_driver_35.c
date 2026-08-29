// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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
#include <sqlite3.h>

static unsigned int read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  unsigned int v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *dyn = NULL;
  int rc;
  int ec1, ec2;
  const char *msg;
  unsigned int mode;
  unsigned int sql_len;
  unsigned int blob_len;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  mode = read_u32(Data, Size, &off) % 4;

  if (mode == 0) {
    rc = sqlite3_open(":memory:", &db);
  } else if (mode == 1) {
    rc = sqlite3_open("./dummy_file", &db);
  } else if (mode == 2) {
    rc = sqlite3_open_v2("./dummy_file", &db,
                         SQLITE_OPEN_READONLY, NULL);
  } else {
    rc = sqlite3_open_v2("./dummy_file", &db,
                         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  }

  if (db) {
    sqlite3_extended_result_codes(db, 1);

    if (off < Size) {
      switch (Data[off++] % 6) {
        case 0:
          sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
          sqlite3_exec(db, "INSERT INTO t(x) VALUES(1);", NULL, NULL, NULL);
          sqlite3_exec(db, "SELECT * FROM no_such_table;", NULL, NULL, NULL);
          break;
        case 1:
          sqlite3_exec(db, "CREATE TABLE t(x); CREATE TABLE t(x);", NULL, NULL, NULL);
          break;
        case 2:
          sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
          sqlite3_exec(db, "SELECT * FROM sqlite_master WHERE", NULL, NULL, NULL);
          break;
        case 3:
          sqlite3_exec(db, "ATTACH DATABASE '/definitely/not/present/db' AS aux;", NULL, NULL, NULL);
          break;
        case 4:
          sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t2(y UNIQUE);", NULL, NULL, NULL);
          sqlite3_exec(db, "INSERT INTO t2(y) VALUES(1);", NULL, NULL, NULL);
          sqlite3_exec(db, "INSERT INTO t2(y) VALUES(1);", NULL, NULL, NULL);
          break;
        default: {
          sql_len = (Size > off) ? (unsigned int)(Size - off) : 0;
          if (sql_len > 256) sql_len = 256;
          if (sql_len > 0) {
            char sqlbuf[257];
            memcpy(sqlbuf, Data + off, sql_len);
            sqlbuf[sql_len] = '\0';
            sqlite3_exec(db, sqlbuf, NULL, NULL, NULL);
            off += sql_len;
          } else {
            sqlite3_exec(db, "", NULL, NULL, NULL);
          }
          break;
        }
      }
    }

    if (off < Size) {
      blob_len = (unsigned int)(Size - off);
      if (blob_len > 128) blob_len = 128;
      rc = sqlite3_prepare_v2(db, (const char *)(Data + off), (int)blob_len, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        sqlite3_step(stmt);
      }
    }
  } else {
    rc = SQLITE_NOMEM;
  }

  ec1 = sqlite3_errcode(db);

  if (Size == 0) {
    dyn = sqlite3_mprintf("%s", "");
  } else {
    size_t rem = Size - (off < Size ? off : Size);
    size_t n = rem > 64 ? 64 : rem;
    char tmp[65];
    if (n > 0) {
      memcpy(tmp, Data + off, n);
    }
    tmp[n] = '\0';
    dyn = sqlite3_mprintf("rc=%d ec=%d s=%s", rc, ec1, tmp);
  }

  msg = sqlite3_errmsg(db);
  (void)msg;

  ec2 = sqlite3_extended_errcode(db);
  (void)ec2;

  if (dyn) {
    volatile size_t sink = strlen(dyn);
    (void)sink;
    sqlite3_free(dyn);
  }

  if (stmt) {
    sqlite3_finalize(stmt);
  }
  if (db) {
    sqlite3_close(db);
  }

  return 0;
}