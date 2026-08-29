// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  (void)argv;
  (void)colv;
  if (ctx) {
    int *state = (int *)ctx;
    *state ^= argc;
    return (*state & 1);
  }
  return 0;
}

static int pick_dbstatus_op(uint8_t v) {
  static const int ops[] = {
#ifdef SQLITE_DBSTATUS_LOOKASIDE_USED
    SQLITE_DBSTATUS_LOOKASIDE_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED
    SQLITE_DBSTATUS_CACHE_USED,
#endif
#ifdef SQLITE_DBSTATUS_SCHEMA_USED
    SQLITE_DBSTATUS_SCHEMA_USED,
#endif
#ifdef SQLITE_DBSTATUS_STMT_USED
    SQLITE_DBSTATUS_STMT_USED,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_HIT
    SQLITE_DBSTATUS_LOOKASIDE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
    SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
    SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_HIT
    SQLITE_DBSTATUS_CACHE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_MISS
    SQLITE_DBSTATUS_CACHE_MISS,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_WRITE
    SQLITE_DBSTATUS_CACHE_WRITE,
#endif
#ifdef SQLITE_DBSTATUS_DEFERRED_FKS
    SQLITE_DBSTATUS_DEFERRED_FKS,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED_SHARED
    SQLITE_DBSTATUS_CACHE_USED_SHARED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_SPILL
    SQLITE_DBSTATUS_CACHE_SPILL,
#endif
  };
  size_t n = sizeof(ops) / sizeof(ops[0]);
  return n ? ops[v % n] : 0;
}

static int pick_status_op(uint8_t v) {
  static const int ops[] = {
#ifdef SQLITE_STATUS_MEMORY_USED
    SQLITE_STATUS_MEMORY_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_USED
    SQLITE_STATUS_PAGECACHE_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_OVERFLOW
    SQLITE_STATUS_PAGECACHE_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_SCRATCH_USED
    SQLITE_STATUS_SCRATCH_USED,
#endif
#ifdef SQLITE_STATUS_SCRATCH_OVERFLOW
    SQLITE_STATUS_SCRATCH_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_MALLOC_SIZE
    SQLITE_STATUS_MALLOC_SIZE,
#endif
#ifdef SQLITE_STATUS_PARSER_STACK
    SQLITE_STATUS_PARSER_STACK,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_SIZE
    SQLITE_STATUS_PAGECACHE_SIZE,
#endif
#ifdef SQLITE_STATUS_SCRATCH_SIZE
    SQLITE_STATUS_SCRATCH_SIZE,
#endif
#ifdef SQLITE_STATUS_MALLOC_COUNT
    SQLITE_STATUS_MALLOC_COUNT,
#endif
  };
  size_t n = sizeof(ops) / sizeof(ops[0]);
  return n ? ops[v % n] : 0;
}

static void free_errmsg(char **pErrmsg) {
  if (pErrmsg && *pErrmsg) {
    sqlite3_free(*pErrmsg);
    *pErrmsg = NULL;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlite3_shutdown();
#ifdef SQLITE_CONFIG_URI
  sqlite3_config(SQLITE_CONFIG_URI, (int)(Size ? (Data[0] & 1) : 0));
#else
  sqlite3_config(0);
#endif

  sqlite3 *db1 = NULL;
  (void)sqlite3_open(":memory:", &db1);

  (void)sqlite3_errmsg(db1);

  char *sql1 = sqlite3_mprintf("%.*s", (int)Size, (const char *)Data);
  char *errmsg1 = NULL;
  int cb_state = (Size > 1) ? Data[1] : 0;
  sqlite3_exec(db1,
               sql1 ? sql1 : "",
               (Size > 2 && (Data[2] & 1)) ? fuzz_callback : NULL,
               &cb_state,
               &errmsg1);
  sqlite3_free(errmsg1);

  int cur = 0, hi = 0;
  for (int i = 0; i < 10; ++i) {
    uint8_t b = (Size > (size_t)(3 + i)) ? Data[3 + i] : (uint8_t)i;
    sqlite3_db_status(db1, pick_dbstatus_op(b), &cur, &hi, b & 1);
  }

  sqlite3_close(db1);

  for (int i = 0; i < 5; ++i) {
    uint8_t b = (Size > (size_t)(13 + i)) ? Data[13 + i] : (uint8_t)i;
    int scur = 0, shi = 0;
    sqlite3_status(pick_status_op(b), &scur, &shi, b & 1);
  }

  char *fmt1 = sqlite3_mprintf("%.*q", (int)Size, (const char *)Data);
  sqlite3_free(fmt1);

  char *fmt2 = sqlite3_mprintf("file:%s?mode=%s&cache=%s",
                               "./dummy_file",
                               (Size > 18 && (Data[18] & 1)) ? "memory" : "ro",
                               (Size > 19 && (Data[19] & 1)) ? "shared" : "private");
  sqlite3_free(fmt2);

  sqlite3 *db2 = NULL;
  (void)sqlite3_open("./dummy_file", &db2);

  (void)sqlite3_errmsg(db2);

  char *errmsg2 = NULL;
  sqlite3_exec(db2, "PRAGMA journal_mode=OFF;", NULL, NULL, &errmsg2);
  free_errmsg(&errmsg2);
  sqlite3_exec(db2, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, &errmsg2);
  free_errmsg(&errmsg2);
  sqlite3_exec(db2,
               sql1 ? sql1 : "",
               (Size > 20 && (Data[20] & 1)) ? fuzz_callback : NULL,
               &cb_state,
               &errmsg2);
  free_errmsg(&errmsg2);

  sqlite3_close(db2);
  sqlite3_free(sql1);
  return 0;
}