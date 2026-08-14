// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t i, m = *n < 4 ? *n : 4;
  for (i = 0; i < m; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += m;
  *n -= m;
  return v;
}

static int read_bool(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int b = (**p) & 1;
  (*p)++;
  (*n)--;
  return b;
}

static char *read_cstring(const uint8_t **p, size_t *n, size_t maxlen) {
  size_t len;
  char *s;
  if (*n == 0) {
    s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }
  len = read_u32(p, n);
  if (maxlen > 0 && len > maxlen) len = maxlen;
  if (len > *n) len = *n;
  s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_124(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  char *dbName = NULL, *jrnlName = NULL, *walName = NULL;
  char *created = NULL;
  char **params = NULL;
  int nParam = 0;
  int i;
  int rc;
  const char *main_fn = NULL;
  const char *temp_fn = NULL;
  const char *db_from_created = NULL;
  const char *jrnl_from_created = NULL;
  const char *wal_from_created = NULL;
  const char *db_from_db = NULL;
  const char *jrnl_from_db = NULL;
  const char *wal_from_db = NULL;
  const char *db_from_temp = NULL;
  const char *jrnl_from_temp = NULL;
  const char *wal_from_temp = NULL;

  write_dummy_file(Data, Size);

  dbName = read_cstring(&p, &n, 256);
  jrnlName = read_cstring(&p, &n, 256);
  walName = read_cstring(&p, &n, 256);
  if (!dbName || !jrnlName || !walName) goto cleanup;

  if (dbName[0] == '\0') {
    free(dbName);
    dbName = (char *)malloc(sizeof("./dummy_file"));
    if (!dbName) goto cleanup;
    memcpy(dbName, "./dummy_file", sizeof("./dummy_file"));
  }

  nParam = (int)(read_u32(&p, &n) % 8);
  if (nParam > 0) {
    params = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!params) goto cleanup;
    for (i = 0; i < nParam * 2; i++) {
      params[i] = read_cstring(&p, &n, 64);
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
      dbName,
      jrnlName,
      walName,
      nParam,
      (const char **)params);

  if (created) {
    db_from_created = sqlite3_filename_database(created);
    jrnl_from_created = sqlite3_filename_journal(created);
    wal_from_created = sqlite3_filename_wal(created);

    if (read_bool(&p, &n)) {
      (void)sqlite3_uri_parameter(created, "mode");
      (void)sqlite3_uri_parameter(created, "cache");
      (void)sqlite3_uri_boolean(created, "immutable", 0);
      (void)sqlite3_uri_int64(created, "psow", 0);
      (void)sqlite3_uri_key(created, 0);
      (void)sqlite3_uri_key(created, 1);
    }

    if (db_from_created) {
      (void)sqlite3_uri_parameter(db_from_created, "v");
    }
  }

  rc = sqlite3_open_v2("./dummy_file", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (rc == SQLITE_OK && db) {
    char *errmsg = NULL;
    (void)sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    (void)sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    (void)sqlite3_exec(db, "INSERT INTO t VALUES(1);", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);

    main_fn = sqlite3_db_filename(db, "main");
    temp_fn = sqlite3_db_filename(db, "temp");
    (void)sqlite3_db_filename(db, "aux");
    (void)sqlite3_db_filename(db, "");

    if (main_fn) {
      db_from_db = sqlite3_filename_database(main_fn);
      jrnl_from_db = sqlite3_filename_journal(main_fn);
      wal_from_db = sqlite3_filename_wal(main_fn);

      (void)sqlite3_uri_parameter(main_fn, "mode");
      (void)sqlite3_uri_boolean(main_fn, "cache", 0);
      (void)sqlite3_uri_int64(main_fn, "nolock", 0);
      (void)sqlite3_uri_key(main_fn, 0);
    }

    if (temp_fn) {
      db_from_temp = sqlite3_filename_database(temp_fn);
      jrnl_from_temp = sqlite3_filename_journal(temp_fn);
      wal_from_temp = sqlite3_filename_wal(temp_fn);
      (void)db_from_temp;
      (void)jrnl_from_temp;
      (void)wal_from_temp;
    }

    (void)db_from_created;
    (void)jrnl_from_created;
    (void)wal_from_created;
    (void)db_from_db;
    (void)jrnl_from_db;
    (void)wal_from_db;
  }

cleanup:
  if (db) sqlite3_close(db);
  if (created) sqlite3_free_filename(created);
  if (params) {
    for (i = 0; i < nParam * 2; i++) free(params[i]);
    free(params);
  }
  free(dbName);
  free(jrnlName);
  free(walName);
  return 0;
}