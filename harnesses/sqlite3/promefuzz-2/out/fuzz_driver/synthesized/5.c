// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
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

#ifndef SQLITE_TESTCTRL_PRNG_RESET
#define SQLITE_TESTCTRL_PRNG_RESET 7
#endif

#ifndef SQLITE_TESTCTRL_PENDING_BYTE
#define SQLITE_TESTCTRL_PENDING_BYTE 11
#endif

static int progress_cb(void *pArg) {
  const uint8_t *bytes = (const uint8_t *)pArg;
  if (!bytes) return 0;
  return (bytes[0] & 1) ? 1 : 0;
}

static int exec_cb(void *unused, int argc, char **argv, char **colv) {
  (void)unused;
  (void)argc;
  (void)argv;
  (void)colv;
  return 0;
}

static int read_u32(const uint8_t *data, size_t size, size_t *off) {
  int v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < size; ++i, ++(*off)) {
    v = (v << 8) | data[*off];
  }
  return v;
}

int LLVMFuzzerTestOneInput_5(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  int rc;
  size_t off = 0;
  int limit_id1, limit_id2;
  int limit_val1, limit_val2;
  int progress_ops;
  int pending_byte;
  const char *ver;
  const char *sid;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    sqlite3_memory_used();
    sqlite3_memory_used();
    sqlite3_libversion();
    sqlite3_sourceid();
    return 0;
  }

  limit_id1 = (read_u32(Data, Size, &off) % 12);
  limit_val1 = (int)(read_u32(Data, Size, &off) % 100000) - 50000;
  (void)sqlite3_limit(db, limit_id1, limit_val1);

  limit_id2 = (read_u32(Data, Size, &off) % 12);
  limit_val2 = (Data && Size && (Data[0] & 2)) ? -1 : ((int)(read_u32(Data, Size, &off) % 100000) - 50000);
  (void)sqlite3_limit(db, limit_id2, limit_val2);

  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);

  progress_ops = ((int)(read_u32(Data, Size, &off) % 1000)) - 100;
  sqlite3_progress_handler(db, progress_ops, progress_cb, (void *)Data);

  pending_byte = (int)(read_u32(Data, Size, &off) & 0x7fffffff);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, pending_byte);

  {
    size_t sql1_len = 0;
    size_t sql2_len = 0;
    char *sql1;
    char *sql2;

    if (off < Size) {
      sql1_len = (Size - off) / 2;
      sql2_len = Size - off - sql1_len;
    }

    sql1 = (char *)malloc(sql1_len + 1);
    sql2 = (char *)malloc(sql2_len + 1);
    if (!sql1 || !sql2) {
      free(sql1);
      free(sql2);
      sqlite3_close(db);
      sqlite3_memory_used();
      sqlite3_memory_used();
      sqlite3_libversion();
      sqlite3_sourceid();
      return 0;
    }

    if (sql1_len) memcpy(sql1, Data + off, sql1_len);
    sql1[sql1_len] = '\0';
    off += sql1_len;

    if (sql2_len) memcpy(sql2, Data + off, sql2_len);
    sql2[sql2_len] = '\0';

    rc = sqlite3_exec(
        db,
        sql1,
        (Data && Size && (Data[0] & 4)) ? exec_cb : NULL,
        NULL,
        &errmsg1);
    if (errmsg1) {
      sqlite3_free(errmsg1);
      errmsg1 = NULL;
    }

    rc = sqlite3_exec(
        db,
        sql2,
        (Data && Size && (Data[0] & 8)) ? exec_cb : NULL,
        NULL,
        &errmsg2);
    if (errmsg2) {
      sqlite3_free(errmsg2);
      errmsg2 = NULL;
    }

    free(sql1);
    free(sql2);
  }

  sqlite3_close(db);
  sqlite3_memory_used();
  sqlite3_memory_used();
  ver = sqlite3_libversion();
  sid = sqlite3_sourceid();
  (void)ver;
  (void)sid;

  return 0;
}