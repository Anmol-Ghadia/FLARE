#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static int pick_int(const uint8_t **p, size_t *n){
  int v = 0;
  size_t take = *n < sizeof(int) ? *n : sizeof(int);
  if(take){
    memcpy(&v, *p, take);
    *p += take;
    *n -= take;
  }
  return v;
}

static unsigned pick_u32(const uint8_t **p, size_t *n){
  unsigned v = 0;
  size_t take = *n < sizeof(unsigned) ? *n : sizeof(unsigned);
  if(take){
    memcpy(&v, *p, take);
    *p += take;
    *n -= take;
  }
  return v;
}

static int pick_bool(const uint8_t **p, size_t *n){
  if(*n == 0) return 0;
  {
    int v = (**p) & 1;
    (*p)++;
    (*n)--;
    return v;
  }
}

static void write_dummy_file(const uint8_t *Data, size_t Size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(!fp) return;
  if(Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static void exercise_stmt(sqlite3_stmt **ppStmt, const uint8_t *Data, size_t Size){
  sqlite3_stmt *stmt;
  const uint8_t *p = Data;
  size_t n = Size;
  int i;

  if(ppStmt == NULL || *ppStmt == NULL) return;
  stmt = *ppStmt;

  (void)sqlite3_stmt_isexplain(NULL);
  (void)sqlite3_expired(NULL);
  (void)sqlite3_stmt_isexplain(stmt);
  (void)sqlite3_expired(stmt);

  {
    int step_count = (int)(pick_u32(&p, &n) % 4U);
    for(i = 0; i < step_count; i++){
      int s = sqlite3_step(stmt);
      if(s != SQLITE_ROW && s != SQLITE_DONE){
        break;
      }
    }
  }

  (void)sqlite3_reset(stmt);
  (void)sqlite3_stmt_isexplain(stmt);
  (void)sqlite3_expired(stmt);

  {
    static const int status_ops[] = {
#ifdef SQLITE_STMTSTATUS_FULLSCAN_STEP
      SQLITE_STMTSTATUS_FULLSCAN_STEP,
#endif
#ifdef SQLITE_STMTSTATUS_SORT
      SQLITE_STMTSTATUS_SORT,
#endif
#ifdef SQLITE_STMTSTATUS_AUTOINDEX
      SQLITE_STMTSTATUS_AUTOINDEX,
#endif
#ifdef SQLITE_STMTSTATUS_VM_STEP
      SQLITE_STMTSTATUS_VM_STEP,
#endif
#ifdef SQLITE_STMTSTATUS_REPREPARE
      SQLITE_STMTSTATUS_REPREPARE,
#endif
#ifdef SQLITE_STMTSTATUS_RUN
      SQLITE_STMTSTATUS_RUN,
#endif
#ifdef SQLITE_STMTSTATUS_FILTER_MISS
      SQLITE_STMTSTATUS_FILTER_MISS,
#endif
#ifdef SQLITE_STMTSTATUS_FILTER_HIT
      SQLITE_STMTSTATUS_FILTER_HIT,
#endif
#ifdef SQLITE_STMTSTATUS_MEMUSED
      SQLITE_STMTSTATUS_MEMUSED,
#endif
      -1
    };
    int count = (int)(pick_u32(&p, &n) % 8U) + 1;
    for(i = 0; i < count && stmt; i++){
      int idx = (int)(pick_u32(&p, &n) % (sizeof(status_ops) / sizeof(status_ops[0])));
      int op = status_ops[idx];
      int resetFlg = pick_bool(&p, &n);
      if(op == -1) continue;
      (void)sqlite3_stmt_status(stmt, op, resetFlg);
#ifdef SQLITE_STMTSTATUS_MEMUSED
      if(op == SQLITE_STMTSTATUS_MEMUSED){
        *ppStmt = NULL;
        return;
      }
#endif
    }
  }

#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
  if(stmt){
    int loops = (int)(pick_u32(&p, &n) % 6U) - 1;
    sqlite3_int64 i64out = 0x1122334455667788LL;
    double dout = 123.456;
    const char *sout = "unchanged";

#ifdef SQLITE_SCANSTAT_NLOOP
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_NLOOP, &i64out);
#endif
#ifdef SQLITE_SCANSTAT_NVISIT
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_NVISIT, &i64out);
#endif
#ifdef SQLITE_SCANSTAT_EST
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_EST, &dout);
#endif
#ifdef SQLITE_SCANSTAT_NAME
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_NAME, &sout);
#endif
#ifdef SQLITE_SCANSTAT_EXPLAIN
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_EXPLAIN, &sout);
#endif
#ifdef SQLITE_SCANSTAT_SELECTID
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_SELECTID, &i64out);
#endif
#ifdef SQLITE_SCANSTAT_PARENTID
    (void)sqlite3_stmt_scanstatus(stmt, loops, SQLITE_SCANSTAT_PARENTID, &i64out);
#endif
  }
#endif

  if(*ppStmt){
    (void)sqlite3_reset(*ppStmt);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const void *tail16 = NULL;
  int rc;

  write_dummy_file(Data, Size);

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "PRAGMA writable_schema=ON;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS idx_t_a ON t(a);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(a,b,c) VALUES(1,'x',X'00');", NULL, NULL, NULL);

  {
    const uint8_t *p = Data;
    size_t n = Size;
    int requested_nbyte = pick_int(&p, &n);
    int prep_nbyte;
    unsigned char *sql16;

    if(n < 2){
      sqlite3_close(db);
      return 0;
    }

    if(requested_nbyte < 0 || (size_t)requested_nbyte > n){
      prep_nbyte = (int)(n & ~(size_t)1);
    }else{
      prep_nbyte = requested_nbyte & ~1;
      if((size_t)prep_nbyte > n){
        prep_nbyte = (int)(n & ~(size_t)1);
      }
    }

    if(prep_nbyte < 2){
      sqlite3_close(db);
      return 0;
    }

    sql16 = (unsigned char *)malloc((size_t)prep_nbyte + 2);
    if(!sql16){
      sqlite3_close(db);
      return 0;
    }

    memcpy(sql16, p, (size_t)prep_nbyte);
    sql16[prep_nbyte] = 0;
    sql16[prep_nbyte + 1] = 0;

    rc = sqlite3_prepare16(db, (const void *)sql16, prep_nbyte, &stmt, &tail16);
    (void)tail16;
    if(rc == SQLITE_OK && stmt){
      exercise_stmt(&stmt, p + prep_nbyte, n - (size_t)prep_nbyte);
    }

    if(stmt){
      sqlite3_finalize(stmt);
      stmt = NULL;
    }

    free(sql16);
  }

  sqlite3_close(db);
  return 0;
}