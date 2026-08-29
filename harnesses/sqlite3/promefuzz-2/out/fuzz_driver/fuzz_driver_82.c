// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_flag(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = (**p) & 1;
  (*p)++;
  (*n)--;
  return v;
}

static char *make_cstring(const uint8_t *data, size_t size) {
  char *s = (char *)malloc(size + 1);
  if (!s) return NULL;
  memcpy(s, data, size);
  s[size] = '\0';
  return s;
}

static void bind_some_values(sqlite3_stmt *stmt, const uint8_t *data, size_t size) {
  int nparam, i;
  size_t off = 0;

  if (!stmt) return;
  nparam = sqlite3_bind_parameter_count(stmt);
  for (i = 1; i <= nparam; ++i) {
    int choice = 0;
    if (off < size) choice = data[off++] % 5;
    switch (choice) {
      case 0:
        sqlite3_bind_null(stmt, i);
        break;
      case 1: {
        sqlite3_int64 v = 0;
        for (int j = 0; j < 8 && off < size; ++j) {
          v = (v << 8) | data[off++];
        }
        sqlite3_bind_int64(stmt, i, v);
        break;
      }
      case 2: {
        double d = 0.0;
        sqlite3_int64 bits = 0;
        for (int j = 0; j < 8 && off < size; ++j) {
          bits = (bits << 8) | data[off++];
        }
        memcpy(&d, &bits, sizeof(d));
        sqlite3_bind_double(stmt, i, d);
        break;
      }
      case 3: {
        int len = 0;
        if (off < size) len = data[off++] % 32;
        if ((size - off) < (size_t)len) len = (int)(size - off);
        sqlite3_bind_text(stmt, i, (const char *)(data + off), len, SQLITE_TRANSIENT);
        off += len;
        break;
      }
      case 4: {
        int len = 0;
        if (off < size) len = data[off++] % 32;
        if ((size - off) < (size_t)len) len = (int)(size - off);
        sqlite3_bind_blob(stmt, i, data + off, len, SQLITE_TRANSIENT);
        off += len;
        break;
      }
    }
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  const char *tail1 = NULL;
  const char *tail2 = NULL;
  char *sql1 = NULL;
  char *sql2 = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "PRAGMA journal_mode=OFF;"
               "PRAGMA synchronous=OFF;"
               "CREATE TABLE IF NOT EXISTS t(a,b,c);"
               "INSERT INTO t(a,b,c) VALUES(1,'x',X'00');"
               "CREATE TABLE IF NOT EXISTS u(x,y);",
               NULL, NULL, NULL);

  {
    uint32_t split1 = n ? (read_u32(&p, &n) % (n + 1)) : 0;
    uint32_t split2 = n ? (read_u32(&p, &n) % (n + 1)) : 0;
    int nbyte1_mode = read_flag(&p, &n);
    int nbyte2_mode = read_flag(&p, &n);

    if (split1 > n) split1 = (uint32_t)n;
    sql1 = make_cstring(p, split1);
    if (!sql1) goto cleanup;
    p += split1;
    n -= split1;

    if (split2 > n) split2 = (uint32_t)n;
    sql2 = make_cstring(p, split2);
    if (!sql2) goto cleanup;

    {
      int nbyte1;
      int nbyte2;

      if (nbyte1_mode == 0) {
        nbyte1 = -1;
      } else if (nbyte1_mode == 1) {
        nbyte1 = 0;
      } else {
        nbyte1 = (int)split1;
      }

      if (nbyte2_mode == 0) {
        nbyte2 = -1;
      } else if (nbyte2_mode == 1) {
        nbyte2 = 0;
      } else {
        nbyte2 = (int)split2;
      }

      sqlite3_prepare(db, sql1, nbyte1, &stmt1, &tail1);
      sqlite3_prepare(db, sql2, nbyte2, &stmt2, &tail2);
    }
  }

  sqlite3_stmt_busy(NULL);
  sqlite3_expired(NULL);

  if (stmt1) {
    int idx1 = sqlite3_bind_parameter_index(stmt1, ":a");
    int idx2 = sqlite3_bind_parameter_index(stmt1, "@a");
    int idx3 = sqlite3_bind_parameter_index(stmt1, "$a");
    int idx4 = sqlite3_bind_parameter_index(stmt1, "?1");
    (void)idx1;
    (void)idx2;
    (void)idx3;
    (void)idx4;

    if (n > 0) {
      int name_len = (int)(p[0] % (n > 1 ? (n - 1) : 1));
      if (name_len > 0 && n > 1) {
        char *name = make_cstring(p + 1, (size_t)name_len);
        if (name) {
          sqlite3_bind_parameter_index(stmt1, name);
          free(name);
        }
      }
    }

    bind_some_values(stmt1, p, n);

    sqlite3_stmt_busy(stmt1);
    sqlite3_expired(stmt1);

    rc = sqlite3_step(stmt1);
    (void)rc;
    sqlite3_stmt_busy(stmt1);
    sqlite3_expired(stmt1);

    if (read_flag(&p, &n)) {
      rc = sqlite3_step(stmt1);
      (void)rc;
      sqlite3_stmt_busy(stmt1);
      sqlite3_expired(stmt1);
    }

    if (read_flag(&p, &n)) {
      sqlite3_reset(stmt1);
      sqlite3_stmt_busy(stmt1);
      sqlite3_expired(stmt1);
    }

    sqlite3_clear_bindings(stmt1);
    sqlite3_stmt_busy(stmt1);
    sqlite3_expired(stmt1);
  }

  if (stmt2) {
    sqlite3_bind_parameter_index(stmt2, ":a");
    sqlite3_bind_parameter_index(stmt2, "@a");
    sqlite3_bind_parameter_index(stmt2, "$a");
    sqlite3_bind_parameter_index(stmt2, "?1");

    bind_some_values(stmt2, Data, Size);

    sqlite3_stmt_busy(stmt2);
    sqlite3_expired(stmt2);

    if (read_flag(&p, &n)) {
      rc = sqlite3_step(stmt2);
      (void)rc;
      sqlite3_stmt_busy(stmt2);
      sqlite3_expired(stmt2);
    }

    if (read_flag(&p, &n)) {
      sqlite3_reset(stmt2);
      sqlite3_clear_bindings(stmt2);
    }
  }

  if (stmt1 && stmt2) {
    sqlite3_transfer_bindings(stmt1, stmt2);
    sqlite3_stmt_busy(stmt1);
    sqlite3_stmt_busy(stmt2);
    sqlite3_expired(stmt1);
    sqlite3_expired(stmt2);

    sqlite3_transfer_bindings(stmt2, stmt1);
    sqlite3_stmt_busy(stmt1);
    sqlite3_stmt_busy(stmt2);
    sqlite3_expired(stmt1);
    sqlite3_expired(stmt2);
  }

cleanup:
  if (stmt1) sqlite3_finalize(stmt1);
  if (stmt2) sqlite3_finalize(stmt2);
  free(sql1);
  free(sql2);
  sqlite3_close(db);
  return 0;
}