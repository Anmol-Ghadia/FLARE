// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_collation_needed16 at sqlite3.c:155824:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static void fuzz_collation_needed16(void *pArg, sqlite3 *db, int eTextRep, const void *zName) {
  (void)pArg;
  (void)db;
  (void)eTextRep;
  (void)zName;
}

static int read_u32(const uint8_t **data, size_t *size, uint32_t *out) {
  if (*size < 4) return 0;
  *out = ((uint32_t)(*data)[0]) |
         ((uint32_t)(*data)[1] << 8) |
         ((uint32_t)(*data)[2] << 16) |
         ((uint32_t)(*data)[3] << 24);
  *data += 4;
  *size -= 4;
  return 1;
}

static int read_i32(const uint8_t **data, size_t *size, int *out) {
  uint32_t v;
  if (!read_u32(data, size, &v)) return 0;
  *out = (int)v;
  return 1;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

static void make_utf16le_path(const char *src, unsigned char *dst, size_t dst_bytes) {
  size_t i = 0;
  if (dst_bytes < 2) return;
  while (src[i] != '\0' && (i * 2 + 2) < dst_bytes) {
    dst[i * 2] = (unsigned char)src[i];
    dst[i * 2 + 1] = 0;
    i++;
  }
  dst[i * 2] = 0;
  dst[i * 2 + 1] = 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  int rc;
  const uint8_t *p = Data;
  size_t remaining = Size;
  int mode = 0;
  int wal_n = 0;
  int toggle = 0;
  int lookaside_sz = 0;
  int lookaside_cnt = 0;
  int op_choice = 0;
  int outint = 0;
  unsigned char utf16_path[64];
  unsigned char utf16_main_name[64];
  char main_name[24];
  char *errmsg = NULL;
  void *lookaside_buf = NULL;
  const void *filename16 = NULL;
  sqlite3_stmt *stmt = NULL;

  write_dummy_file(Data, Size);

  if (remaining > 0) {
    mode = *p++;
    remaining--;
  }
  if (!read_i32(&p, &remaining, &wal_n)) wal_n = 0;
  if (!read_i32(&p, &remaining, &toggle)) toggle = 0;
  if (!read_i32(&p, &remaining, &lookaside_sz)) lookaside_sz = 0;
  if (!read_i32(&p, &remaining, &lookaside_cnt)) lookaside_cnt = 0;
  if (!read_i32(&p, &remaining, &op_choice)) op_choice = 0;

  memset(utf16_path, 0, sizeof(utf16_path));
  memset(utf16_main_name, 0, sizeof(utf16_main_name));
  memset(main_name, 0, sizeof(main_name));

  if ((mode & 1) == 0) {
    make_utf16le_path("./dummy_file", utf16_path, sizeof(utf16_path));
    filename16 = utf16_path;
  } else if ((mode & 2) == 0) {
    make_utf16le_path("", utf16_path, sizeof(utf16_path));
    filename16 = utf16_path;
  } else {
    filename16 = NULL;
  }

  rc = sqlite3_open16(filename16, &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      (void)sqlite3_errcode(db);
      (void)sqlite3_extended_errcode(db);
      sqlite3_close(db);
    }
    return 0;
  }

  (void)sqlite3_collation_needed16(db, NULL, fuzz_collation_needed16);
  (void)sqlite3_wal_autocheckpoint(db, wal_n);

  switch ((unsigned)op_choice % 5U) {
    case 0:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, toggle & 1, &outint);
      break;
    case 1:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, toggle & 1, &outint);
      break;
    case 2:
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, toggle & 1, &outint);
      break;
    case 3:
      snprintf(main_name, sizeof(main_name), "m%x", (unsigned)(toggle & 0xffff));
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, main_name);
      break;
    case 4: {
      if (lookaside_sz < 0) lookaside_sz = -lookaside_sz;
      if (lookaside_cnt < 0) lookaside_cnt = -lookaside_cnt;
      lookaside_sz %= 4096;
      lookaside_cnt %= 1024;
      if (lookaside_sz > 0 && lookaside_cnt > 0) {
        size_t total = (size_t)lookaside_sz * (size_t)lookaside_cnt;
        if (total > 0 && total <= (1U << 20)) {
          lookaside_buf = malloc(total);
        }
      }
      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE,
                              lookaside_buf, lookaside_sz, lookaside_cnt);
      break;
    }
  }

  (void)sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  if (remaining > 0) {
    char *sql = (char *)malloc(remaining + 1);
    if (sql) {
      memcpy(sql, p, remaining);
      sql[remaining] = '\0';

      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        int step_rc = sqlite3_step(stmt);
        if (step_rc != SQLITE_ROW && step_rc != SQLITE_DONE) {
          (void)sqlite3_errcode(db);
          (void)sqlite3_extended_errcode(db);
        }
      } else {
        (void)sqlite3_errcode(db);
        (void)sqlite3_extended_errcode(db);
      }

      if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
      }

      rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
      if (rc != SQLITE_OK) {
        (void)sqlite3_errcode(db);
        (void)sqlite3_extended_errcode(db);
      }
      if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
      }
      free(sql);
    }
  } else {
    rc = sqlite3_exec(db, "SELECT * FROM no_such_table;", NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
      (void)sqlite3_errcode(db);
      (void)sqlite3_extended_errcode(db);
    }
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }
  }

  (void)sqlite3_errcode(db);
  (void)sqlite3_extended_errcode(db);

  free(lookaside_buf);
  sqlite3_close(db);
  return 0;
}