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

static unsigned int rd32(const uint8_t *p, size_t n, size_t *off) {
  unsigned int v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < n; ++i, ++(*off)) {
    v = (v << 8) | p[*off];
  }
  return v;
}

static char *take_cstring(const uint8_t *Data, size_t Size, size_t *off, size_t max_len) {
  size_t remain, len;
  char *s;
  if (*off >= Size) {
    s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }
  remain = Size - *off;
  len = rd32(Data, Size, off);
  if (max_len > 0 && len > max_len) len = max_len;
  if (len > Size - *off) len = Size - *off;
  s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, Data + *off, len);
  s[len] = '\0';
  *off += len;
  (void)remain;
  return s;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  char *db_name = NULL;
  char *journal_name = NULL;
  char *wal_name = NULL;
  char **params = NULL;
  int nParam = 0;
  char *created = NULL;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *dbfile_main = NULL;
  const char *dbfile_temp = NULL;
  const char *dbfile_invalid = NULL;
  const char *p_db = NULL;
  const char *p_journal = NULL;
  const char *p_wal = NULL;
  int i;

  write_dummy_file(Data, Size);

  db_name = take_cstring(Data, Size, &off, 256);
  journal_name = take_cstring(Data, Size, &off, 256);
  wal_name = take_cstring(Data, Size, &off, 256);
  if (!db_name || !journal_name || !wal_name) goto cleanup;

  nParam = (int)(rd32(Data, Size, &off) % 8);
  if (nParam > 0) {
    params = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!params) goto cleanup;
    for (i = 0; i < nParam * 2; ++i) {
      params[i] = take_cstring(Data, Size, &off, 64);
      if (!params[i]) goto cleanup;
      if ((i % 2) == 0 && params[i][0] == '\0') {
        free(params[i]);
        params[i] = (char *)malloc(2);
        if (!params[i]) goto cleanup;
        params[i][0] = 'k';
        params[i][1] = '\0';
      }
    }
  }

  created = sqlite3_create_filename(
      db_name,
      journal_name,
      wal_name,
      nParam,
      (const char **)params);

  if (created) {
    p_db = sqlite3_filename_database(created);
    p_journal = sqlite3_filename_journal(created);
    p_wal = sqlite3_filename_wal(created);

    if (p_db) (void)sqlite3_filename_database(created);
    if (p_journal) (void)sqlite3_filename_journal(created);
    if (p_wal) (void)sqlite3_filename_wal(created);

    if (nParam > 0 && params && params[0]) {
      (void)sqlite3_uri_parameter(created, params[0]);
      (void)sqlite3_uri_boolean(created, params[0], 0);
      (void)sqlite3_uri_int64(created, params[0], 0);
    }
    (void)sqlite3_uri_parameter(created, "mode");
    (void)sqlite3_uri_boolean(created, "cache", 0);
    (void)sqlite3_uri_int64(created, "psow", 0);
  }

  if (sqlite3_open("./dummy_file", &db) == SQLITE_OK && db) {
    (void)sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    (void)sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
    (void)sqlite3_exec(db, "INSERT INTO t VALUES(1);", NULL, NULL, NULL);
    (void)sqlite3_exec(db, "ATTACH DATABASE ':memory:' AS aux;", NULL, NULL, NULL);

    dbfile_main = sqlite3_db_filename(db, "main");
    dbfile_temp = sqlite3_db_filename(db, "temp");
    dbfile_invalid = sqlite3_db_filename(db, "no_such_db");

    if (dbfile_main) {
      (void)sqlite3_filename_database(dbfile_main);
      (void)sqlite3_filename_journal(dbfile_main);
      (void)sqlite3_filename_wal(dbfile_main);
      (void)sqlite3_uri_parameter(dbfile_main, "mode");
      (void)sqlite3_uri_boolean(dbfile_main, "cache", 0);
      (void)sqlite3_uri_int64(dbfile_main, "psow", 0);
    }

    if (dbfile_temp && dbfile_temp[0] != '\0') {
      (void)sqlite3_filename_database(dbfile_temp);
      (void)sqlite3_filename_journal(dbfile_temp);
      (void)sqlite3_filename_wal(dbfile_temp);
    }

    if (dbfile_invalid) {
      (void)sqlite3_filename_database(dbfile_invalid);
      (void)sqlite3_filename_journal(dbfile_invalid);
      (void)sqlite3_filename_wal(dbfile_invalid);
    }

    (void)sqlite3_prepare_v2(db, "SELECT * FROM t;", -1, &stmt, NULL);
    if (stmt) {
      while (sqlite3_step(stmt) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  /* Intentionally avoid sqlite3_database_file_object() because calling it
     without a valid xOpen journal/WAL filename pointer from SQLite internals
     is undefined and may crash by API contract. */

cleanup:
  if (stmt) sqlite3_finalize(stmt);
  if (db) sqlite3_close(db);
  if (created) sqlite3_free_filename(created);
  if (params) {
    for (i = 0; i < nParam * 2; ++i) free(params[i]);
    free(params);
  }
  free(db_name);
  free(journal_name);
  free(wal_name);
  return 0;
}