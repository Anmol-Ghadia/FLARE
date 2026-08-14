#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static int progress_cb(void *pArg) {
  const uint8_t *bytes = (const uint8_t *)pArg;
  if (!bytes) return 0;
  return (bytes[0] & 1) ? 1 : 0;
}

static int exec_cb(void *pArg, int argc, char **argv, char **colv) {
  const uint8_t *bytes = (const uint8_t *)pArg;
  volatile int sink = argc;
  if (argv) sink ^= (argv[0] != NULL);
  if (colv) sink ^= (colv[0] != NULL);
  (void)sink;
  if (!bytes) return 0;
  return (bytes[0] & 2) ? 1 : 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  FILE *fp;
  int rc;
  int limit_id1, limit_id2;
  int new_limit1, new_limit2;
  int nOps;
  int tc_rc1, tc_rc2;
  sqlite3_int64 mem1, mem2;
  const char *ver;
  const char *sid;
  char *sqlbuf = NULL;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
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

  sqlbuf = (char *)malloc(Size + 1);
  if (!sqlbuf) {
    sqlite3_close(db);
    return 0;
  }
  if (Size) memcpy(sqlbuf, Data, Size);
  sqlbuf[Size] = '\0';

  limit_id1 = Size > 0 ? (Data[0] % 12) : 0;
  new_limit1 = Size > 1 ? ((int)(int8_t)Data[1]) : -1;
  sqlite3_limit(db, limit_id1, new_limit1);

  limit_id2 = Size > 2 ? (Data[2] % 12) : 0;
  new_limit2 = Size > 6 ? (int)((uint32_t)Data[3] |
                                ((uint32_t)Data[4] << 8) |
                                ((uint32_t)Data[5] << 16) |
                                ((uint32_t)Data[6] << 24))
                        : -1;
  sqlite3_limit(db, limit_id2, new_limit2);

  tc_rc1 = sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
  (void)tc_rc1;

  nOps = Size > 7 ? ((Data[7] % 16) + 1) : 1;
  sqlite3_progress_handler(db, nOps, progress_cb, (void *)Data);

  tc_rc2 = sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, 0x40000000);
  (void)tc_rc2;

  sqlite3_exec(db, sqlbuf, exec_cb, (void *)Data, &errmsg1);
  if (errmsg1) {
    sqlite3_free(errmsg1);
    errmsg1 = NULL;
  }

  sqlite3_exec(db, sqlbuf, NULL, NULL, &errmsg2);
  if (errmsg2) {
    sqlite3_free(errmsg2);
    errmsg2 = NULL;
  }

  free(sqlbuf);
  sqlbuf = NULL;

  sqlite3_close(db);
  db = NULL;

  mem1 = sqlite3_memory_used();
  mem2 = sqlite3_memory_used();
  ver = sqlite3_libversion();
  sid = sqlite3_sourceid();

  (void)mem1;
  (void)mem2;
  (void)ver;
  (void)sid;

  return 0;
}