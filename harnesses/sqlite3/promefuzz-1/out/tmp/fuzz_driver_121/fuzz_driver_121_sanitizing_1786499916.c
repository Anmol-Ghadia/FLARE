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
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static char *consume_cstring(const uint8_t **p, size_t *n, size_t max_len) {
  if (*n == 0) {
    char *s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }
  size_t len = read_u32(p, n);
  if (max_len > 0 && len > max_len) len = max_len;
  if (len > *n) len = *n;

  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;

  for (size_t i = 0; i < len; i++) {
    unsigned char c = (*p)[i];
    if (c == 0) c = 'A';
    s[i] = (char)c;
  }
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  write_dummy_file(Data, Size);

  char *db_name = consume_cstring(&p, &n, 256);
  char *jrnl_name = consume_cstring(&p, &n, 256);
  char *wal_name = consume_cstring(&p, &n, 256);
  if (!db_name || !jrnl_name || !wal_name) {
    free(db_name);
    free(jrnl_name);
    free(wal_name);
    return 0;
  }

  int nParam = 0;
  if (n > 0) {
    nParam = (int)(read_u32(&p, &n) % 8);
  }

  const char **azParam = NULL;
  char **owned = NULL;
  if (nParam > 0) {
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(char *));
    owned = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!azParam || !owned) {
      free((void *)azParam);
      free(owned);
      free(db_name);
      free(jrnl_name);
      free(wal_name);
      return 0;
    }
    for (int i = 0; i < nParam * 2; i += 2) {
      owned[i] = consume_cstring(&p, &n, 64);
      owned[i + 1] = consume_cstring(&p, &n, 128);
      if (!owned[i] || !owned[i + 1]) {
        for (int j = 0; j < nParam * 2; j++) free(owned[j]);
        free(owned);
        free((void *)azParam);
        free(db_name);
        free(jrnl_name);
        free(wal_name);
        return 0;
      }
      if (owned[i][0] == '\0') {
        free(owned[i]);
        owned[i] = (char *)malloc(2);
        if (!owned[i]) {
          for (int j = 0; j < nParam * 2; j++) free(owned[j]);
          free(owned);
          free((void *)azParam);
          free(db_name);
          free(jrnl_name);
          free(wal_name);
          return 0;
        }
        owned[i][0] = 'k';
        owned[i][1] = '\0';
      }
      azParam[i] = owned[i];
      azParam[i + 1] = owned[i + 1];
    }
  }

  char *fname = sqlite3_create_filename(db_name, jrnl_name, wal_name, nParam, azParam);
  if (fname) {
    const char *dbp = sqlite3_filename_database(fname);
    const char *jrp = sqlite3_filename_journal(fname);
    const char *wlp = sqlite3_filename_wal(fname);

    if (dbp) (void)sqlite3_uri_parameter(dbp, "k");
    if (fname) (void)sqlite3_uri_parameter(fname, "mode");
    if (fname) (void)sqlite3_uri_boolean(fname, "cache", 0);
    if (fname) (void)sqlite3_uri_int64(fname, "psow", 0);

    (void)dbp;
    (void)jrp;
    (void)wlp;

    sqlite3_free_filename(fname);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (Size > 0) {
    if (Data[0] & 1) flags |= SQLITE_OPEN_URI;
    if (Data[0] & 2) flags |= SQLITE_OPEN_MEMORY;
  }

  int rc = sqlite3_open_v2("./dummy_file", &db, flags, NULL);
  if (rc == SQLITE_OK && db) {
    char *errmsg = NULL;
    (void)sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    errmsg = NULL;
    (void)sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);

    const char *main_fn = sqlite3_db_filename(db, "main");
    const char *temp_fn = sqlite3_db_filename(db, "temp");
    const char *bogus_fn = sqlite3_db_filename(db, "bogus");

    if (main_fn) {
      const char *db2 = sqlite3_filename_database(main_fn);
      const char *jr2 = sqlite3_filename_journal(main_fn);
      const char *wl2 = sqlite3_filename_wal(main_fn);
      if (db2) (void)sqlite3_uri_parameter(db2, "mode");
      if (jr2) (void)jr2;
      if (wl2) (void)wl2;
    }
    if (temp_fn && temp_fn[0]) {
      const char *db3 = sqlite3_filename_database(temp_fn);
      const char *jr3 = sqlite3_filename_journal(temp_fn);
      const char *wl3 = sqlite3_filename_wal(temp_fn);
      (void)db3;
      (void)jr3;
      (void)wl3;
    }
    if (bogus_fn && bogus_fn[0]) {
      const char *db4 = sqlite3_filename_database(bogus_fn);
      const char *jr4 = sqlite3_filename_journal(bogus_fn);
      const char *wl4 = sqlite3_filename_wal(bogus_fn);
      (void)db4;
      (void)jr4;
      (void)wl4;
    }

    sqlite3_close(db);
  } else if (db) {
    sqlite3_close(db);
  }

  for (int i = 0; i < nParam * 2; i++) free(owned ? owned[i] : NULL);
  free(owned);
  free((void *)azParam);
  free(db_name);
  free(jrnl_name);
  free(wal_name);
  return 0;
}