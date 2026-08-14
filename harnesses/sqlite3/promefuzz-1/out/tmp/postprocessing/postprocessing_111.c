#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static size_t fuzz_strnlen(const char *s, size_t maxlen) {
  size_t i;
  for (i = 0; i < maxlen; i++) {
    if (s[i] == '\0') break;
  }
  return i;
}

static char *make_cstring(const uint8_t *data, size_t size, size_t *consumed) {
  size_t len = 0;
  char *out;

  while (len < size && data[len] != 0) len++;
  out = (char *)malloc(len + 1);
  if (!out) {
    *consumed = size;
    return NULL;
  }
  if (len) memcpy(out, data, len);
  out[len] = '\0';
  *consumed = (len < size) ? (len + 1) : len;
  return out;
}

static int write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
  return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0, used = 0;
  char *db = NULL, *jrnl = NULL, *wal = NULL;
  char *param_storage[8] = {0};
  const char *azParam[8] = {0};
  int nParam = 0;
  char *fname = NULL;
  const char *p1, *p2, *p3, *p4, *p5;

  (void)write_dummy_file(Data, Size);

  db = make_cstring(Data + off, Size - off, &used);
  off += used;
  jrnl = make_cstring(Data + off, Size - off, &used);
  off += used;
  wal = make_cstring(Data + off, Size - off, &used);
  off += used;

  if (!db) {
    db = (char *)malloc(13);
    if (db) memcpy(db, "./dummy_file", 13);
  }
  if (!jrnl) {
    jrnl = (char *)malloc(21);
    if (jrnl) memcpy(jrnl, "./dummy_file-journal", 21);
  }
  if (!wal) {
    wal = (char *)malloc(17);
    if (wal) memcpy(wal, "./dummy_file-wal", 17);
  }

  if (!db || !jrnl || !wal) goto cleanup;

  if (db[0] == '\0') {
    free(db);
    db = (char *)malloc(13);
    if (!db) goto cleanup;
    memcpy(db, "./dummy_file", 13);
  }
  if (jrnl[0] == '\0') {
    free(jrnl);
    jrnl = (char *)malloc(21);
    if (!jrnl) goto cleanup;
    memcpy(jrnl, "./dummy_file-journal", 21);
  }
  if (wal[0] == '\0') {
    free(wal);
    wal = (char *)malloc(17);
    if (!wal) goto cleanup;
    memcpy(wal, "./dummy_file-wal", 17);
  }

  for (nParam = 0; nParam < 4 && off < Size; nParam++) {
    param_storage[2 * nParam] = make_cstring(Data + off, Size - off, &used);
    off += used;
    param_storage[2 * nParam + 1] = make_cstring(Data + off, Size - off, &used);
    off += used;

    if (!param_storage[2 * nParam] || !param_storage[2 * nParam + 1]) {
      if (param_storage[2 * nParam]) {
        free(param_storage[2 * nParam]);
        param_storage[2 * nParam] = NULL;
      }
      if (param_storage[2 * nParam + 1]) {
        free(param_storage[2 * nParam + 1]);
        param_storage[2 * nParam + 1] = NULL;
      }
      break;
    }

    if (param_storage[2 * nParam][0] == '\0') {
      free(param_storage[2 * nParam]);
      param_storage[2 * nParam] = (char *)malloc(2);
      if (!param_storage[2 * nParam]) break;
      memcpy(param_storage[2 * nParam], "k", 2);
    }

    azParam[2 * nParam] = param_storage[2 * nParam];
    azParam[2 * nParam + 1] = param_storage[2 * nParam + 1];
  }

  fname = sqlite3_create_filename(db, jrnl, wal, nParam, nParam ? azParam : NULL);
  if (fname) {
    p1 = sqlite3_filename_database(fname);
    p2 = sqlite3_filename_journal(fname);
    p3 = sqlite3_filename_wal(fname);

    if (p1) {
      volatile size_t l1 = fuzz_strnlen(p1, 4096);
      (void)l1;
    }
    if (p2) {
      volatile size_t l2 = fuzz_strnlen(p2, 4096);
      (void)l2;
    }
    if (p3) {
      volatile size_t l3 = fuzz_strnlen(p3, 4096);
      (void)l3;
    }

    if (nParam > 0) {
      p4 = sqlite3_uri_parameter(fname, azParam[0]);
      if (p4) {
        volatile size_t l4 = fuzz_strnlen(p4, 4096);
        (void)l4;
      }
    }

    p5 = sqlite3_uri_parameter(fname, "mode");
    if (p5) {
      volatile size_t l5 = fuzz_strnlen(p5, 4096);
      (void)l5;
    }

    (void)sqlite3_uri_parameter(fname, "");
    (void)sqlite3_uri_parameter(fname, NULL);

    if (p2) {
      (void)sqlite3_uri_parameter(p2, "cache");
    }
    if (p3) {
      (void)sqlite3_uri_parameter(p3, "vfs");
    }

    /* sqlite3_database_file_object() must only be called on a valid
       database filename object with attached pager metadata. The strings
       returned by sqlite3_filename_journal()/wal() do not satisfy that
       contract, so avoid calling it on p2/p3. */
    sqlite3_free_filename(fname);
  }

cleanup:
  free(db);
  free(jrnl);
  free(wal);
  for (int i = 0; i < 8; i++) {
    free(param_storage[i]);
  }
  return 0;
}