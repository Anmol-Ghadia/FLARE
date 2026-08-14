#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

static int consume_u32(const uint8_t **data, size_t *size) {
  int v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int choose_dbstatus_op(int selector) {
#ifdef SQLITE_DBSTATUS_LOOKASIDE_USED
  static const int ops[] = {
    SQLITE_DBSTATUS_LOOKASIDE_USED,
    SQLITE_DBSTATUS_CACHE_USED,
    SQLITE_DBSTATUS_SCHEMA_USED,
    SQLITE_DBSTATUS_STMT_USED,
#ifdef SQLITE_DBSTATUS_LOOKASIDE_HIT
    SQLITE_DBSTATUS_LOOKASIDE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
    SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
    SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL,
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
#ifdef SQLITE_DBSTATUS_CACHE_USED_SHARED
    SQLITE_DBSTATUS_CACHE_USED_SHARED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_SPILL
    SQLITE_DBSTATUS_CACHE_SPILL,
#endif
  };
  return ops[(unsigned)selector % (sizeof(ops) / sizeof(ops[0]))];
#else
  (void)selector;
  return 0;
#endif
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  int rc;
  const uint8_t *p = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  const void *filename16 = NULL;
  unsigned char *utf16name = NULL;

  if (remaining > 0 && (p[0] & 1) == 0) {
    filename16 = NULL;
    p++;
    remaining--;
  } else {
    static const char path[] = "./dummy_file";
    size_t path_len = sizeof(path);
    utf16name = (unsigned char *)malloc(path_len * 2);
    if (utf16name != NULL) {
      for (size_t i = 0; i < path_len; ++i) {
        utf16name[2 * i] = (unsigned char)path[i];
        utf16name[2 * i + 1] = 0;
      }
      filename16 = utf16name;
    } else {
      filename16 = NULL;
    }
  }

  rc = sqlite3_open16(filename16, &db);

  if (db != NULL) {
    int n1 = remaining ? (int)((int8_t)*p++) : 0;
    if (remaining) remaining--;
    (void)sqlite3_wal_autocheckpoint(db, n1);

    int ac1 = sqlite3_get_autocommit(db);
    (void)ac1;

    int err1 = sqlite3_errcode(db);
    (void)err1;

    const char *zDb = NULL;
    if (remaining > 0) {
      switch (*p++ % 4) {
        case 0: zDb = NULL; break;
        case 1: zDb = "main"; break;
        case 2: zDb = "temp"; break;
        default: zDb = ""; break;
      }
      remaining--;
    }
    (void)sqlite3_wal_checkpoint(db, zDb);

    for (int i = 0; i < 4 && remaining > 0; ++i) {
      int cur = 0, hi = 0;
      int op = choose_dbstatus_op(consume_u32(&p, &remaining));
      int reset = remaining ? (*p++ & 1) : 0;
      if (remaining) remaining--;
      (void)sqlite3_db_status(db, op, &cur, &hi, reset);
    }

    if (remaining > 0) {
      int n2 = (int)((int8_t)*p++);
      remaining--;
      (void)sqlite3_wal_autocheckpoint(db, n2);
    }

    (void)sqlite3_get_autocommit(db);
    (void)sqlite3_errcode(db);
    (void)sqlite3_wal_checkpoint(db, "main");
    (void)sqlite3_wal_checkpoint(db, NULL);

    sqlite3_close(db);
  } else {
    (void)rc;
  }

  free(utf16name);
  remove("./dummy_file");
  return 0;
}