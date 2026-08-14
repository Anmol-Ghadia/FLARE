#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_int(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static char *read_string(const uint8_t **data, size_t *size) {
  size_t len;
  char *s;

  if (*size == 0) {
    s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }

  len = (*data)[0] % (*size + 1);
  *data += 1;
  *size -= 1;
  if (len > *size) len = *size;

  s = (char *)malloc(len + 1);
  if (!s) return NULL;
  memcpy(s, *data, len);
  s[len] = '\0';
  *data += len;
  *size -= len;
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
  size_t remaining = Size;
  sqlite3 *db = NULL;
  int open_rc;
  int rc_input;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  write_dummy_file(Data, Size);

  rc_input = read_int(&p, &remaining);
  (void)sqlite3_errstr(rc_input);
  (void)sqlite3_errstr(rc_input | 0xFF);

#ifdef SQLITE_OPEN_URI
  if (remaining && (p[0] & 1)) flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
  if (remaining && (p[0] & 2)) flags |= SQLITE_OPEN_MEMORY;
#endif

  (void)sqlite3_errmsg(NULL);
  (void)sqlite3_errcode(NULL);

  open_rc = sqlite3_open_v2("./dummy_file", &db, flags, NULL);

  if (open_rc == SQLITE_OK && db != NULL) {
    char *zFile;
    char *zProc;
    char *pzErrMsg = NULL;
    int cur = 0, hi = 0;
    int reset = 0;
    int load_rc;
    int op;

    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL);

    zFile = read_string(&p, &remaining);
    zProc = read_string(&p, &remaining);

    load_rc = sqlite3_load_extension(
        db,
        (zFile && zFile[0]) ? zFile : "./dummy_file",
        (zProc && zProc[0]) ? zProc : NULL,
        &pzErrMsg);
    (void)load_rc;

    (void)sqlite3_errmsg(db);
    (void)sqlite3_errcode(db);
    (void)sqlite3_errstr(sqlite3_errcode(db));

    if (pzErrMsg) {
      sqlite3_free(pzErrMsg);
      pzErrMsg = NULL;
    }

    free(zFile);
    free(zProc);

#ifdef SQLITE_DBSTATUS_LOOKASIDE_USED
    sqlite3_db_status(db, SQLITE_DBSTATUS_LOOKASIDE_USED, &cur, &hi, 0);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_LOOKASIDE_USED, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED
    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_USED, &cur, &hi, 0);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_CACHE_USED, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_SCHEMA_USED
    sqlite3_db_status(db, SQLITE_DBSTATUS_SCHEMA_USED, &cur, &hi, 0);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_SCHEMA_USED, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_STMT_USED
    sqlite3_db_status(db, SQLITE_DBSTATUS_STMT_USED, &cur, &hi, 0);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_STMT_USED, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_CACHE_HIT
    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_HIT, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_CACHE_HIT, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_CACHE_MISS
    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_MISS, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_CACHE_MISS, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_CACHE_WRITE
    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_WRITE, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_CACHE_WRITE, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_DEFERRED_FKS
    sqlite3_db_status(db, SQLITE_DBSTATUS_DEFERRED_FKS, &cur, &hi, 0);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_DEFERRED_FKS, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_HIT
    sqlite3_db_status(db, SQLITE_DBSTATUS_LOOKASIDE_HIT, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_LOOKASIDE_HIT, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
    sqlite3_db_status(db, SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
    sqlite3_db_status(db, SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED_SHARED
    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_USED_SHARED, &cur, &hi, 0);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_CACHE_USED_SHARED, cur, hi);
#endif
#ifdef SQLITE_DBSTATUS_CACHE_SPILL
    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_SPILL, &cur, &hi, 1);
    sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d",
                SQLITE_DBSTATUS_CACHE_SPILL, cur, hi);
#endif

    op = read_int(&p, &remaining);
    reset = (remaining && (p[0] & 1)) ? 1 : 0;
    sqlite3_db_status(db, op, &cur, &hi, reset);

    sqlite3_log(rc_input, "fuzz rc=%d open_rc=%d err=%s",
                rc_input, open_rc, sqlite3_errmsg(db));
  } else {
    sqlite3_log(open_rc, "open failed: %s / %s",
                sqlite3_errstr(open_rc),
                db ? sqlite3_errmsg(db) : sqlite3_errmsg(NULL));
  }

  if (db) {
    (void)sqlite3_errmsg(db);
    (void)sqlite3_errcode(db);
    sqlite3_close(db);
  }

  return 0;
}