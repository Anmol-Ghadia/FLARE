// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static int has_nul_within(const uint8_t *p, int n, int unit) {
  int i;
  if (!p || n <= 0) return 0;
  if (unit <= 1) {
    for (i = 0; i < n; i++) {
      if (p[i] == 0) return 1;
    }
    return 0;
  }
  for (i = 0; i + 1 < n; i += 2) {
    if (p[i] == 0 && p[i + 1] == 0) return 1;
  }
  return 0;
}

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  const uint8_t *data = NULL;
  int size = 0;
  int mode = 0;
  int avail = 0;
  int n = 0;
  const char *p8;
  const void *p16;

  (void)argc;
  if (!ctx) return;

  if (argv && argc > 0 && argv[0]) {
    data = (const uint8_t *)sqlite3_value_blob(argv[0]);
    size = sqlite3_value_bytes(argv[0]);
  }

  if (!data || size <= 0) {
    sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
    return;
  }

  mode = data[0] % 6;
  avail = size > 2 ? size - 2 : 0;
  p8 = (const char *)(size > 2 ? data + 2 : data + size);
  p16 = (const void *)(size > 2 ? data + 2 : data + size);

  if (size > 1) {
    n = (int)data[1];
    if ((data[0] & 0x80) != 0) {
      n = -1;
    } else if (n > avail) {
      n = avail;
    }
  } else {
    n = 0;
  }

  switch (mode) {
    case 0:
      if (n < 0) {
        if (has_nul_within((const uint8_t *)p8, avail, 1)) {
          sqlite3_result_text(ctx, p8, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_text(ctx, p8, avail, SQLITE_TRANSIENT);
        }
      } else {
        sqlite3_result_text(ctx, p8, n, SQLITE_TRANSIENT);
      }
      break;
    case 1:
      if (n < 0) {
        int even_avail = avail & ~1;
        if (has_nul_within((const uint8_t *)p16, even_avail, 2)) {
          sqlite3_result_text16(ctx, p16, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_text16(ctx, p16, even_avail, SQLITE_TRANSIENT);
        }
      } else {
        sqlite3_result_text16(ctx, p16, n & ~1, SQLITE_TRANSIENT);
      }
      break;
    case 2:
      if (n < 0) {
        int even_avail = avail & ~1;
        if (has_nul_within((const uint8_t *)p16, even_avail, 2)) {
          sqlite3_result_text16le(ctx, p16, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_text16le(ctx, p16, even_avail, SQLITE_TRANSIENT);
        }
      } else {
        sqlite3_result_text16le(ctx, p16, n & ~1, SQLITE_TRANSIENT);
      }
      break;
    case 3:
      if (n < 0) {
        int even_avail = avail & ~1;
        if (has_nul_within((const uint8_t *)p16, even_avail, 2)) {
          sqlite3_result_text16be(ctx, p16, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_text16be(ctx, p16, even_avail, SQLITE_TRANSIENT);
        }
      } else {
        sqlite3_result_text16be(ctx, p16, n & ~1, SQLITE_TRANSIENT);
      }
      break;
    case 4:
      sqlite3_result_blob(ctx, p16, avail >= 0 ? avail : 0, SQLITE_TRANSIENT);
      break;
    case 5:
    default:
      if (n < 0) {
        if (has_nul_within((const uint8_t *)p8, avail, 1)) {
          sqlite3_result_error(ctx, p8, -1);
        } else {
          sqlite3_result_error(ctx, p8, avail);
        }
      } else {
        sqlite3_result_error(ctx, p8, n);
      }
      break;
  }

  if (size > 4) {
    int mode2 = data[size - 1] % 6;
    int n2 = (data[size - 2] & 1) ? -1 : ((avail < 16) ? avail : 16);
    switch (mode2) {
      case 0:
        if (n2 < 0) {
          if (has_nul_within((const uint8_t *)p8, avail, 1)) {
            sqlite3_result_text(ctx, p8, -1, SQLITE_STATIC);
          } else {
            sqlite3_result_text(ctx, p8, avail, SQLITE_STATIC);
          }
        } else {
          sqlite3_result_text(ctx, p8, n2, SQLITE_STATIC);
        }
        break;
      case 1: {
        int even_avail = avail & ~1;
        int even_n2 = n2 < 0 ? -1 : (n2 & ~1);
        if (even_n2 < 0) {
          if (has_nul_within((const uint8_t *)p16, even_avail, 2)) {
            sqlite3_result_text16(ctx, p16, -1, SQLITE_STATIC);
          } else {
            sqlite3_result_text16(ctx, p16, even_avail, SQLITE_STATIC);
          }
        } else {
          sqlite3_result_text16(ctx, p16, even_n2, SQLITE_STATIC);
        }
        break;
      }
      case 2: {
        int even_avail = avail & ~1;
        int even_n2 = n2 < 0 ? -1 : (n2 & ~1);
        if (even_n2 < 0) {
          if (has_nul_within((const uint8_t *)p16, even_avail, 2)) {
            sqlite3_result_text16le(ctx, p16, -1, SQLITE_STATIC);
          } else {
            sqlite3_result_text16le(ctx, p16, even_avail, SQLITE_STATIC);
          }
        } else {
          sqlite3_result_text16le(ctx, p16, even_n2, SQLITE_STATIC);
        }
        break;
      }
      case 3: {
        int even_avail = avail & ~1;
        int even_n2 = n2 < 0 ? -1 : (n2 & ~1);
        if (even_n2 < 0) {
          if (has_nul_within((const uint8_t *)p16, even_avail, 2)) {
            sqlite3_result_text16be(ctx, p16, -1, SQLITE_STATIC);
          } else {
            sqlite3_result_text16be(ctx, p16, even_avail, SQLITE_STATIC);
          }
        } else {
          sqlite3_result_text16be(ctx, p16, even_n2, SQLITE_STATIC);
        }
        break;
      }
      case 4:
        sqlite3_result_blob(ctx, p16, avail > 8 ? 8 : avail, SQLITE_STATIC);
        break;
      case 5:
      default:
        if (n2 < 0) {
          if (has_nul_within((const uint8_t *)p8, avail, 1)) {
            sqlite3_result_error(ctx, p8, -1);
          } else {
            sqlite3_result_error(ctx, p8, avail);
          }
        } else {
          sqlite3_result_error(ctx, p8, n2);
        }
        break;
    }
  }
}

int LLVMFuzzerTestOneInput_61(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, fuzz_sql_func, NULL, NULL);

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL) == SQLITE_OK && stmt) {
    (void)sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
    (void)sqlite3_step(stmt);
    (void)sqlite3_finalize(stmt);
    stmt = NULL;
  }

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS TEXT));", -1, &stmt, NULL) == SQLITE_OK && stmt) {
    (void)sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
    (void)sqlite3_step(stmt);
    (void)sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}