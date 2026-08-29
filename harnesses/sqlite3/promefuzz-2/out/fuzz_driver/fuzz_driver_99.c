// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void scalar_udf(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  const void *blob = NULL;
  const void *txt16 = NULL;
  int blob_n = 0;
  int txt16_n = 0;
  int ival = 0;
  int aux_index = 0;
  int mode = 0;
  int n = -1;

  if (argc > 0) {
    blob = sqlite3_value_blob(argv[0]);
    blob_n = sqlite3_value_bytes(argv[0]);
    if (blob_n < 0) blob_n = 0;
    sqlite3_result_blob(ctx, blob, blob_n, SQLITE_TRANSIENT);
  }

  if (argc > 1) {
    txt16 = sqlite3_value_text16(argv[1]);
    txt16_n = sqlite3_value_bytes16(argv[1]);
    (void)txt16_n;
  }

  if (argc > 2) {
    ival = sqlite3_value_int(argv[2]);
  }

  if (argc > 3) {
    aux_index = sqlite3_value_int(argv[3]);
  }

  if (argc > 5) {
    mode = sqlite3_value_int(argv[5]) & 3;
  }

  if (argc > 6) {
    n = sqlite3_value_int(argv[6]);
  }

  if (txt16) {
    if (mode == 0) {
      sqlite3_result_text16(ctx, txt16, n, SQLITE_TRANSIENT);
    } else if (mode == 1) {
      sqlite3_result_text16le(ctx, txt16, n, SQLITE_TRANSIENT);
    } else if (mode == 2) {
      sqlite3_result_text16(ctx, txt16, -1, SQLITE_TRANSIENT);
    } else {
      sqlite3_result_text16le(ctx, txt16, -1, SQLITE_TRANSIENT);
    }
  }

  sqlite3_result_int(ctx, ival);

  if (argc > 7) {
    int aux_len = sqlite3_value_bytes(argv[7]);
    const void *aux_src = sqlite3_value_blob(argv[7]);
    if (aux_len < 0) aux_len = 0;
    if (aux_len > 0 && aux_src) {
      void *aux = sqlite3_malloc((sqlite3_uint64)aux_len);
      if (aux) {
        memcpy(aux, aux_src, (size_t)aux_len);
        sqlite3_set_auxdata(ctx, aux_index, aux, sqlite3_free);
      } else {
        sqlite3_set_auxdata(ctx, aux_index, NULL, NULL);
      }
    } else {
      sqlite3_set_auxdata(ctx, aux_index, NULL, NULL);
    }
  }
}

static void agg_udf_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int agg_n = 0;
  unsigned char *p;

  if (argc > 4) {
    agg_n = sqlite3_value_int(argv[4]);
  }
  if (agg_n < 1) agg_n = 1;
  if (agg_n > 4096) agg_n = 4096;

  p = (unsigned char *)sqlite3_aggregate_context(ctx, agg_n);
  if (p) {
    if (argc > 0) {
      int blob_n = sqlite3_value_bytes(argv[0]);
      p[0] ^= (unsigned char)(blob_n & 0xff);
    }
    if (agg_n > 1 && argc > 1) {
      int txt16_n = sqlite3_value_bytes16(argv[1]);
      p[1] ^= (unsigned char)(txt16_n & 0xff);
    }
    if (agg_n > 2 && argc > 2) {
      int ival = sqlite3_value_int(argv[2]);
      p[2] ^= (unsigned char)(ival & 0xff);
    }
  }

  scalar_udf(ctx, argc, argv);
}

static void agg_udf_final(sqlite3_context *ctx) {
  unsigned char *p = (unsigned char *)sqlite3_aggregate_context(ctx, 0);
  if (p) {
    sqlite3_result_int(ctx, (int)p[0]);
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp = NULL;
  int rc;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 8, SQLITE_UTF8, NULL, scalar_udf, NULL, NULL);
  sqlite3_create_function(db, "fuzzagg", 8, SQLITE_UTF8, NULL, NULL, agg_udf_step, agg_udf_final);

  exec_sql(db, "CREATE TABLE t(a BLOB, b BLOB, c INT, d INT, e INT, f INT, g INT, h BLOB);");

  {
    const char *ins = "INSERT INTO t(a,b,c,d,e,f,g,h) VALUES(?1,?2,?3,?4,?5,?6,?7,?8);";
    sqlite3_stmt *istmt = NULL;
    if (sqlite3_prepare_v2(db, ins, -1, &istmt, NULL) == SQLITE_OK) {
      size_t q1 = Size / 4;
      size_t q2 = Size / 2;
      size_t q3 = (Size * 3) / 4;

      sqlite3_bind_blob(istmt, 1, Data, (int)q1, SQLITE_STATIC);
      sqlite3_bind_blob(istmt, 2, Data + q1, (int)(q2 - q1), SQLITE_STATIC);
      sqlite3_bind_int(istmt, 3, Size > 0 ? (int)(int8_t)Data[0] : 0);
      sqlite3_bind_int(istmt, 4, Size > 1 ? (int)(int8_t)Data[1] : 0);
      sqlite3_bind_int(istmt, 5, Size > 2 ? (int)(unsigned char)Data[2] * 32 - 1024 : 1);
      sqlite3_bind_int(istmt, 6, Size > 3 ? (int)(Data[3] & 3) : 0);
      sqlite3_bind_int(istmt, 7, Size > 4 ? (int)(int8_t)Data[4] : -1);
      sqlite3_bind_blob(istmt, 8, Data + q3, (int)(Size - q3), SQLITE_STATIC);
      sqlite3_step(istmt);
    }
    sqlite3_finalize(istmt);
  }

  {
    const char *sql1 = "SELECT fuzzfunc(a,b,c,d,e,f,g,h) FROM t;";
    sqlite3_stmt *s1 = NULL;
    if (sqlite3_prepare_v2(db, sql1, -1, &s1, NULL) == SQLITE_OK) {
      while ((rc = sqlite3_step(s1)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(s1);
  }

  {
    const char *sql2 = "SELECT fuzzagg(a,b,c,d,e,f,g,h) FROM t;";
    sqlite3_stmt *s2 = NULL;
    if (sqlite3_prepare_v2(db, sql2, -1, &s2, NULL) == SQLITE_OK) {
      while ((rc = sqlite3_step(s2)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(s2);
  }

  {
    const char *sel =
        "SELECT fuzzfunc(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8), "
        "       fuzzagg(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);";
    if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
      size_t p1 = Size / 3;
      size_t p2 = (Size * 2) / 3;
      sqlite3_bind_blob(stmt, 1, Data, (int)p1, SQLITE_STATIC);
      sqlite3_bind_blob(stmt, 2, Data + p1, (int)(p2 - p1), SQLITE_STATIC);
      sqlite3_bind_int(stmt, 3, Size > 0 ? (int)(int8_t)Data[0] : 0);
      sqlite3_bind_int(stmt, 4, Size > 1 ? (int)(int8_t)Data[1] : 0);
      sqlite3_bind_int(stmt, 5, Size > 2 ? (int)(unsigned char)Data[2] * 64 - 2048 : 1);
      sqlite3_bind_int(stmt, 6, Size > 3 ? (int)(Data[3] & 3) : 0);
      sqlite3_bind_int(stmt, 7, Size > 4 ? (int)(int8_t)Data[4] : -1);
      sqlite3_bind_blob(stmt, 8, Data + p2, (int)(Size - p2), SQLITE_STATIC);
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}