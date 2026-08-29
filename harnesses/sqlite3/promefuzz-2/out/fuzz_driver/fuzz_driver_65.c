// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_unregister at sqlite3.c:11505:16 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_bool(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = (**p) & 1;
  (*p)++;
  (*n)--;
  return v;
}

static int pick_from_flags(uint32_t selector) {
  static const int flags[] = {
    SQLITE_OPEN_READONLY,
    SQLITE_OPEN_READWRITE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE
  };
  return flags[selector % (sizeof(flags) / sizeof(flags[0]))];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  char vfs_name[64];
  size_t name_len;
  sqlite3_vfs *default_vfs = NULL;
  sqlite3_vfs *found_vfs = NULL;
  sqlite3 *db = NULL;
  int flags;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  (void)sqlite3_initialize();

  default_vfs = sqlite3_vfs_find(NULL);

  name_len = n < sizeof(vfs_name) - 1 ? n : sizeof(vfs_name) - 1;
  if (name_len > 0) {
    memcpy(vfs_name, p, name_len);
  }
  vfs_name[name_len] = '\0';

  found_vfs = sqlite3_vfs_find(read_bool(&p, &n) ? NULL : vfs_name);

  if (default_vfs != NULL) {
    int make_default = read_bool(&p, &n);
    (void)sqlite3_vfs_register(default_vfs, make_default);
    if (default_vfs->zName != NULL) {
      (void)sqlite3_vfs_find(default_vfs->zName);
    }
    (void)sqlite3_vfs_unregister(default_vfs);
    (void)sqlite3_vfs_register(default_vfs, 1);
  }

#ifdef SQLITE_TESTCTRL_PRNG_RESET
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
#endif

#ifdef SQLITE_TESTCTRL_PENDING_BYTE
  {
    unsigned int pending = read_u32(&p, &n);
    (void)sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE,
                               (int)(pending & 0x7fffffff));
  }
#endif

  flags = pick_from_flags(read_u32(&p, &n));

  if (found_vfs != NULL && found_vfs->zName != NULL) {
    (void)sqlite3_open_v2("./dummy_file", &db, flags, found_vfs->zName);
    if (db != NULL) {
      sqlite3_close(db);
      db = NULL;
    }
  }

  (void)sqlite3_open_v2("./dummy_file", &db, flags, NULL);
  if (db != NULL) {
    sqlite3_close(db);
    db = NULL;
  }

  if (default_vfs != NULL && default_vfs->zName != NULL) {
    (void)sqlite3_open_v2(":memory:", &db, flags, default_vfs->zName);
    if (db != NULL) {
      sqlite3_close(db);
      db = NULL;
    }
  }

  return 0;
}