// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_blob_reopen at sqlite3.c:83034:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_blob_reopen at sqlite3.c:83034:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static sqlite3_int64 rd64(const uint8_t *p){
  sqlite3_int64 v = 0;
  for(int i = 0; i < 8; i++){
    v |= ((sqlite3_int64)p[i]) << (8 * i);
  }
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql){
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if(errmsg) sqlite3_free(errmsg);
  return rc;
}

static void copy_token(char *dst, size_t dstsz, const uint8_t *src, size_t n){
  size_t j = 0;
  if(dstsz == 0) return;
  for(size_t i = 0; i < n && j + 1 < dstsz; i++){
    unsigned char c = src[i];
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
       (c >= '0' && c <= '9') || c == '_'){
      dst[j++] = (char)c;
    }
  }
  if(j == 0){
    dst[j++] = 'x';
  }
  dst[j] = '\0';
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db1 = NULL, *db2 = NULL;
  sqlite3_blob *blob = NULL;
  FILE *fp;
  int rc;
  const char *filename = "./dummy_file";

  fp = fopen(filename, "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db1);
  if(rc != SQLITE_OK || db1 == NULL){
    if(db1) sqlite3_close(db1);
    return 0;
  }

  {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if(Size > 0){
      if(Data[0] & 0x01) flags = SQLITE_OPEN_READONLY;
      else if(Data[0] & 0x02) flags = SQLITE_OPEN_READWRITE;
      else flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
      if(Data[0] & 0x04) flags |= SQLITE_OPEN_URI;
      if(Data[0] & 0x08) flags |= SQLITE_OPEN_MEMORY;
      if(Data[0] & 0x10) flags |= SQLITE_OPEN_NOMUTEX;
      if(Data[0] & 0x20) flags |= SQLITE_OPEN_FULLMUTEX;
      if(Data[0] & 0x40) flags |= SQLITE_OPEN_PRIVATECACHE;
      if(Data[0] & 0x80) flags |= SQLITE_OPEN_SHAREDCACHE;
    }
    rc = sqlite3_open_v2(filename, &db2, flags, NULL);
    if(rc != SQLITE_OK && db2 == NULL){
      db2 = NULL;
    }
  }

  exec_sql(db1, "PRAGMA foreign_keys=ON;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY AUTOINCREMENT, a TEXT NOT NULL, b BLOB, c INTEGER UNIQUE);");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t2(x INTEGER PRIMARY KEY, y BLOB, z TEXT);");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS wr(a INTEGER PRIMARY KEY, b TEXT) WITHOUT ROWID;");
  exec_sql(db1, "CREATE VIEW IF NOT EXISTS v1 AS SELECT a,b FROM t1;");
  exec_sql(db1, "INSERT INTO t1(a,b,c) VALUES('alpha', zeroblob(16), 1);");
  exec_sql(db1, "INSERT INTO t1(a,b,c) VALUES('beta', x'01020304', 2);");
  exec_sql(db1, "INSERT INTO t2(x,y,z) VALUES(1, zeroblob(8), 'txt');");
  exec_sql(db1, "CREATE TEMP TABLE IF NOT EXISTS temp_tt(q BLOB, r TEXT);");
  exec_sql(db1, "INSERT INTO temp_tt(q,r) VALUES(zeroblob(4), 'tmp');");
  exec_sql(db1, "ATTACH DATABASE ':memory:' AS aux;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS aux.atab(k INTEGER PRIMARY KEY, bb BLOB, cc TEXT);");
  exec_sql(db1, "INSERT INTO aux.atab(k,bb,cc) VALUES(1, zeroblob(12), 'aux');");

  {
    int limit_ids[] = {
      SQLITE_LIMIT_LENGTH,
      SQLITE_LIMIT_SQL_LENGTH,
      SQLITE_LIMIT_COLUMN,
      SQLITE_LIMIT_EXPR_DEPTH,
      SQLITE_LIMIT_COMPOUND_SELECT,
      SQLITE_LIMIT_VDBE_OP,
      SQLITE_LIMIT_FUNCTION_ARG,
      SQLITE_LIMIT_ATTACHED,
      SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
      SQLITE_LIMIT_VARIABLE_NUMBER,
      SQLITE_LIMIT_TRIGGER_DEPTH,
      SQLITE_LIMIT_WORKER_THREADS
    };
    size_t nids = sizeof(limit_ids) / sizeof(limit_ids[0]);
    for(size_t i = 0; i < nids; i++){
      int newVal = -1;
      if(Size >= 8 + i * 4 + 4){
        newVal = (int)(rd32(Data + 8 + i * 4) % 10000);
        if((Data[(8 + i * 4) % Size] & 1) == 0) newVal = -1;
      }
      sqlite3_limit(db1, limit_ids[i], newVal);
    }
    if(Size > 2){
      sqlite3_limit(db1, (int)(Data[1] % 64), (Data[2] & 1) ? (int)(Data[2] * 100) : -1);
    }
  }

  {
    const char *dbnames[] = {"main", "temp", "aux", "nope", "", NULL};
    for(size_t i = 0; i < sizeof(dbnames)/sizeof(dbnames[0]); i++){
      sqlite3_db_readonly(db1, dbnames[i]);
    }
    if(db2){
      for(size_t i = 0; i < sizeof(dbnames)/sizeof(dbnames[0]); i++){
        sqlite3_db_readonly(db2, dbnames[i]);
      }
    }
  }

  {
    char tok1[32], tok2[32], tok3[32];
    const char *dbn_candidates[] = {"main", "temp", "aux", NULL, "nope"};
    const char *tbl_candidates[] = {"t1", "t2", "wr", "v1", "temp_tt", "atab", "missing"};
    const char *col_candidates[] = {"id", "a", "b", "c", "x", "y", "z", "q", "r", "bb", "cc", "rowid", "oid", "_rowid_", NULL, "missing"};
    const char *dtype = NULL, *coll = NULL;
    int notnull = 0, pk = 0, autoinc = 0;
    size_t dbn_count = sizeof(dbn_candidates) / sizeof(dbn_candidates[0]);

    memset(tok1, 0, sizeof(tok1));
    memset(tok2, 0, sizeof(tok2));
    memset(tok3, 0, sizeof(tok3));

    if(Size > 16){
      copy_token(tok1, sizeof(tok1), Data + 0, Size > 8 ? 8 : Size);
      copy_token(tok2, sizeof(tok2), Data + 8, Size > 8 ? ((Size - 8 > 8) ? 8 : Size - 8) : 0);
      copy_token(tok3, sizeof(tok3), Data + 16, Size > 16 ? ((Size - 16 > 8) ? 8 : Size - 16) : 0);
    } else {
      strcpy(tok1, "main");
      strcpy(tok2, "t1");
      strcpy(tok3, "b");
    }

    for(size_t i = 0; i < sizeof(tbl_candidates)/sizeof(tbl_candidates[0]); i++){
      for(size_t j = 0; j < sizeof(col_candidates)/sizeof(col_candidates[0]); j++){
        const char *dbn = dbn_candidates[(i + j) % dbn_count];
        sqlite3_table_column_metadata(db1, dbn, tbl_candidates[i], col_candidates[j],
                                      &dtype, &coll, &notnull, &pk, &autoinc);
        sqlite3_table_column_metadata(db1, dbn, tbl_candidates[i], col_candidates[j],
                                      NULL, NULL, NULL, NULL, NULL);
        sqlite3_table_column_metadata(db1, dbn, tbl_candidates[i], col_candidates[j],
                                      &dtype, NULL, &notnull, NULL, &autoinc);
      }
      sqlite3_table_column_metadata(db1, NULL, tbl_candidates[i], NULL,
                                    &dtype, &coll, &notnull, &pk, &autoinc);
    }

    sqlite3_table_column_metadata(db1, tok1, tok2, tok3, &dtype, &coll, &notnull, &pk, &autoinc);
    sqlite3_table_column_metadata(db1, NULL, tok2, tok3, NULL, &coll, NULL, &pk, NULL);
    sqlite3_table_column_metadata(db1, tok1, tok2, NULL, NULL, NULL, NULL, NULL, NULL);
  }

  {
    const char *dbs[] = {"main", "temp", "aux", "nope"};
    const char *tables[] = {"t1", "t2", "wr", "temp_tt", "atab", "missing"};
    const char *cols[] = {"b", "a", "c", "y", "z", "q", "bb", "missing"};
    sqlite3_int64 rows[] = {1, 2, 3, 0, -1, 999999};
    for(size_t i = 0; i < sizeof(dbs)/sizeof(dbs[0]); i++){
      for(size_t j = 0; j < sizeof(tables)/sizeof(tables[0]); j++){
        for(size_t k = 0; k < sizeof(cols)/sizeof(cols[0]); k++){
          int wr = (int)((i + j + k) & 1);
          sqlite3_blob *b = NULL;
          sqlite3_blob_open(db1, dbs[i], tables[j], cols[k], rows[(i + j + k) % (sizeof(rows)/sizeof(rows[0]))], wr, &b);
          if(b){
            int n = sqlite3_blob_bytes(b);
            if(n > 0){
              unsigned char tmp[32];
              int amt = n < (int)sizeof(tmp) ? n : (int)sizeof(tmp);
              memset(tmp, 0, sizeof(tmp));
              sqlite3_blob_read(b, tmp, amt, 0);
              if(wr){
                sqlite3_blob_write(b, tmp, amt, 0);
              }
            }
            sqlite3_blob_reopen(b, 1);
            sqlite3_blob_close(b);
          }
        }
      }
    }

    if(Size >= 24){
      char dbn[32], tbl[32], col[32];
      sqlite3_int64 rowid = rd64(Data + (Size - 8));
      int wr = (Data[0] & 1);
      copy_token(dbn, sizeof(dbn), Data, Size > 8 ? 8 : Size);
      copy_token(tbl, sizeof(tbl), Data + 8, Size > 16 ? 8 : (Size > 8 ? Size - 8 : 0));
      copy_token(col, sizeof(col), Data + 16, Size > 24 ? 8 : (Size > 16 ? Size - 16 : 0));
      sqlite3_blob_open(db1, dbn, tbl, col, rowid, wr, &blob);
      if(blob){
        int n = sqlite3_blob_bytes(blob);
        if(n > 0){
          int amt = (int)((Size % (size_t)n) + 1);
          if(amt > n) amt = n;
          unsigned char *buf = (unsigned char *)malloc((size_t)amt);
          if(buf){
            memset(buf, 0, (size_t)amt);
            sqlite3_blob_read(blob, buf, amt, 0);
            if(wr){
              sqlite3_blob_write(blob, buf, amt, 0);
            }
            free(buf);
          }
        }
        sqlite3_blob_reopen(blob, 2);
        sqlite3_blob_close(blob);
        blob = NULL;
      }
    }
  }

  exec_sql(db1, "DETACH DATABASE aux;");

  if(blob) sqlite3_blob_close(blob);
  if(db2) sqlite3_close(db2);
  sqlite3_close(db1);
  return 0;
}