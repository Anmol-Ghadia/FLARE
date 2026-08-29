// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

typedef struct {
  const uint8_t *data;
  size_t size;
  size_t off;
} FuzzInput;

static uint8_t fi_u8(FuzzInput *in) {
  if (in->off >= in->size) return 0;
  return in->data[in->off++];
}

static uint32_t fi_u32(FuzzInput *in) {
  uint32_t v = 0;
  for (int i = 0; i < 4; i++) {
    v = (v << 8) | fi_u8(in);
  }
  return v;
}

static int fi_int(FuzzInput *in) {
  return (int)fi_u32(in);
}

static size_t fi_take_len(FuzzInput *in, size_t maxlen) {
  if (maxlen == 0) return 0;
  return (size_t)(fi_u32(in) % (maxlen + 1));
}

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  if (!ctx) return;

  int mode = 0;
  if (argv && argc > 0) {
    mode = sqlite3_value_int(argv[0]);
  }

  switch ((unsigned)mode % 6U) {
    case 0: {
      int err = SQLITE_ERROR;
      if (argv && argc > 1) {
        err = sqlite3_value_int(argv[1]);
      }
      sqlite3_result_error_code(ctx, err);
      break;
    }
    case 1: {
      const void *blob = NULL;
      int n = 0;
      if (argv && argc > 2) {
        blob = sqlite3_value_blob(argv[1]);
        n = sqlite3_value_int(argv[2]);
        if (n < 0) n = 0;
        {
          int actual = sqlite3_value_bytes(argv[1]);
          if (n > actual) n = actual;
        }
      }
      sqlite3_result_blob(ctx, blob, n, SQLITE_TRANSIENT);
      break;
    }
    case 2: {
      int v = 0;
      if (argv && argc > 1) {
        v = sqlite3_value_int(argv[1]);
      }
      sqlite3_result_int(ctx, v);
      break;
    }
    case 3: {
      const unsigned char *txt = (const unsigned char *)"";
      int n = -1;
      if (argv && argc > 2) {
        txt = sqlite3_value_text(argv[1]);
        n = sqlite3_value_int(argv[2]);
        if (!txt) {
          txt = (const unsigned char *)"";
          n = 0;
        } else if (n >= 0) {
          int actual = sqlite3_value_bytes(argv[1]);
          if (n > actual) n = actual;
        }
      }
      sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
      break;
    }
    case 4: {
      const void *txt16 = NULL;
      int n = -1;
      if (argv && argc > 2) {
        txt16 = sqlite3_value_blob(argv[1]);
        n = sqlite3_value_int(argv[2]);
        if (!txt16) {
          static const unsigned char empty16[2] = {0, 0};
          txt16 = empty16;
          n = -1;
        } else if (n >= 0) {
          int actual = sqlite3_value_bytes(argv[1]);
          if (n > actual) n = actual;
        }
      }
      sqlite3_result_error16(ctx, txt16, n);
      break;
    }
    case 5:
    default: {
      if (argv && argc > 1) {
        sqlite3_result_value(ctx, argv[1]);
      } else {
        sqlite3_result_int(ctx, 0);
      }
      break;
    }
  }
}

int LLVMFuzzerTestOneInput_59(const uint8_t *Data, size_t Size) {
  FuzzInput in;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  in.data = Data;
  in.size = Size;
  in.off = 0;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_result", 3, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c);"
               "INSERT INTO t VALUES(1,'x',X'00');"
               "INSERT INTO t VALUES(2,'hello',X'010203');"
               "INSERT INTO t VALUES(-1,NULL,NULL);",
               NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  while (in.off < in.size) {
    int mode = fi_u8(&in) % 6;
    int errcode = fi_int(&in);
    size_t blob_len = fi_take_len(&in, in.size - in.off);
    const uint8_t *blob_ptr = Data + in.off;
    in.off += blob_len;

    size_t text_len = fi_take_len(&in, in.size - in.off);
    const char *text_ptr = (const char *)(Data + in.off);
    in.off += text_len;

    size_t utf16_len = fi_take_len(&in, in.size - in.off);
    const void *utf16_ptr = (const void *)(Data + in.off);
    in.off += utf16_len;

    const char *sql =
        "SELECT "
        "fuzz_result(?1, ?2, ?3), "
        "fuzz_result(?1, a, ?3), "
        "fuzz_result(?1, b, ?3), "
        "fuzz_result(?1, c, ?3) "
        "FROM t;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
      break;
    }

    sqlite3_bind_int(stmt, 1, mode);

    switch (mode) {
      case 0:
        sqlite3_bind_int(stmt, 2, errcode);
        sqlite3_bind_int(stmt, 3, (int)blob_len);
        break;
      case 1:
        sqlite3_bind_blob(stmt, 2, blob_ptr, (int)blob_len, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, (int)blob_len);
        break;
      case 2:
        sqlite3_bind_int(stmt, 2, errcode);
        sqlite3_bind_int(stmt, 3, 0);
        break;
      case 3:
        sqlite3_bind_text(stmt, 2, text_ptr, (int)text_len, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, (int)text_len);
        break;
      case 4:
        sqlite3_bind_blob(stmt, 2, utf16_ptr, (int)utf16_len, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, (int)utf16_len);
        break;
      case 5:
      default:
        if ((fi_u8(&in) & 1) != 0) {
          sqlite3_bind_blob(stmt, 2, blob_ptr, (int)blob_len, SQLITE_TRANSIENT);
        } else {
          sqlite3_bind_text(stmt, 2, text_ptr, (int)text_len, SQLITE_TRANSIENT);
        }
        sqlite3_bind_int(stmt, 3, 0);
        break;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      int cols = sqlite3_column_count(stmt);
      for (int i = 0; i < cols; i++) {
        (void)sqlite3_column_type(stmt, i);
        (void)sqlite3_column_bytes(stmt, i);
        (void)sqlite3_column_text(stmt, i);
        (void)sqlite3_column_blob(stmt, i);
      }
    }

    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}