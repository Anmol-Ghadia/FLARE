// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
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

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4; i++){
    v <<= 8;
    if(*Off < Size){
      v |= Data[*Off];
      (*Off)++;
    }
  }
  return (int)v;
}

static int read_i32(const uint8_t *Data, size_t Size, size_t *Off){
  return read_u32(Data, Size, Off);
}

static void exec_sql(sqlite3 *db, const char *sql){
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if(errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  size_t off = 0;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close_v2(db);
    return 0;
  }

  exec_sql(db, "PRAGMA journal_mode=WAL;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT, c BLOB);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x INTEGER PRIMARY KEY, y TEXT);");

  sqlite3_busy_timeout(db, read_i32(Data, Size, &off));
  sqlite3_wal_autocheckpoint(db, read_i32(Data, Size, &off));

#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_DEFENSIVE
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_DQS_DML
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DML, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_DQS_DDL
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DDL, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_ALTER_TABLE
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_LEGACY_ALTER_TABLE, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_WRITABLE_SCHEMA
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_WRITABLE_SCHEMA, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_RESET_DATABASE
  {
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, read_i32(Data, Size, &off) & 1, &out);
  }
#endif
#ifdef SQLITE_DBCONFIG_LOOKASIDE
  {
    sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, NULL, 0, 0);
  }
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
  {
    const char *names[] = {"main", "aux", "tempmain", "db0"};
    sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, names[(read_u32(Data, Size, &off) & 3)]);
  }
#endif

  if(read_u32(Data, Size, &off) & 1){
    exec_sql(db, "BEGIN IMMEDIATE;");
  }else{
    exec_sql(db, "BEGIN;");
  }

  for(i = 0; i < 8; i++){
    int op = read_u32(Data, Size, &off) % 6;
    sqlite3_get_autocommit(db);

    switch(op){
      case 0:
        exec_sql(db, "INSERT INTO t(a,b,c) VALUES(1,'x',X'00FF');");
        break;
      case 1:
        exec_sql(db, "UPDATE t SET b='y' WHERE a=1;");
        break;
      case 2:
        exec_sql(db, "DELETE FROM t WHERE a=1;");
        break;
      case 3:
        exec_sql(db, "INSERT OR REPLACE INTO t2(x,y) VALUES(1,'z');");
        break;
      case 4:
        if(stmt == NULL){
          if(sqlite3_prepare_v2(db, "SELECT a,b,c FROM t;", -1, &stmt, NULL) == SQLITE_OK){
            sqlite3_step(stmt);
          }
        }
        break;
      case 5:
        sqlite3_db_cacheflush(db);
        break;
    }

    if(read_u32(Data, Size, &off) & 1){
      sqlite3_db_cacheflush(db);
    }
    if(read_u32(Data, Size, &off) & 1){
      sqlite3_get_autocommit(db);
    }
  }

  if(stmt){
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  if(read_u32(Data, Size, &off) & 1){
    exec_sql(db, "COMMIT;");
  }else{
    exec_sql(db, "ROLLBACK;");
  }

  sqlite3_get_autocommit(db);
  sqlite3_db_cacheflush(db);
  sqlite3_wal_autocheckpoint(db, read_i32(Data, Size, &off));
  sqlite3_busy_timeout(db, read_i32(Data, Size, &off));

  sqlite3_close_v2(db);
  return 0;
}