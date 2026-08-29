// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_vtab_nochange at sqlite3.c:72738:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_vtab_nochange at sqlite3.c:72738:16 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_vtab_nochange at sqlite3.c:72738:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
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
#include <stdlib.h>
#include "sqlite3.h"

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int v32 = 0;
  sqlite3_int64 v64 = 0;
  int blobsz = 0;
  int mode = 0;
  sqlite3 *db = NULL;

  if (argc > 0 && argv && argv[0]) v32 = sqlite3_value_int(argv[0]);
  if (argc > 1 && argv && argv[1]) v64 = sqlite3_value_int64(argv[1]);
  if (argc > 2 && argv && argv[2]) blobsz = sqlite3_value_int(argv[2]);
  if (argc > 3 && argv && argv[3]) mode = sqlite3_value_int(argv[3]);

  db = sqlite3_context_db_handle(ctx);
  (void)db;

  if (blobsz < 0) blobsz = -blobsz;
  if (blobsz > 1 << 20) blobsz %= (1 << 20);

  switch ((unsigned)mode % 6U) {
    case 0:
      sqlite3_result_int(ctx, v32);
      break;
    case 1:
      sqlite3_result_int64(ctx, v64);
      break;
    case 2:
      sqlite3_result_zeroblob(ctx, blobsz);
      break;
    case 3:
      (void)sqlite3_vtab_nochange(ctx);
      sqlite3_result_int(ctx, v32 ^ (int)(v64 & 0xffffffff));
      break;
    case 4:
      (void)sqlite3_vtab_nochange(ctx);
      sqlite3_result_zeroblob(ctx, blobsz);
      break;
    default:
      sqlite3_result_int64(ctx, v64);
      sqlite3_result_int(ctx, v32);
      break;
  }
}

static void fuzz_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  int i;
  if (!acc) return;
  for (i = 0; i < argc; i++) {
    if (argv && argv[i]) {
      *acc ^= sqlite3_value_int64(argv[i]) + (sqlite3_int64)i;
    }
  }
  (void)sqlite3_context_db_handle(ctx);
  (void)sqlite3_vtab_nochange(ctx);
}

static void fuzz_agg_final(sqlite3_context *ctx) {
  sqlite3_int64 *acc = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  if (acc) {
    sqlite3_result_int64(ctx, *acc);
  } else {
    sqlite3_result_int(ctx, 0);
  }
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

int LLVMFuzzerTestOneInput_69(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  char funcname[64];
  int nArg;
  int encFlags;
  size_t i;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  memset(funcname, 0, sizeof(funcname));
  if (Size == 0) {
    memcpy(funcname, "f0", 3);
  } else {
    size_t name_len = Size < 40 ? Size : 40;
    funcname[0] = 'f';
    for (i = 0; i < name_len && (i + 1) < sizeof(funcname) - 1; i++) {
      unsigned char c = Data[i];
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
        funcname[i + 1] = (char)c;
      } else {
        funcname[i + 1] = (char)('a' + (c % 26));
      }
    }
    if (funcname[1] == '\0') memcpy(funcname, "f1", 3);
  }

  if (Size > 0) {
    unsigned char b = Data[0];
    if ((b % 5) == 0) nArg = -1;
    else nArg = (int)(b % 5);
  } else {
    nArg = -1;
  }

  encFlags = SQLITE_UTF8;
  if (Size > 1) {
    switch (Data[1] % 4) {
      case 0: encFlags = SQLITE_UTF8; break;
      case 1: encFlags = SQLITE_UTF16LE; break;
      case 2: encFlags = SQLITE_UTF16BE; break;
      default: encFlags = SQLITE_UTF16; break;
    }
  }
  if (Size > 2 && (Data[2] & 1)) encFlags |= SQLITE_DETERMINISTIC;
  if (Size > 3 && (Data[3] & 1)) encFlags |= SQLITE_DIRECTONLY;

  sqlite3_create_function(db, funcname, nArg, encFlags, NULL, fuzz_scalar, NULL, NULL);
  sqlite3_create_function(db, "aggfuzz", nArg, encFlags, NULL, NULL, fuzz_agg_step, fuzz_agg_final);

  exec_sql(db, "PRAGMA journal_mode=OFF;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b,c,d);");
  exec_sql(db, "DELETE FROM t;");
  exec_sql(db, "INSERT INTO t(a,b,c,d) VALUES(1,2,3,4),(5,6,7,8),(NULL,-1,1234567890123,'x');");

  {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT %s(1, 2, 3, 4), %s(a,b,c,d), aggfuzz(a,b,c,d) FROM t;",
             funcname, funcname);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT %s(x'010203', -2147483648, 1048576, 2), "
             "%s(9223372036854775807, -9223372036854775807, 0, 1);",
             funcname, funcname);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT %s(?1, ?2, ?3, ?4);",
             funcname);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)(Size > 1024 ? 1024 : Size), SQLITE_STATIC);
      sqlite3_bind_int(stmt, 2, (Size > 4) ? (int)(Data[4] | (Data[5 % (Size ? Size : 1)] << 8)) : 0);
      sqlite3_bind_int64(stmt, 3, (sqlite3_int64)Size);
      sqlite3_bind_int(stmt, 4, (Size > 6) ? Data[6] : 0);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_create_function(db, funcname, nArg, encFlags, NULL, NULL, NULL, NULL);

  sqlite3_close(db);
  return 0;
}