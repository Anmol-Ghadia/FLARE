// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_value *v = (argc > 0) ? argv[0] : NULL;
  int iv = 0;
  int nbytes = 0;
  const void *blob = NULL;
  void *mem = NULL;
  int alloc_sz = 0;

  if (v) {
    iv = sqlite3_value_int(v);
    nbytes = sqlite3_value_bytes(v);
    sqlite3_result_value(ctx, v);
    blob = sqlite3_value_blob(v);
  } else {
    sqlite3_result_error(ctx, "no value", -1);
    return;
  }

  alloc_sz = (iv & 0x3ff) + 1;
  mem = sqlite3_malloc(alloc_sz);

  if (mem && blob && nbytes > 0) {
    int copy_n = nbytes < alloc_sz ? nbytes : alloc_sz;
    memcpy(mem, blob, (size_t)copy_n);
  } else if (mem) {
    memset(mem, iv & 0xff, (size_t)alloc_sz);
  }

  if ((iv & 1) != 0) {
    const char *err = NULL;
    int errlen = 0;

    if (blob && nbytes > 0) {
      err = (const char *)blob;
      errlen = nbytes;
    } else if (mem && alloc_sz > 0) {
      err = (const char *)mem;
      errlen = alloc_sz;
    } else {
      err = "err";
      errlen = 3;
    }

    sqlite3_result_error(ctx, err, errlen);
  } else {
    const void *out = mem ? mem : blob;
    int out_n = 0;
    if (mem) {
      out_n = alloc_sz;
    } else if (blob && nbytes > 0) {
      out_n = nbytes;
    }
    sqlite3_result_blob(ctx, out, out_n, SQLITE_TRANSIENT);
  }

  sqlite3_free(mem);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size > 0) {
        fwrite(Data, 1, Size, fp);
      }
      fclose(fp);
    }
  }

  sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

  rc = sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
  if (rc == SQLITE_OK && stmt) {
    if (Size == 0) {
      sqlite3_bind_null(stmt, 1);
    } else {
      switch (Data[0] % 5) {
        case 0:
          sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
          break;
        case 1:
          sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
          break;
        case 2: {
          sqlite3_int64 v64 = 0;
          size_t n = Size < sizeof(v64) ? Size : sizeof(v64);
          memcpy(&v64, Data, n);
          sqlite3_bind_int64(stmt, 1, v64);
          break;
        }
        case 3: {
          double d = 0.0;
          size_t n = Size < sizeof(d) ? Size : sizeof(d);
          memcpy(&d, Data, n);
          sqlite3_bind_double(stmt, 1, d);
          break;
        }
        default:
          sqlite3_bind_null(stmt, 1);
          break;
      }
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }

  if (stmt) {
    sqlite3_finalize(stmt);
  }
  sqlite3_close(db);
  return 0;
}