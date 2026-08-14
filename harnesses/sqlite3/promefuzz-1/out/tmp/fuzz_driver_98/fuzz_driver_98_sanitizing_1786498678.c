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
  if (*size == 0) {
    char *s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }
  size_t len = (*data)[0] % (*size + 1);
  *data += 1;
  *size -= 1;
  if (len > *size) len = *size;
  char *s = (char *)malloc(len + 1);
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

  write_dummy_file(Data, Size);

  int rc_input = read_int(&p, &remaining);
  const char *errstr1 = sqlite3_errstr(rc_input);
  (void)errstr1;
  const char *errstr2 = sqlite3_errstr(rc_input | 0xFF);
  (void)errstr2;

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_OPEN_URI
  if (remaining && (p[0] & 1)) flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
  if (remaining && (p[0] & 2)) flags |= SQLITE_OPEN_MEMORY;
#endif

  int open_rc = sqlite3_open_v2("./dummy_file", &db, flags, NULL);

  const char *null_msg = sqlite3_errmsg(NULL);
  (void)null_msg;
  int null_code = sqlite3_errcode(NULL);
  (void)null_code;

  if (open_rc == SQLITE_OK && db != NULL) {
    int enable = 1;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, enable, NULL);

    sqlite3_vfs *vfs = sqlite3_db_vfs(db);
    if (vfs && vfs->xDlOpen && vfs->xDlSym && vfs->xDlClose) {
      char *zFile = read_string(&p, &remaining);
      char *zProc = read_string(&p, &remaining);
      char *pzErrMsg = NULL;

      const char *file_to_use = (zFile && zFile[0]) ? zFile : "./dummy_file";
      const char *proc_to_use = NULL;
      if (zProc && zProc[0]) {
        proc_to_use = zProc;
      } else if (remaining && (p[0] & 1)) {
        proc_to_use = NULL;
      }

      int load_rc = sqlite3_load_extension(db, file_to_use, proc_to_use, &pzErrMsg);
      (void)load_rc;

      const char *msg = sqlite3_errmsg(db);
      (void)msg;
      int code = sqlite3_errcode(db);
      (void)code;
      const char *mapped = sqlite3_errstr(code);
      (void)mapped;

      if (pzErrMsg) {
        sqlite3_free(pzErrMsg);
      }
      free(zFile);
      free(zProc);
    }

    int ops[] = {
#ifdef SQLITE_DBSTATUS_LOOKASIDE_USED
      SQLITE_DBSTATUS_LOOKASIDE_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED
      SQLITE_DBSTATUS_CACHE_USED,
#endif
#ifdef SQLITE_DBSTATUS_SCHEMA_USED
      SQLITE_DBSTATUS_SCHEMA_USED,
#endif
#ifdef SQLITE_DBSTATUS_STMT_USED
      SQLITE_DBSTATUS_STMT_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_HIT
      SQLITE_DBSTATUS_CACHE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_MISS
      SQLITE_DBSTATUS_CACHE_MISS,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_WRITE
      SQLITE_DBSTATUS_CACHE_WRITE,
#endif
#ifdef SQLITE_DBSTATUS_DEFERRED_FKS
      SQLITE_DBSTATUS_DEFERRED_FKS,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_HIT
      SQLITE_DBSTATUS_LOOKASIDE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
      SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
      SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED_SHARED
      SQLITE_DBSTATUS_CACHE_USED_SHARED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_SPILL
      SQLITE_DBSTATUS_CACHE_SPILL,
#endif
    };

    size_t nops = sizeof(ops) / sizeof(ops[0]);
    for (size_t i = 0; i < nops; ++i) {
      int cur = 0, hi = 0;
      int reset = (remaining && (p[0] & 1)) ? 1 : 0;
      sqlite3_db_status(db, ops[i], &cur, &hi, reset);
      sqlite3_log(sqlite3_errcode(db), "db_status op=%d cur=%d hi=%d", ops[i], cur, hi);
    }

    int cur = 0, hi = 0;
    sqlite3_db_status(db, read_int(&p, &remaining), &cur, &hi, remaining ? (p[0] & 1) : 0);
    sqlite3_log(rc_input, "fuzz rc=%d open_rc=%d err=%s", rc_input, open_rc, sqlite3_errmsg(db));
  } else {
    sqlite3_log(open_rc, "open failed: %s / %s",
                sqlite3_errstr(open_rc),
                db ? sqlite3_errmsg(db) : sqlite3_errmsg(NULL));
  }

  if (db) {
    const char *final_msg = sqlite3_errmsg(db);
    (void)final_msg;
    int final_code = sqlite3_errcode(db);
    (void)final_code;
    sqlite3_close(db);
  }

  return 0;
}