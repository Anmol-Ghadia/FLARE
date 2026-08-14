#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "sqlite3.h"

static int progress_cb(void *pArg) {
  const uint8_t *p = (const uint8_t *)pArg;
  return p ? (p[0] & 1) : 0;
}

static int pick_int(const uint8_t *Data, size_t Size, size_t *Off, int defval) {
  int v = defval;
  if (*Off + 4 <= Size) {
    v = (int)(
        ((unsigned int)Data[*Off]) |
        ((unsigned int)Data[*Off + 1] << 8) |
        ((unsigned int)Data[*Off + 2] << 16) |
        ((unsigned int)Data[*Off + 3] << 24));
    *Off += 4;
  } else if (*Off < Size) {
    v = (int)Data[(*Off)++];
  }
  return v;
}

static sqlite3_int64 pick_i64(const uint8_t *Data, size_t Size, size_t *Off, sqlite3_int64 defval) {
  sqlite3_int64 v = defval;
  if (*Off + 8 <= Size) {
    v = (sqlite3_int64)(
        ((sqlite3_uint64)Data[*Off]) |
        ((sqlite3_uint64)Data[*Off + 1] << 8) |
        ((sqlite3_uint64)Data[*Off + 2] << 16) |
        ((sqlite3_uint64)Data[*Off + 3] << 24) |
        ((sqlite3_uint64)Data[*Off + 4] << 32) |
        ((sqlite3_uint64)Data[*Off + 5] << 40) |
        ((sqlite3_uint64)Data[*Off + 6] << 48) |
        ((sqlite3_uint64)Data[*Off + 7] << 56));
    *Off += 8;
  } else if (*Off < Size) {
    v = (sqlite3_int64)(signed char)Data[(*Off)++];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  if (sqlite3_initialize() != SQLITE_OK) {
    return 0;
  }

  {
    int flag_choices[] = {
      SQLITE_OPEN_READONLY,
      SQLITE_OPEN_READWRITE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX
    };
    int flags = flag_choices[(Size ? Data[off % Size] : 0) % (sizeof(flag_choices) / sizeof(flag_choices[0]))];
    const char *filename = (Size > 0 && (Data[off % Size] & 1)) ? ":memory:" : "./dummy_file";
    const char *zVfs = NULL;
    if (Size > 1 && (Data[(off + 1) % Size] & 1)) {
      zVfs = "unix";
    }

    sqlite3_open_v2(filename, &db, flags, zVfs);
  }

  if (db != NULL) {
    int nOps = pick_int(Data, Size, &off, 0);
    sqlite3_progress_handler(db, nOps, progress_cb, (void *)Data);

    {
      int limit_ids[] = {
#ifdef SQLITE_LIMIT_LENGTH
        SQLITE_LIMIT_LENGTH,
#endif
#ifdef SQLITE_LIMIT_SQL_LENGTH
        SQLITE_LIMIT_SQL_LENGTH,
#endif
#ifdef SQLITE_LIMIT_COLUMN
        SQLITE_LIMIT_COLUMN,
#endif
#ifdef SQLITE_LIMIT_EXPR_DEPTH
        SQLITE_LIMIT_EXPR_DEPTH,
#endif
#ifdef SQLITE_LIMIT_COMPOUND_SELECT
        SQLITE_LIMIT_COMPOUND_SELECT,
#endif
#ifdef SQLITE_LIMIT_VDBE_OP
        SQLITE_LIMIT_VDBE_OP,
#endif
#ifdef SQLITE_LIMIT_FUNCTION_ARG
        SQLITE_LIMIT_FUNCTION_ARG,
#endif
#ifdef SQLITE_LIMIT_ATTACHED
        SQLITE_LIMIT_ATTACHED,
#endif
#ifdef SQLITE_LIMIT_LIKE_PATTERN_LENGTH
        SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
#endif
#ifdef SQLITE_LIMIT_VARIABLE_NUMBER
        SQLITE_LIMIT_VARIABLE_NUMBER,
#endif
#ifdef SQLITE_LIMIT_TRIGGER_DEPTH
        SQLITE_LIMIT_TRIGGER_DEPTH,
#endif
#ifdef SQLITE_LIMIT_WORKER_THREADS
        SQLITE_LIMIT_WORKER_THREADS,
#endif
      };
      int id1 = limit_ids[(Size ? Data[off % Size] : 0) % (sizeof(limit_ids) / sizeof(limit_ids[0]))];
      int newVal1 = pick_int(Data, Size, &off, -1);
      sqlite3_limit(db, id1, newVal1);
    }

    {
      sqlite3_int64 heap_limit = pick_i64(Data, Size, &off, -1);
      sqlite3_hard_heap_limit64(heap_limit);
    }

    {
      int limit_ids[] = {
#ifdef SQLITE_LIMIT_LENGTH
        SQLITE_LIMIT_LENGTH,
#endif
#ifdef SQLITE_LIMIT_SQL_LENGTH
        SQLITE_LIMIT_SQL_LENGTH,
#endif
#ifdef SQLITE_LIMIT_COLUMN
        SQLITE_LIMIT_COLUMN,
#endif
#ifdef SQLITE_LIMIT_EXPR_DEPTH
        SQLITE_LIMIT_EXPR_DEPTH,
#endif
#ifdef SQLITE_LIMIT_COMPOUND_SELECT
        SQLITE_LIMIT_COMPOUND_SELECT,
#endif
#ifdef SQLITE_LIMIT_VDBE_OP
        SQLITE_LIMIT_VDBE_OP,
#endif
#ifdef SQLITE_LIMIT_FUNCTION_ARG
        SQLITE_LIMIT_FUNCTION_ARG,
#endif
#ifdef SQLITE_LIMIT_ATTACHED
        SQLITE_LIMIT_ATTACHED,
#endif
#ifdef SQLITE_LIMIT_LIKE_PATTERN_LENGTH
        SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
#endif
#ifdef SQLITE_LIMIT_VARIABLE_NUMBER
        SQLITE_LIMIT_VARIABLE_NUMBER,
#endif
#ifdef SQLITE_LIMIT_TRIGGER_DEPTH
        SQLITE_LIMIT_TRIGGER_DEPTH,
#endif
#ifdef SQLITE_LIMIT_WORKER_THREADS
        SQLITE_LIMIT_WORKER_THREADS,
#endif
      };
      int id2 = limit_ids[(Size ? Data[off % Size] : 0) % (sizeof(limit_ids) / sizeof(limit_ids[0]))];
      int newVal2 = pick_int(Data, Size, &off, -1);
      sqlite3_limit(db, id2, newVal2);
    }

    {
      int choice = Size ? Data[off % Size] % 6 : 0;
      off += (Size ? 1 : 0);
      switch (choice) {
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
        case 0: {
          int onoff = pick_int(Data, Size, &off, -1);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
          break;
        }
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
        case 1: {
          int onoff = pick_int(Data, Size, &off, -1);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, onoff, &out);
          break;
        }
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
        case 2: {
          int onoff = pick_int(Data, Size, &off, -1);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, onoff, &out);
          break;
        }
#endif
#ifdef SQLITE_DBCONFIG_LOOKASIDE
        case 3: {
          static unsigned char lookaside_buf[512];
          int sz = pick_int(Data, Size, &off, 64);
          int cnt = pick_int(Data, Size, &off, 4);
          void *buf = (Size > 2 && (Data[(off + 2) % Size] & 1)) ? (void *)lookaside_buf : NULL;
          sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, buf, sz, cnt);
          break;
        }
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
        case 4: {
          char namebuf[32];
          size_t remain = Size > off ? Size - off : 0;
          size_t n = remain < sizeof(namebuf) - 1 ? remain : sizeof(namebuf) - 1;
          if (n > 0) {
            memcpy(namebuf, Data + off, n);
          }
          namebuf[n] = '\0';
          sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, n ? namebuf : "main");
          break;
        }
#endif
        default: {
#ifdef SQLITE_DBCONFIG_ENABLE_QPSG
          int onoff = pick_int(Data, Size, &off, -1);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_QPSG, onoff, &out);
#elif defined(SQLITE_DBCONFIG_DEFENSIVE)
          int onoff = pick_int(Data, Size, &off, -1);
          int out = 0;
          sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, onoff, &out);
#else
          sqlite3_db_config(db, 0);
#endif
          break;
        }
      }
    }

    sqlite3_close(db);
  }

  return 0;
}