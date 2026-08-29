// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int mode = 0;
  if (argc > 0) {
    mode = sqlite3_value_type(argv[0]);
  }
  switch (mode % 5) {
    case 0:
      sqlite3_result_null(ctx);
      break;
    case 1:
      if (argc > 0) {
        sqlite3_result_value(ctx, argv[0]);
      } else {
        sqlite3_result_int(ctx, 0);
      }
      break;
    case 2:
      sqlite3_result_int(ctx, argc);
      break;
    case 3: {
      sqlite3_int64 sum = 0;
      for (int i = 0; i < argc; ++i) {
        sum += sqlite3_value_bytes(argv[i]);
      }
      sqlite3_result_int64(ctx, sum);
      break;
    }
    default:
      sqlite3_result_text(ctx, "fuzz", -1, SQLITE_STATIC);
      break;
  }
}

static int fuzz_busy_handler(void *ptr, int count) {
  const uint8_t *data = (const uint8_t *)ptr;
  if (data == NULL) return 0;
  return (data[count & 7] & 1) ? 1 : 0;
}

static int fuzz_exec_callback(void *ptr, int argc, char **argv, char **colnames) {
  const uint8_t *data = (const uint8_t *)ptr;
  (void)argv;
  (void)colnames;
  if (data == NULL) return 0;
  return (argc > 0 && (data[0] & 1)) ? 1 : 0;
}

int LLVMFuzzerTestOneInput_29(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (Size > 0) {
    if (Data[0] & 1) flags |= SQLITE_OPEN_URI;
    if (Data[0] & 2) flags |= SQLITE_OPEN_MEMORY;
    if (Data[0] & 4) flags |= SQLITE_OPEN_NOMUTEX;
    if (Data[0] & 8) flags |= SQLITE_OPEN_FULLMUTEX;
    if (Data[0] & 16) flags |= SQLITE_OPEN_PRIVATECACHE;
    if (Data[0] & 32) flags |= SQLITE_OPEN_SHAREDCACHE;
  }

  const char *filename = "./dummy_file";
  if (flags & SQLITE_OPEN_MEMORY) {
    filename = ":memory:";
  }

  int rc = sqlite3_open_v2(filename, &db, flags, NULL);
  if (rc != SQLITE_OK && db == NULL) {
    return 0;
  }

  if (db != NULL) {
    sqlite3_close(db);
    db = NULL;
  }

  rc = sqlite3_open_v2(filename, &db, flags, NULL);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) sqlite3_close(db);
    return 0;
  }

  char funcname[64];
  size_t fnlen = 0;
  if (Size > 1) {
    fnlen = Data[1] % 32;
  }
  if (fnlen == 0) fnlen = 4;
  if (fnlen > sizeof(funcname) - 1) fnlen = sizeof(funcname) - 1;
  for (size_t i = 0; i < fnlen; ++i) {
    uint8_t b = (i + 2 < Size) ? Data[i + 2] : (uint8_t)('a' + (i % 26));
    funcname[i] = (char)('a' + (b % 26));
  }
  funcname[fnlen] = '\0';

  int nArg = -1;
  if (Size > 2) {
    uint8_t v = Data[2];
    if (v % 3 == 0) {
      nArg = -1;
    } else {
      nArg = v % 8;
    }
  }

  int eTextRep = SQLITE_UTF8;
  if (Size > 3) {
    switch (Data[3] % 4) {
      case 0: eTextRep = SQLITE_UTF8; break;
      case 1: eTextRep = SQLITE_UTF16LE; break;
      case 2: eTextRep = SQLITE_UTF16BE; break;
      default: eTextRep = SQLITE_UTF16; break;
    }
    if (Data[3] & 0x10) eTextRep |= SQLITE_DETERMINISTIC;
#ifdef SQLITE_DIRECTONLY
    if (Data[3] & 0x20) eTextRep |= SQLITE_DIRECTONLY;
#endif
  }

  sqlite3_create_function(db, funcname, nArg, eTextRep, NULL,
                          fuzz_scalar, NULL, NULL);

  sqlite3_busy_handler(db, fuzz_busy_handler, (void *)Data);

  char *sql = (char *)malloc(Size + 1);
  if (sql != NULL) {
    if (Size > 0) {
      memcpy(sql, Data, Size);
    }
    sql[Size] = '\0';

    char *errmsg = NULL;
    int use_callback = (Size > 4) ? (Data[4] & 1) : 0;
    sqlite3_exec(db, sql, use_callback ? fuzz_exec_callback : NULL,
                 (void *)Data, &errmsg);
    if (errmsg != NULL) {
      sqlite3_free(errmsg);
    }
    free(sql);
  } else {
    char *errmsg = NULL;
    sqlite3_exec(db, "", NULL, NULL, &errmsg);
    if (errmsg != NULL) {
      sqlite3_free(errmsg);
    }
  }

  sqlite3_close(db);
  return 0;
}