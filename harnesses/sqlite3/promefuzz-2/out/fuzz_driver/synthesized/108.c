// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_global_recover at sqlite3.c:155846:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

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

static int read_i32(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static const char *choose_dbname(uint32_t v) {
  switch (v % 4) {
    case 0: return "main";
    case 1: return "temp";
    case 2: return NULL;
    default: return "nosuchdb";
  }
}

int LLVMFuzzerTestOneInput_108(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  FILE *fp = NULL;
  void *mem1 = NULL;
  void *mem2 = NULL;
  int rc;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    fp = NULL;
  }

  rc = sqlite3_initialize();
  (void)rc;

  (void)sqlite3_global_recover();

  {
    int sz1 = (read_i32(&p, &n) & 0xFFFF) - 0x100;
    int sz2 = (read_i32(&p, &n) & 0x7FFF);

    mem1 = sqlite3_malloc(sz1);
    if (mem1) {
      memset(mem1, 0xA5, (size_t)(sz1 > 0 ? sz1 : 0));
    }

    mem2 = sqlite3_malloc(sz2);
    if (mem2) {
      size_t copy = n < (size_t)sz2 ? n : (size_t)sz2;
      if (copy > 0) {
        memcpy(mem2, p, copy);
      }
    }
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc == SQLITE_OK && db != NULL) {
    char *errmsg = NULL;
    const char *sqls[] = {
      "PRAGMA journal_mode=OFF;",
      "PRAGMA synchronous=OFF;",
      "CREATE TABLE IF NOT EXISTS t(a,b);",
      "INSERT INTO t(a,b) VALUES(1,'x');",
      "CREATE TEMP TABLE IF NOT EXISTS tt(x);",
      "SELECT * FROM t;",
      "VACUUM;"
    };
    size_t count = sizeof(sqls) / sizeof(sqls[0]);
    uint32_t rounds = (n > 0 ? p[0] : 0) % 8 + 1;
    for (uint32_t i = 0; i < rounds; i++) {
      const char *sql = sqls[(n > i ? p[i] : i) % count];
      (void)sqlite3_exec(db, sql, NULL, NULL, &errmsg);
      if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
      }
    }

    {
      void *arg = NULL;
      sqlite3_int64 i64arg = 0;
      int intarg = 0;
      int opcodes[] = {
#ifdef SQLITE_FCNTL_FILE_POINTER
        SQLITE_FCNTL_FILE_POINTER,
#endif
#ifdef SQLITE_FCNTL_JOURNAL_POINTER
        SQLITE_FCNTL_JOURNAL_POINTER,
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        SQLITE_FCNTL_VFS_POINTER,
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
        SQLITE_FCNTL_DATA_VERSION,
#endif
      };
      size_t opcount = sizeof(opcodes) / sizeof(opcodes[0]);

      if (opcount > 0) {
        const char *zDbName = choose_dbname(read_u32(&p, &n));
        int op = opcodes[read_u32(&p, &n) % opcount];

#ifdef SQLITE_FCNTL_DATA_VERSION
        if (op == SQLITE_FCNTL_DATA_VERSION) {
          (void)sqlite3_file_control(db, zDbName, op, &intarg);
        } else
#endif
#ifdef SQLITE_FCNTL_FILE_POINTER
        if (op == SQLITE_FCNTL_FILE_POINTER) {
          (void)sqlite3_file_control(db, zDbName, op, &arg);
        } else
#endif
#ifdef SQLITE_FCNTL_JOURNAL_POINTER
        if (op == SQLITE_FCNTL_JOURNAL_POINTER) {
          (void)sqlite3_file_control(db, zDbName, op, &arg);
        } else
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        if (op == SQLITE_FCNTL_VFS_POINTER) {
          (void)sqlite3_file_control(db, zDbName, op, &arg);
        } else
#endif
        {
          (void)sqlite3_file_control(db, zDbName, op, &i64arg);
        }
      }

      (void)sqlite3_file_control(db, "nosuchdb", 0, &arg);
    }
  }

  {
    int rel = read_i32(&p, &n);
    (void)sqlite3_release_memory(rel);
    (void)sqlite3_release_memory(0);
    (void)sqlite3_release_memory(1024);
  }

#ifdef SQLITE_TESTCTRL_PRNG_RESET
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
#endif
#ifdef SQLITE_TESTCTRL_BYTEORDER
  (void)sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
#endif
#ifdef SQLITE_TESTCTRL_ISINIT
  (void)sqlite3_test_control(SQLITE_TESTCTRL_ISINIT);
#endif
#ifdef SQLITE_TESTCTRL_ASSERT
  (void)sqlite3_test_control(SQLITE_TESTCTRL_ASSERT, 1);
#endif

  if (db) {
    sqlite3_close(db);
    db = NULL;
  }

  sqlite3_free(mem1);
  sqlite3_free(mem2);

  return 0;
}