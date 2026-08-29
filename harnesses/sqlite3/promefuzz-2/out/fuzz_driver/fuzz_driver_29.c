// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static int read_u32(const uint8_t *data, size_t size, size_t *off, uint32_t *out) {
  if (*off + 4 > size) return 0;
  *out = ((uint32_t)data[*off]) |
         ((uint32_t)data[*off + 1] << 8) |
         ((uint32_t)data[*off + 2] << 16) |
         ((uint32_t)data[*off + 3] << 24);
  *off += 4;
  return 1;
}

static int read_i32(const uint8_t *data, size_t size, size_t *off, int32_t *out) {
  uint32_t v;
  if (!read_u32(data, size, off, &v)) return 0;
  *out = (int32_t)v;
  return 1;
}

static int busy_cb(void *ptr, int count) {
  const uint8_t *bytes = (const uint8_t *)ptr;
  if (bytes == NULL) return 0;
  return (bytes[count & 15] & 1) ? 1 : 0;
}

static int exec_cb(void *ptr, int argc, char **argv, char **colnames) {
  const uint8_t *bytes = (const uint8_t *)ptr;
  int acc = argc;
  int i;
  (void)argv;
  (void)colnames;
  if (bytes) acc ^= bytes[0];
  for (i = 0; i < argc; i++) acc ^= i;
  return acc & 1;
}

static void fuzz_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  sqlite3_int64 sum = 0;
  for (i = 0; i < argc; i++) {
    switch (sqlite3_value_type(argv[i])) {
      case SQLITE_INTEGER:
        sum += sqlite3_value_int64(argv[i]);
        break;
      case SQLITE_FLOAT:
        sum += (sqlite3_int64)sqlite3_value_double(argv[i]);
        break;
      case SQLITE_TEXT:
      case SQLITE_BLOB:
        sum += sqlite3_value_bytes(argv[i]);
        break;
      case SQLITE_NULL:
      default:
        break;
    }
  }
  sqlite3_result_int64(ctx, sum);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3 *db2 = NULL;
  char *errmsg = NULL;
  size_t off = 0;
  uint32_t flags_sel = 0;
  int32_t narg_raw = 0;
  uint32_t enc_sel = 0;
  uint32_t mode = 0;
  int rc;
  int flags;
  int nArg;
  int eTextRep;
  const char *vfs = NULL;
  const char *func_name = "fuzzfunc";
  const char *sql_ptr = NULL;
  size_t sql_len = 0;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  (void)read_u32(Data, Size, &off, &flags_sel);
  (void)read_i32(Data, Size, &off, &narg_raw);
  (void)read_u32(Data, Size, &off, &enc_sel);
  (void)read_u32(Data, Size, &off, &mode);

  flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (flags_sel & 1) flags |= SQLITE_OPEN_URI;
  if (flags_sel & 2) flags |= SQLITE_OPEN_MEMORY;
  if (flags_sel & 4) flags |= SQLITE_OPEN_NOMUTEX;
  if (flags_sel & 8) flags |= SQLITE_OPEN_FULLMUTEX;
#ifdef SQLITE_OPEN_SHAREDCACHE
  if (flags_sel & 16) flags |= SQLITE_OPEN_SHAREDCACHE;
#endif
#ifdef SQLITE_OPEN_PRIVATECACHE
  if (flags_sel & 32) flags |= SQLITE_OPEN_PRIVATECACHE;
#endif

  rc = sqlite3_open_v2("./dummy_file", &db, flags, vfs);
  if (db) {
    sqlite3_close(db);
    db = NULL;
  }

  rc = sqlite3_open_v2("./dummy_file", &db2, flags, vfs);
  if (rc != SQLITE_OK || db2 == NULL) {
    if (db2) sqlite3_close(db2);
    return 0;
  }

  if ((mode & 1) && off < Size) {
    size_t remain = Size - off;
    size_t name_len = remain > 255 ? 255 : remain;
    static char namebuf[256];
    memcpy(namebuf, Data + off, name_len);
    namebuf[name_len] = '\0';
    if (name_len > 0) func_name = namebuf;
    off += name_len;
  }

  nArg = (narg_raw % 129) - 1;
  if (nArg < -1) nArg = -1;
  if (nArg > 127) nArg = 127;

  switch (enc_sel % 4) {
    case 0:
      eTextRep = SQLITE_UTF8;
      break;
    case 1:
      eTextRep = SQLITE_UTF16LE;
      break;
    case 2:
      eTextRep = SQLITE_UTF16BE;
      break;
    default:
      eTextRep = SQLITE_UTF16;
      break;
  }
#ifdef SQLITE_DETERMINISTIC
  if (mode & 2) eTextRep |= SQLITE_DETERMINISTIC;
#endif
#ifdef SQLITE_DIRECTONLY
  if (mode & 4) eTextRep |= SQLITE_DIRECTONLY;
#endif

  sqlite3_create_function(db2, func_name, nArg, eTextRep, (void *)Data,
                          (mode & 8) ? NULL : fuzz_func,
                          NULL, NULL);

  sqlite3_busy_handler(db2, (mode & 16) ? NULL : busy_cb, (void *)Data);

  if (off < Size) {
    sql_ptr = (const char *)(Data + off);
    sql_len = Size - off;
  } else {
    sql_ptr = "";
    sql_len = 0;
  }

  {
    char *sql = (char *)sqlite3_malloc64(sql_len + 1);
    if (sql) {
      if (sql_len > 0) memcpy(sql, sql_ptr, sql_len);
      sql[sql_len] = '\0';
      sqlite3_exec(db2, sql, (mode & 32) ? exec_cb : NULL, (void *)Data, &errmsg);
      sqlite3_free(sql);
    } else {
      sqlite3_exec(db2, "", NULL, NULL, &errmsg);
    }
  }

  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  sqlite3_close(db2);
  return 0;
}