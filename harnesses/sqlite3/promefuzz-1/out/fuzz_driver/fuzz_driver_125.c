// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
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

static uint32_t rd32(const uint8_t *p, size_t n, size_t *off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < n; ++i, ++(*off)) {
    v = (v << 8) | p[*off];
  }
  return v;
}

static char *make_cstring(const uint8_t *Data, size_t Size, size_t *off, size_t max_len) {
  size_t rem = (*off < Size) ? (Size - *off) : 0;
  size_t len = rem ? (rd32(Data, Size, off) % (max_len + 1)) : 0;
  if (len > Size - *off) len = Size - *off;

  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;

  for (size_t i = 0; i < len; ++i) {
    unsigned char c = Data[*off + i];
    if (c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  *off += len;
  return s;
}

static void consume_ptr(const char *p) {
  volatile unsigned char x = 0;
  if (p) {
    x ^= (unsigned char)p[0];
    (void)x;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *created = NULL;
  char *zDb = NULL, *zJournal = NULL, *zWal = NULL;
  const char **azParam = NULL;
  int nParam = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  zDb = make_cstring(Data, Size, &off, 64);
  zJournal = make_cstring(Data, Size, &off, 64);
  zWal = make_cstring(Data, Size, &off, 64);
  if (!zDb || !zJournal || !zWal) goto cleanup;

  nParam = (int)(rd32(Data, Size, &off) % 4);
  if (nParam > 0) {
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!azParam) goto cleanup;
    for (int i = 0; i < nParam * 2; ++i) {
      char *tmp = make_cstring(Data, Size, &off, 32);
      if (!tmp) goto cleanup;
      if ((i % 2) == 0 && tmp[0] == '\0') {
        free(tmp);
        tmp = (char *)malloc(2);
        if (!tmp) goto cleanup;
        tmp[0] = 'k';
        tmp[1] = '\0';
      }
      azParam[i] = tmp;
    }
  }

  sqlite3_initialize();

  created = sqlite3_create_filename(zDb, zJournal, zWal, nParam, azParam);
  if (created) {
    const char *db_name = sqlite3_filename_database(created);
    const char *jrnl_name = sqlite3_filename_journal(created);
    const char *wal_name = sqlite3_filename_wal(created);

    consume_ptr(db_name);
    consume_ptr(jrnl_name);
    consume_ptr(wal_name);

    if (nParam > 0 && azParam[0]) {
      consume_ptr(sqlite3_uri_parameter(created, azParam[0]));
    }
  }

  if (sqlite3_open_v2("./dummy_file", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK && db) {
    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO t VALUES(1);", NULL, NULL, NULL);
    sqlite3_exec(db, "SELECT * FROM t;", NULL, NULL, NULL);

    const char *main_fn = sqlite3_db_filename(db, "main");
    const char *temp_fn = sqlite3_db_filename(db, "temp");
    const char *bogus_fn = sqlite3_db_filename(db, "bogus");

    consume_ptr(main_fn);
    consume_ptr(temp_fn);
    consume_ptr(bogus_fn);

    if (main_fn) {
      const char *m_db = sqlite3_filename_database(main_fn);
      const char *m_j = sqlite3_filename_journal(main_fn);
      const char *m_w = sqlite3_filename_wal(main_fn);
      consume_ptr(m_db);
      consume_ptr(m_j);
      consume_ptr(m_w);
    }

    sqlite3_prepare_v2(db, "SELECT * FROM t;", -1, &stmt, NULL);
    if (stmt) {
      while (sqlite3_step(stmt) == SQLITE_ROW) {
      }
    }
  }

cleanup:
  if (stmt) sqlite3_finalize(stmt);
  if (db) sqlite3_close(db);
  if (created) sqlite3_free_filename(created);

  if (azParam) {
    for (int i = 0; i < nParam * 2; ++i) {
      free((void *)azParam[i]);
    }
    free(azParam);
  }

  free(zDb);
  free(zJournal);
  free(zWal);
  return 0;
}