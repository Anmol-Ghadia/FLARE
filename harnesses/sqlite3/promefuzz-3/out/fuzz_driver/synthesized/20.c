// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void udf_target(sqlite3_context *context, int argc, sqlite3_value **argv) {
  const unsigned char *txt;
  sqlite3_uint64 alloc_size = 0;
  void *mem = NULL;
  int copy_len = 0;

  (void)argc;

  txt = sqlite3_value_text(argv[0]);

  if (argc > 1) {
    alloc_size = (sqlite3_uint64)sqlite3_value_int64(argv[1]);
  }

  if (alloc_size > 1U << 20) {
    alloc_size = alloc_size & 0xFFFF;
  }

  mem = sqlite3_malloc64(alloc_size);

  if (mem != NULL && alloc_size > 0) {
    if (txt != NULL) {
      copy_len = sqlite3_value_bytes(argv[0]);
      if ((sqlite3_uint64)copy_len >= alloc_size) {
        copy_len = (int)alloc_size - 1;
      }
      if (copy_len > 0) {
        memcpy(mem, txt, (size_t)copy_len);
      }
      ((char *)mem)[copy_len >= 0 ? copy_len : 0] = '\0';
      sqlite3_result_text(context, (const char *)mem, copy_len, SQLITE_TRANSIENT);
    } else {
      ((char *)mem)[0] = '\0';
      sqlite3_result_text(context, (const char *)mem, 0, SQLITE_TRANSIENT);
    }
  } else {
    sqlite3_result_text(context, txt ? (const char *)txt : "", -1, SQLITE_TRANSIENT);
  }

  sqlite3_free(mem);
}

int LLVMFuzzerTestOneInput_20(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  sqlite3_int64 n = 0;
  const char *sql = "SELECT fuzz_target(?1, ?2);";

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) {
      sqlite3_close(db);
    }
    return 0;
  }

  rc = sqlite3_create_function(db, "fuzz_target", 2, SQLITE_UTF8, NULL,
                               udf_target, NULL, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK || stmt == NULL) {
    sqlite3_close(db);
    return 0;
  }

  if (Size == 0) {
    sqlite3_bind_null(stmt, 1);
    sqlite3_bind_int64(stmt, 2, 0);
  } else {
    switch (Data[0] % 4) {
      case 0:
        sqlite3_bind_text(stmt, 1, (const char *)(Data + 1),
                          (int)(Size > 1 ? Size - 1 : 0), SQLITE_TRANSIENT);
        break;
      case 1:
        sqlite3_bind_blob(stmt, 1, Data + 1,
                          (int)(Size > 1 ? Size - 1 : 0), SQLITE_TRANSIENT);
        break;
      case 2:
        if (Size > 1) {
          memcpy(&n, Data + 1, (Size - 1) < sizeof(n) ? (Size - 1) : sizeof(n));
        }
        sqlite3_bind_int64(stmt, 1, n);
        break;
      default: {
        double d = 0.0;
        if (Size > 1) {
          memcpy(&d, Data + 1, (Size - 1) < sizeof(d) ? (Size - 1) : sizeof(d));
        }
        sqlite3_bind_double(stmt, 1, d);
        break;
      }
    }

    n = 0;
    if (Size > 1) {
      size_t off = 1;
      size_t rem = Size - off;
      memcpy(&n, Data + off, rem < sizeof(n) ? rem : sizeof(n));
    }
    sqlite3_bind_int64(stmt, 2, n);
  }

  (void)sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}