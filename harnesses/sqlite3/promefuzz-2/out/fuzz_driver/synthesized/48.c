// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_keyword_name at sqlite3.c:151127:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_keyword_count at sqlite3.c:151133:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_count at sqlite3.c:151133:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static unsigned int read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  unsigned int v = 0;
  size_t i;
  for (i = 0; i < 4; ++i) {
    v <<= 8;
    if (*Off < Size) {
      v |= Data[*Off];
      (*Off)++;
    }
  }
  return v;
}

static int read_int(const uint8_t *Data, size_t Size, size_t *Off) {
  return (int)read_u32(Data, Size, Off);
}

static unsigned int choose_flags(unsigned int raw) {
  unsigned int flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
  if (raw & 1u) flags |= SQLITE_PREPARE_PERSISTENT;
#endif
#ifdef SQLITE_PREPARE_NORMALIZE
  if (raw & 2u) flags |= SQLITE_PREPARE_NORMALIZE;
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
  if (raw & 4u) flags |= SQLITE_PREPARE_NO_VTAB;
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
  if (raw & 8u) flags |= SQLITE_PREPARE_DONT_LOG;
#endif
  return flags;
}

static void exec_and_finalize(sqlite3_stmt *stmt) {
  if (stmt) {
    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
  }
}

int LLVMFuzzerTestOneInput_48(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const void *tail16 = NULL;
  size_t off = 0;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_exec(
      db,
      "PRAGMA foreign_keys=OFF;"
      "CREATE TABLE IF NOT EXISTS t(a,b,c);"
      "CREATE TABLE IF NOT EXISTS \"select\"(x,y);"
      "INSERT INTO t(a,b,c) VALUES(1,'x',zeroblob(4));",
      NULL, NULL, NULL);

  (void)sqlite3_keyword_count();

  if (Size > 0) {
    int n1 = read_int(Data, Size, &off);
    if (n1 < 0) n1 = -n1;
    if ((size_t)n1 > Size) n1 = (int)Size;
    (void)sqlite3_keyword_check((const char *)Data, n1);
  } else {
    (void)sqlite3_keyword_check("", 0);
  }

  {
    int kwcount = sqlite3_keyword_count();
    int idx = 0;
    if (kwcount > 0) {
      idx = read_int(Data, Size, &off);
      if (idx < 0) idx = -idx;
      idx %= (kwcount + 1);
    }
    for (i = 0; i < 3; ++i) {
      const char *z = NULL;
      int n = 0;
      int probe = idx + i - 1;
      (void)sqlite3_keyword_name(probe, &z, &n);
      if (z != NULL && n >= 0) {
        (void)sqlite3_keyword_check(z, n);
      }
    }
  }

  if (Size > 0) {
    int nByte = read_int(Data, Size, &off);
    if (nByte < 0) nByte = -nByte;
    if ((size_t)nByte > Size) nByte = (int)Size;

    stmt = NULL;
    tail16 = NULL;
    rc = sqlite3_prepare16(db, Data, nByte, &stmt, &tail16);
    (void)rc;
    exec_and_finalize(stmt);
    stmt = NULL;

    stmt = NULL;
    tail16 = NULL;
    rc = sqlite3_prepare16_v2(db, Data, nByte, &stmt, &tail16);
    (void)rc;
    exec_and_finalize(stmt);
    stmt = NULL;

    stmt = NULL;
    tail16 = NULL;
    rc = sqlite3_prepare16_v3(db, Data, nByte,
                              choose_flags(read_u32(Data, Size, &off)),
                              &stmt, &tail16);
    (void)rc;
    exec_and_finalize(stmt);
    stmt = NULL;
  }

  if (Size >= 2) {
    size_t remain = Size - off;
    const uint8_t *p = Data + off;
    int nByte = (int)(remain & ~((size_t)1));

    stmt = NULL;
    tail16 = NULL;
    rc = sqlite3_prepare16(db, p, nByte, &stmt, &tail16);
    (void)rc;
    exec_and_finalize(stmt);
    stmt = NULL;

    stmt = NULL;
    tail16 = NULL;
    rc = sqlite3_prepare16_v2(db, p, nByte, &stmt, &tail16);
    (void)rc;
    exec_and_finalize(stmt);
    stmt = NULL;

    stmt = NULL;
    tail16 = NULL;
    rc = sqlite3_prepare16_v3(db, p, nByte,
                              choose_flags(read_u32(Data, Size, &off)),
                              &stmt, &tail16);
    (void)rc;
    exec_and_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}