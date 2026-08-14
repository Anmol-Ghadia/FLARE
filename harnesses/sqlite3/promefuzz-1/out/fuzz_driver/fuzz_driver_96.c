// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int rdbool(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = (**p) & 1;
  (*p)++;
  (*n)--;
  return v;
}

static int rdint(const uint8_t **p, size_t *n, int mod) {
  if (mod <= 0) return 0;
  return (int)(rd32(p, n) % (uint32_t)mod);
}

static char *rdstr(const uint8_t **p, size_t *n, size_t maxlen) {
  size_t len = 0;
  if (*n > 0) {
    len = (**p) % (maxlen + 1);
    (*p)++;
    (*n)--;
  }
  if (len > *n) len = *n;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  for (size_t i = 0; i < len; ++i) {
    unsigned char c = (*p)[i];
    if (c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static int fuzz_compare(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  int bias = pArg ? (*(unsigned char *)pArg % 3) - 1 : 0;
  int ncmp = n1 < n2 ? n1 : n2;
  for (int i = 0; i < ncmp; ++i) {
    if (a[i] != b[i]) return (int)a[i] - (int)b[i] + bias;
  }
  return (n1 - n2) + bias;
}

static void fuzz_destroy(void *pArg) {
  free(pArg);
}

static void fuzz_rollback(void *pArg) {
  volatile unsigned char *x = (volatile unsigned char *)pArg;
  if (x) (void)*x;
}

static void fuzz_coll_needed(void *pArg, sqlite3 *db, int eTextRep, const char *zName) {
  unsigned char *ctx = (unsigned char *)malloc(1);
  if (!ctx) return;
  *ctx = pArg ? *(unsigned char *)pArg : 0;
  sqlite3_create_collation_v2(db, zName ? zName : "auto_coll", eTextRep, ctx,
                              fuzz_compare, fuzz_destroy);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  unsigned char *rollback_arg = NULL;
  unsigned char *need_arg = NULL;
  char *maindbname = NULL;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_OPEN_URI
  if (rdbool(&p, &n)) flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
  if (rdbool(&p, &n)) flags |= SQLITE_OPEN_MEMORY;
#endif
#ifdef SQLITE_OPEN_NOMUTEX
  if (rdbool(&p, &n)) flags |= SQLITE_OPEN_NOMUTEX;
#endif
#ifdef SQLITE_OPEN_FULLMUTEX
  if (rdbool(&p, &n)) flags |= SQLITE_OPEN_FULLMUTEX;
#endif

  if (sqlite3_open_v2("./dummy_file", &db, flags, NULL) != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_extended_result_codes(db, rdbool(&p, &n));

  {
    int out = 0;
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_DEFENSIVE
    sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_DQS_DML
    sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DML, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_DQS_DDL
    sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DDL, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_QPSG
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_QPSG, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
    sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_WRITABLE_SCHEMA
    sqlite3_db_config(db, SQLITE_DBCONFIG_WRITABLE_SCHEMA, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_ALTER_TABLE
    sqlite3_db_config(db, SQLITE_DBCONFIG_LEGACY_ALTER_TABLE, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_FILE_FORMAT
    sqlite3_db_config(db, SQLITE_DBCONFIG_LEGACY_FILE_FORMAT, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE
    sqlite3_db_config(db, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_RESET_DATABASE
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, rdint(&p, &n, 3) - 1, &out);
#endif
#ifdef SQLITE_DBCONFIG_LOOKASIDE
    {
      int slot_sz = rdint(&p, &n, 256);
      int slot_cnt = rdint(&p, &n, 64);
      sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, NULL, slot_sz, slot_cnt);
    }
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
    maindbname = rdstr(&p, &n, 32);
    if (maindbname) {
      sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, maindbname);
    }
#endif
  }

  rollback_arg = (unsigned char *)malloc(1);
  if (rollback_arg) *rollback_arg = (unsigned char)rdint(&p, &n, 256);
  sqlite3_rollback_hook(db, fuzz_rollback, rollback_arg);

  need_arg = (unsigned char *)malloc(1);
  if (need_arg) *need_arg = (unsigned char)rdint(&p, &n, 256);
  sqlite3_collation_needed(db, need_arg, fuzz_coll_needed);

  for (int i = 0; i < 4; ++i) {
    char *name = rdstr(&p, &n, 24);
    if (!name) break;

    int encs[] = { SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE, SQLITE_UTF16 };
    int eTextRep = encs[rdint(&p, &n, (int)(sizeof(encs) / sizeof(encs[0])))];

    if (rdbool(&p, &n)) {
      unsigned char *ctx = (unsigned char *)malloc(1);
      if (ctx) *ctx = (unsigned char)rdint(&p, &n, 256);
      sqlite3_create_collation_v2(db, name, eTextRep, ctx, fuzz_compare, fuzz_destroy);
    } else {
      sqlite3_create_collation_v2(db, name, eTextRep, NULL, NULL, NULL);
    }
    free(name);
  }

  sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x TEXT, y INTEGER);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(x,y) VALUES('a',1),('b',2),('c',3);", NULL, NULL, NULL);

  char *sql = rdstr(&p, &n, 128);
  if (!sql) {
    sql = (char *)malloc(64);
    if (sql) strcpy(sql, "SELECT x FROM t ORDER BY x COLLATE missing_coll;");
  }

  if (sql) {
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK && stmt) {
      int steps = 0;
      while (steps++ < 8) {
        if (rdbool(&p, &n)) sqlite3_interrupt(db);
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW && rc != SQLITE_BUSY) break;
      }
    } else {
      sqlite3_exec(db, sql, NULL, NULL, NULL);
    }
    free(sql);
  }

  sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);

  if (stmt) sqlite3_finalize(stmt);

  sqlite3_rollback_hook(db, NULL, NULL);
  sqlite3_collation_needed(db, NULL, NULL);

  sqlite3_close(db);

  free(maindbname);
  free(rollback_arg);
  free(need_arg);
  return 0;
}