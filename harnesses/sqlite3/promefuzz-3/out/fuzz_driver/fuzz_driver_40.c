// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static int read_u8(const uint8_t *Data, size_t Size, size_t *Off) {
  if (*Off >= Size) return 0;
  return Data[(*Off)++];
}

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; i++) {
    v <<= 8;
    if (*Off < Size) v |= Data[(*Off)++];
  }
  return v;
}

static int pick_int(const uint8_t *Data, size_t Size, size_t *Off, int minv, int maxv) {
  if (minv >= maxv) return minv;
  return minv + (int)(read_u32(Data, Size, Off) % (uint32_t)(maxv - minv + 1));
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &err);
  if (err) sqlite3_free(err);
}

static void bind_from_fuzz(sqlite3_stmt *stmt, const uint8_t *Data, size_t Size, size_t *Off) {
  int n = sqlite3_bind_parameter_count(stmt);
  for (int i = 1; i <= n; i++) {
    int mode = read_u8(Data, Size, Off) % 6;
    switch (mode) {
      case 0:
        sqlite3_bind_null(stmt, i);
        break;
      case 1:
        sqlite3_bind_int(stmt, i, (int)read_u32(Data, Size, Off));
        break;
      case 2:
        sqlite3_bind_int64(stmt, i, (sqlite3_int64)read_u32(Data, Size, Off));
        break;
      case 3: {
        double d = (double)(int32_t)read_u32(Data, Size, Off) / 17.0;
        sqlite3_bind_double(stmt, i, d);
        break;
      }
      case 4: {
        int len = pick_int(Data, Size, Off, 0, 32);
        if (*Off + (size_t)len > Size) len = (int)(Size - *Off);
        sqlite3_bind_text(stmt, i, (const char *)(Data + *Off), len, SQLITE_TRANSIENT);
        *Off += (size_t)len;
        break;
      }
      case 5: {
        int len = pick_int(Data, Size, Off, 0, 32);
        if (*Off + (size_t)len > Size) len = (int)(Size - *Off);
        sqlite3_bind_blob(stmt, i, Data + *Off, len, SQLITE_TRANSIENT);
        *Off += (size_t)len;
        break;
      }
    }
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  size_t off = 0;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA temp_store=MEMORY;");
  exec_sql(db, "CREATE TABLE t1(a INTEGER, b TEXT, c BLOB);");
  exec_sql(db, "CREATE TABLE t2(x INTEGER, y TEXT);");
  exec_sql(db, "CREATE INDEX i1 ON t1(a);");
  exec_sql(db, "INSERT INTO t1 VALUES(1,'alpha',x'0102');");
  exec_sql(db, "INSERT INTO t1 VALUES(2,'beta',x'0304');");
  exec_sql(db, "INSERT INTO t1 VALUES(3,'gamma',x'0506');");
  exec_sql(db, "INSERT INTO t2 VALUES(1,'one');");
  exec_sql(db, "INSERT INTO t2 VALUES(2,'two');");
  exec_sql(db, "INSERT INTO t2 VALUES(3,'three');");

  {
    static const char *sqls[] = {
      "SELECT a,b,c FROM t1;",
      "SELECT a,b FROM t1 WHERE a=?1;",
      "SELECT t1.a,t2.y FROM t1 JOIN t2 ON t1.a=t2.x WHERE t1.a>?1;",
      "UPDATE t1 SET b=?1 WHERE a=?2;",
      "DELETE FROM t1 WHERE a=?1;",
      "INSERT INTO t1(a,b,c) VALUES(?1,?2,?3);",
      "SELECT count(*), max(a) FROM t1;",
      "PRAGMA table_info(t1);",
      "SELECT a, hex(c) FROM t1 WHERE b LIKE ?1;"
    };
    int idx = pick_int(Data, Size, &off, 0, (int)(sizeof(sqls) / sizeof(sqls[0])) - 1);
    rc = sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
      sqlite3_close(db);
      return 0;
    }
  }

  bind_from_fuzz(stmt, Data, Size, &off);

  (void)sqlite3_column_count(NULL);
  (void)sqlite3_data_count(NULL);
  (void)sqlite3_stmt_busy(NULL);

  (void)sqlite3_column_count(stmt);
  (void)sqlite3_data_count(stmt);
  (void)sqlite3_stmt_busy(stmt);

  {
    int resetFlg = read_u8(Data, Size, &off) & 1;
    int ops[] = {
      SQLITE_STMTSTATUS_FULLSCAN_STEP,
      SQLITE_STMTSTATUS_SORT,
      SQLITE_STMTSTATUS_AUTOINDEX,
      SQLITE_STMTSTATUS_VM_STEP,
      SQLITE_STMTSTATUS_REPREPARE,
      SQLITE_STMTSTATUS_RUN
    };
    int op = ops[pick_int(Data, Size, &off, 0, (int)(sizeof(ops) / sizeof(ops[0])) - 1)];
    (void)sqlite3_stmt_status(stmt, op, resetFlg);
  }

  for (int iter = 0; iter < 4; iter++) {
    rc = sqlite3_step(stmt);

    (void)sqlite3_stmt_busy(stmt);
    (void)sqlite3_column_count(stmt);
    (void)sqlite3_data_count(stmt);

    if (rc == SQLITE_ROW) {
      int cols = sqlite3_column_count(stmt);
      int dcols = sqlite3_data_count(stmt);
      int maxcols = cols > dcols ? cols : dcols;
      if (maxcols < 0) maxcols = 0;

      for (int i = -1; i <= maxcols + 1; i++) {
        const void *txt16 = sqlite3_column_text16(stmt, i);
        (void)txt16;
      }
    } else if (rc == SQLITE_DONE) {
      break;
    } else {
      break;
    }

#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
    {
      int sidx = pick_int(Data, Size, &off, -1, 4);
      sqlite3_int64 i64out = 0x1122334455667788LL;
      double dout = 123.456;
      const char *sout = "unchanged";

      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_NLOOP, &i64out);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_NVISIT, &i64out);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_EST, &dout);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_NAME, &sout);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_EXPLAIN, &sout);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_SELECTID, &i64out);
    }
#endif
  }

#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
  {
    for (int sidx = -1; sidx < 3; sidx++) {
      sqlite3_int64 i64out = 0;
      double dout = 0.0;
      const char *sout = NULL;
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_NLOOP, &i64out);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_NVISIT, &i64out);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_EST, &dout);
      (void)sqlite3_stmt_scanstatus(stmt, sidx, SQLITE_SCANSTAT_NAME, &sout);
    }
  }
#endif

  {
    int resetFlg = read_u8(Data, Size, &off) & 1;
    int ops[] = {
      SQLITE_STMTSTATUS_FULLSCAN_STEP,
      SQLITE_STMTSTATUS_SORT,
      SQLITE_STMTSTATUS_AUTOINDEX,
      SQLITE_STMTSTATUS_VM_STEP,
      SQLITE_STMTSTATUS_REPREPARE,
      SQLITE_STMTSTATUS_RUN
    };
    for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
      (void)sqlite3_stmt_status(stmt, ops[i], resetFlg);
    }
  }

  (void)sqlite3_reset(stmt);
  (void)sqlite3_stmt_busy(stmt);
  (void)sqlite3_data_count(stmt);
  (void)sqlite3_column_count(stmt);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}