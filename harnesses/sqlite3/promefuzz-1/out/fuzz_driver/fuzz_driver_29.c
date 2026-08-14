// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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
  int i;
  sqlite3_int64 sum = 0;
  for (i = 0; i < argc; i++) {
    sum += sqlite3_value_bytes(argv[i]);
    sum += sqlite3_value_type(argv[i]);
  }
  sqlite3_result_int64(ctx, sum);
}

static int fuzz_busy_handler(void *ptr, int count) {
  const uint8_t *data = (const uint8_t *)ptr;
  if (data == NULL) return 0;
  return (data[count & 15] & 1) ? 1 : 0;
}

static int fuzz_exec_callback(void *ptr, int argc, char **argv, char **colnames) {
  const uint8_t *data = (const uint8_t *)ptr;
  int i;
  int acc = argc;
  for (i = 0; i < argc; i++) {
    if (argv && argv[i]) acc += (int)(unsigned char)argv[i][0];
    if (colnames && colnames[i]) acc += (int)(unsigned char)colnames[i][0];
  }
  if (data == NULL) return 0;
  return (data[0] ^ acc) & 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *sql = NULL;
  char *errmsg = NULL;
  char *funcname = NULL;
  FILE *fp;
  int rc;
  int flags;
  int nArg;
  int enc;
  const char *vfs = NULL;
  const char *filename = "./dummy_file";

  fp = fopen(filename, "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (Size > 0) {
    if (Data[0] & 0x01) flags |= SQLITE_OPEN_URI;
    if (Data[0] & 0x02) flags |= SQLITE_OPEN_MEMORY;
    if (Data[0] & 0x04) flags |= SQLITE_OPEN_NOMUTEX;
    if (Data[0] & 0x08) flags |= SQLITE_OPEN_FULLMUTEX;
    if (Data[0] & 0x10) flags |= SQLITE_OPEN_PRIVATECACHE;
    if (Data[0] & 0x20) flags |= SQLITE_OPEN_SHAREDCACHE;
  }

  if (Size > 1 && (Data[1] & 1)) {
    vfs = "unix";
  }

  rc = sqlite3_open_v2(filename, &db, flags, vfs);

  if (db != NULL) {
    sqlite3_close(db);
    db = NULL;
  }

  rc = sqlite3_open_v2(filename, &db, flags, vfs);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) {
      sqlite3_close(db);
    }
    return 0;
  }

  funcname = (char *)malloc(256);
  if (funcname == NULL) {
    sqlite3_close(db);
    return 0;
  }
  if (Size > 2) {
    size_t fnlen = Size - 2;
    if (fnlen > 255) fnlen = 255;
    memcpy(funcname, Data + 2, fnlen);
    funcname[fnlen] = '\0';
    if (fnlen == 0) {
      strcpy(funcname, "f");
    }
  } else {
    strcpy(funcname, "f");
  }

  nArg = -1;
  if (Size > 3) {
    nArg = (int)(Data[2] % 6) - 1;
  }

  enc = SQLITE_UTF8;
  if (Size > 4) {
    switch (Data[3] % 4) {
      case 0: enc = SQLITE_UTF8; break;
      case 1: enc = SQLITE_UTF16LE; break;
      case 2: enc = SQLITE_UTF16BE; break;
      default: enc = SQLITE_UTF16; break;
    }
    if (Data[3] & 0x40) enc |= SQLITE_DETERMINISTIC;
    if (Data[3] & 0x80) enc |= SQLITE_DIRECTONLY;
  }

  sqlite3_create_function(db, funcname, nArg, enc, (void *)Data, fuzz_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzzsum", -1, SQLITE_UTF8, (void *)Data, fuzz_scalar, NULL, NULL);

  if (Size > 5 && (Data[4] & 1)) {
    sqlite3_busy_handler(db, NULL, NULL);
  } else {
    sqlite3_busy_handler(db, fuzz_busy_handler, (void *)Data);
  }

  sql = (char *)malloc(Size + 1);
  if (sql != NULL) {
    if (Size > 0) {
      memcpy(sql, Data, Size);
    }
    sql[Size] = '\0';

    if (Size > 6 && (Data[5] & 1)) {
      sqlite3_exec(db, sql, fuzz_exec_callback, (void *)Data, &errmsg);
    } else {
      sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    }

    if (errmsg != NULL) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    free(sql);
  }

  free(funcname);
  sqlite3_close(db);
  return 0;
}