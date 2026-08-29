// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
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
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_trace_v2 at sqlite3.c:154389:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static int trace_cb(unsigned t, void *ctx, void *p, void *x) {
  (void)t;
  (void)ctx;
  (void)p;
  (void)x;
  return 0;
}

static unsigned get_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  unsigned v = 0;
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

static int get_int(const uint8_t *Data, size_t Size, size_t *Off) {
  return (int)get_u32(Data, Size, Off);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  size_t off = 0;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      sqlite3_errcode(db);
      sqlite3_close(db);
    }
    return 0;
  }

  sqlite3_extended_result_codes(db, get_int(Data, Size, &off) & 1);

  {
    unsigned mask_choices[] = {
      0,
      SQLITE_TRACE_STMT,
      SQLITE_TRACE_PROFILE,
      SQLITE_TRACE_ROW,
      SQLITE_TRACE_CLOSE,
      SQLITE_TRACE_STMT | SQLITE_TRACE_PROFILE,
      SQLITE_TRACE_ROW | SQLITE_TRACE_CLOSE,
      SQLITE_TRACE_STMT | SQLITE_TRACE_ROW | SQLITE_TRACE_PROFILE | SQLITE_TRACE_CLOSE
    };
    unsigned mask = mask_choices[get_u32(Data, Size, &off) % (sizeof(mask_choices) / sizeof(mask_choices[0]))];
    sqlite3_trace_v2(db, mask, trace_cb, db);
    if (get_int(Data, Size, &off) & 1) {
      sqlite3_trace_v2(db, 0, NULL, NULL);
      sqlite3_trace_v2(db, mask, trace_cb, Data ? (void *)Data : NULL);
    }
  }

  {
    int nframe = get_int(Data, Size, &off);
    sqlite3_wal_autocheckpoint(db, nframe);
    if (get_int(Data, Size, &off) & 1) {
      sqlite3_wal_autocheckpoint(db, 0);
    }
  }

  {
    int enable = get_int(Data, Size, &off) & 1;
    int out = 0;
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_QPSG
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_QPSG, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_DEFENSIVE
    sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_WRITABLE_SCHEMA
    sqlite3_db_config(db, SQLITE_DBCONFIG_WRITABLE_SCHEMA, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_ALTER_TABLE
    sqlite3_db_config(db, SQLITE_DBCONFIG_LEGACY_ALTER_TABLE, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_DQS_DML
    sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DML, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_DQS_DDL
    sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DDL, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
    sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE
    sqlite3_db_config(db, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_RESET_DATABASE
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, enable, &out);
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
    {
      const char *name = (Size > off) ? (const char *)(Data + off) : "main";
      sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, name);
    }
#endif
#ifdef SQLITE_DBCONFIG_LOOKASIDE
    {
      static unsigned char lookaside_buf[512];
      int sz = (get_int(Data, Size, &off) & 0x7f) + 1;
      int cnt = get_int(Data, Size, &off) & 0xf;
      sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, lookaside_buf, sz, cnt);
    }
#endif
  }

  sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
  sqlite3_exec(db, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(x) VALUES(1);", NULL, NULL, NULL);
  sqlite3_db_cacheflush(db);

  if (Size > off) {
    const char *sql = (const char *)(Data + off);
    sqlite3_prepare_v2(db, sql, (int)(Size - off), &stmt, NULL);
    if (stmt) {
      int step_count = 0;
      while (step_count < 8) {
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
        step_count++;
      }
    }
    sqlite3_errcode(db);
  }

  if (stmt) {
    sqlite3_finalize(stmt);
  }

  sqlite3_db_cacheflush(db);
  sqlite3_errcode(db);

#ifdef SQLITE_TESTCTRL_PENDING_BYTE
  sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, (int)(get_u32(Data, Size, &off) & 0xffff));
#endif
#ifdef SQLITE_TESTCTRL_ASSERT
  sqlite3_test_control(SQLITE_TESTCTRL_ASSERT, 1);
#endif
#ifdef SQLITE_TESTCTRL_ALWAYS
  sqlite3_test_control(SQLITE_TESTCTRL_ALWAYS, get_int(Data, Size, &off) & 1);
#endif
#ifdef SQLITE_TESTCTRL_BYTEORDER
  sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
#endif
#ifdef SQLITE_TESTCTRL_IMPOSTER
  sqlite3_test_control(SQLITE_TESTCTRL_IMPOSTER, db, "main", 0, 0);
#endif
#ifdef SQLITE_TESTCTRL_FAULT_INSTALL
  sqlite3_test_control(SQLITE_TESTCTRL_FAULT_INSTALL, NULL);
#endif

  sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
  sqlite3_trace_v2(db, 0, NULL, NULL);
  sqlite3_close(db);
  return 0;
}