// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_complete16 at sqlite3.c:152128:16 in sqlite3.h
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

static int fuzz_callback(void *ctx, int argc, char **argv, char **colnames) {
  const uint8_t *data = (const uint8_t *)ctx;
  int decision = 0;

  if (data) {
    decision = data[0] & 1;
  }

  (void)argc;
  (void)argv;
  (void)colnames;
  return decision;
}

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *p3, const char *p4) {
  const uint8_t *data = (const uint8_t *)pUserData;
  int mode = 0;

  if (data) {
    mode = data[0] % 4;
  }

  (void)action;
  (void)p1;
  (void)p2;
  (void)p3;
  (void)p4;

  switch (mode) {
    case 0:
      return SQLITE_OK;
    case 1:
      return SQLITE_IGNORE;
    case 2:
      return SQLITE_DENY;
    default:
      return 999;
  }
}

int LLVMFuzzerTestOneInput_115(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *sql = NULL;
  char *errmsg = NULL;
  void *utf16buf = NULL;
  FILE *fp = NULL;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    fp = NULL;
  }

  sql = (char *)malloc(Size + 1);
  if (!sql) {
    return 0;
  }
  if (Size > 0) {
    memcpy(sql, Data, Size);
  }
  sql[Size] = '\0';

  {
    size_t u16_units = Size / 2;
    size_t u16_bytes = (u16_units + 1) * 2;
    utf16buf = malloc(u16_bytes);
    if (utf16buf) {
      if (u16_units > 0) {
        memcpy(utf16buf, Data, u16_units * 2);
      }
      ((unsigned char *)utf16buf)[u16_units * 2] = 0;
      ((unsigned char *)utf16buf)[u16_units * 2 + 1] = 0;
      (void)sqlite3_complete16(utf16buf);
    }
  }

  {
    int flags_options[] = {
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
      SQLITE_OPEN_READONLY,
      SQLITE_OPEN_READWRITE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY
    };
    int flags = flags_options[(Size > 0 ? Data[0] : 0) % (sizeof(flags_options) / sizeof(flags_options[0]))];
    const char *filename = "./dummy_file";
    const char *vfs = NULL;

    if (Size > 1 && (Data[1] & 1)) {
      filename = ":memory:";
    }
    if (Size > 2 && (Data[2] & 1)) {
      vfs = NULL;
    }

    (void)sqlite3_open_v2(filename, &db, flags, vfs);
  }

  if (db) {
    int limit_ids[] = {
      SQLITE_LIMIT_LENGTH,
      SQLITE_LIMIT_SQL_LENGTH,
      SQLITE_LIMIT_COLUMN,
      SQLITE_LIMIT_EXPR_DEPTH,
      SQLITE_LIMIT_COMPOUND_SELECT,
      SQLITE_LIMIT_VDBE_OP,
      SQLITE_LIMIT_FUNCTION_ARG,
      SQLITE_LIMIT_ATTACHED,
      SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
      SQLITE_LIMIT_VARIABLE_NUMBER,
      SQLITE_LIMIT_TRIGGER_DEPTH,
      SQLITE_LIMIT_WORKER_THREADS
    };
    size_t nlimits = sizeof(limit_ids) / sizeof(limit_ids[0]);
    size_t i;

    for (i = 0; i < 3 && i < nlimits; i++) {
      int id = limit_ids[(Size > i ? Data[i] : 0) % nlimits];
      int newVal;
      if (Size > (i + 3)) {
        newVal = (int)Data[i + 3] - 1;
      } else {
        newVal = -1;
      }
      (void)sqlite3_limit(db, id, newVal);
    }

    if (Size > 0) {
      (void)sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
    } else {
      (void)sqlite3_set_authorizer(db, NULL, NULL);
    }

    (void)sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    (void)sqlite3_exec(db, sql, fuzz_callback, (void *)Data, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    (void)sqlite3_set_authorizer(db, NULL, NULL);
    (void)sqlite3_exec(db, sql, fuzz_callback, (void *)Data, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    sqlite3_close(db);
    db = NULL;
  }

  free(utf16buf);
  free(sql);

  (void)sqlite3_shutdown();
  return 0;
}