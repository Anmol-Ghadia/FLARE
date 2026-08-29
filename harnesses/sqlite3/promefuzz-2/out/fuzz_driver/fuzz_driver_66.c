// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_collation_needed16 at sqlite3.c:155824:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation16 at sqlite3.c:155773:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_collation_needed16 at sqlite3.c:155824:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation16 at sqlite3.c:155773:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_int(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static size_t read_sized_chunk(const uint8_t **data, size_t *size, uint8_t *out, size_t max_out) {
  if (*size == 0 || max_out == 0) return 0;
  size_t want = read_u32(data, size);
  size_t n = want % max_out;
  if (n > *size) n = *size;
  memcpy(out, *data, n);
  *data += n;
  *size -= n;
  return n;
}

static char *make_utf8_string(const uint8_t **data, size_t *size) {
  size_t max_len = (*size > 0) ? (*size < 64 ? *size : 64) : 0;
  char *s = (char *)malloc(max_len + 1);
  size_t n;
  if (!s) return NULL;

  n = read_sized_chunk(data, size, (uint8_t *)s, max_len);
  if (n > max_len) n = max_len;

  for (size_t i = 0; i < n; ++i) {
    if (s[i] == '\0') s[i] = 'A';
  }

  if (n == 0) {
    if (max_len == 0) {
      s[0] = '\0';
      return s;
    }
    s[0] = 'X';
    n = 1;
  }

  if (n > max_len) n = max_len;
  s[n] = '\0';
  return s;
}

static void *make_utf16le_name(const uint8_t **data, size_t *size) {
  size_t chars = (*size > 0) ? (read_u32(data, size) % 32) : 0;
  size_t bytes = (chars + 1) * 2;
  unsigned char *buf = (unsigned char *)malloc(bytes);
  if (!buf) return NULL;
  for (size_t i = 0; i < chars; ++i) {
    unsigned char c = (*size > 0) ? *(*data)++ : (unsigned char)('A' + (i % 26));
    if (*size > 0) (*size)--;
    if (c == 0) c = 'B';
    buf[i * 2] = c;
    buf[i * 2 + 1] = 0;
  }
  buf[chars * 2] = 0;
  buf[chars * 2 + 1] = 0;
  return buf;
}

static int fuzz_compare(void *pArg, int len1, const void *a, int len2, const void *b) {
  uint32_t seed = pArg ? *(uint32_t *)pArg : 0;
  const unsigned char *s1 = (const unsigned char *)a;
  const unsigned char *s2 = (const unsigned char *)b;
  int min = len1 < len2 ? len1 : len2;
  for (int i = 0; i < min; ++i) {
    unsigned char c1 = s1 ? (unsigned char)(s1[i] ^ (seed & 0xFF)) : 0;
    unsigned char c2 = s2 ? (unsigned char)(s2[i] ^ ((seed >> 8) & 0xFF)) : 0;
    if (c1 != c2) return (int)c1 - (int)c2;
  }
  if ((seed & 1U) != 0) return len2 - len1;
  return len1 - len2;
}

static void fuzz_destroy(void *p) {
  free(p);
}

static void fuzz_collation_needed16(void *pArg, sqlite3 *db, int eTextRep, const void *zName16) {
  (void)zName16;
  uint32_t *ctx = (uint32_t *)malloc(sizeof(uint32_t));
  if (!ctx) return;
  *ctx = pArg ? *(uint32_t *)pArg : 0x12345678U;
  sqlite3_create_collation_v2(db, "needed16_auto", eTextRep, ctx, fuzz_compare, fuzz_destroy);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close_v2(db);
    return 0;
  }

  const uint8_t *p = Data;
  size_t remaining = Size;

  uint32_t *needed_arg = (uint32_t *)malloc(sizeof(uint32_t));
  if (needed_arg) {
    *needed_arg = read_u32(&p, &remaining);
  }
  sqlite3_collation_needed16(db, needed_arg, fuzz_collation_needed16);

  int iterations = 1;
  if (remaining > 0) iterations = (int)(p[0] % 16) + 1;
  if (remaining > 0) {
    p++;
    remaining--;
  }

  for (int i = 0; i < iterations; ++i) {
    int action = read_int(&p, &remaining) % 8;

    if (action == 0) {
      char *name = make_utf8_string(&p, &remaining);
      int encs[] = {SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE, SQLITE_UTF16, SQLITE_UTF16_ALIGNED};
      int enc = encs[read_u32(&p, &remaining) % (sizeof(encs) / sizeof(encs[0]))];
      uint32_t *ctx = (uint32_t *)malloc(sizeof(uint32_t));
      if (ctx) *ctx = read_u32(&p, &remaining);
      int use_null_cmp = read_int(&p, &remaining) & 1;
      int rc = sqlite3_create_collation(db, name ? name : "c", enc, ctx,
                                        use_null_cmp ? NULL : fuzz_compare);
      if (rc != SQLITE_OK && ctx) {
        free(ctx);
      }
      free(name);
    } else if (action == 1) {
      char *name = make_utf8_string(&p, &remaining);
      int encs[] = {SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE, SQLITE_UTF16, SQLITE_UTF16_ALIGNED};
      int enc = encs[read_u32(&p, &remaining) % (sizeof(encs) / sizeof(encs[0]))];
      uint32_t *ctx = (uint32_t *)malloc(sizeof(uint32_t));
      if (ctx) *ctx = read_u32(&p, &remaining);
      int mode = read_int(&p, &remaining) % 3;
      int (*cmp)(void *, int, const void *, int, const void *) = (mode == 0) ? NULL : fuzz_compare;
      int rc = sqlite3_create_collation_v2(db, name ? name : "v2c", enc, ctx, cmp, fuzz_destroy);
      if (rc != SQLITE_OK && ctx) {
        free(ctx);
      }
      free(name);
    } else if (action == 2) {
      void *name16 = make_utf16le_name(&p, &remaining);
      int encs[] = {SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE, SQLITE_UTF16, SQLITE_UTF16_ALIGNED};
      int enc = encs[read_u32(&p, &remaining) % (sizeof(encs) / sizeof(encs[0]))];
      uint32_t *ctx = (uint32_t *)malloc(sizeof(uint32_t));
      if (ctx) *ctx = read_u32(&p, &remaining);
      int use_null_cmp = read_int(&p, &remaining) & 1;
      int rc = sqlite3_create_collation16(db, name16, enc, ctx,
                                          use_null_cmp ? NULL : fuzz_compare);
      if (rc != SQLITE_OK && ctx) {
        free(ctx);
      }
      free(name16);
    } else if (action == 3) {
      int op_choices[] = {
#ifdef SQLITE_DBCONFIG_LOOKASIDE
        SQLITE_DBCONFIG_LOOKASIDE,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
        SQLITE_DBCONFIG_ENABLE_FKEY,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
        SQLITE_DBCONFIG_ENABLE_TRIGGER,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
        SQLITE_DBCONFIG_ENABLE_VIEW,
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_ALTER_TABLE
        SQLITE_DBCONFIG_LEGACY_ALTER_TABLE,
#endif
#ifdef SQLITE_DBCONFIG_DQS_DML
        SQLITE_DBCONFIG_DQS_DML,
#endif
#ifdef SQLITE_DBCONFIG_DQS_DDL
        SQLITE_DBCONFIG_DQS_DDL,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_QPSG
        SQLITE_DBCONFIG_ENABLE_QPSG,
#endif
#ifdef SQLITE_DBCONFIG_DEFENSIVE
        SQLITE_DBCONFIG_DEFENSIVE,
#endif
#ifdef SQLITE_DBCONFIG_WRITABLE_SCHEMA
        SQLITE_DBCONFIG_WRITABLE_SCHEMA,
#endif
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
        SQLITE_DBCONFIG_TRUSTED_SCHEMA,
#endif
#ifdef SQLITE_DBCONFIG_STMT_SCANSTATUS
        SQLITE_DBCONFIG_STMT_SCANSTATUS,
#endif
#ifdef SQLITE_DBCONFIG_REVERSE_SCANORDER
        SQLITE_DBCONFIG_REVERSE_SCANORDER,
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
        SQLITE_DBCONFIG_MAINDBNAME,
#endif
      };
      size_t nops = sizeof(op_choices) / sizeof(op_choices[0]);
      if (nops > 0) {
        int op = op_choices[read_u32(&p, &remaining) % nops];
#ifdef SQLITE_DBCONFIG_LOOKASIDE
        if (op == SQLITE_DBCONFIG_LOOKASIDE) {
          int sz = (int)(read_u32(&p, &remaining) % 4096);
          int cnt = (int)(read_u32(&p, &remaining) % 64);
          void *buf = NULL;
          size_t total = (size_t)sz * (size_t)cnt;
          if (total > 0 && total < (1U << 20)) {
            buf = malloc(total);
          }
          sqlite3_db_config(db, op, buf, sz, cnt);
          free(buf);
        } else
#endif
#ifdef SQLITE_DBCONFIG_MAINDBNAME
        if (op == SQLITE_DBCONFIG_MAINDBNAME) {
          char *name = make_utf8_string(&p, &remaining);
          sqlite3_db_config(db, op, name ? name : "main");
          free(name);
        } else
#endif
        {
          int onoff = read_int(&p, &remaining) & 1;
          int out = 0;
          sqlite3_db_config(db, op, onoff, &out);
        }
      }
    } else if (action == 4) {
      uint32_t *arg = (uint32_t *)malloc(sizeof(uint32_t));
      if (arg) *arg = read_u32(&p, &remaining);
      int set_cb = read_int(&p, &remaining) & 1;
      sqlite3_collation_needed16(db, arg, set_cb ? fuzz_collation_needed16 : NULL);
      if (!set_cb && arg) free(arg);
    } else if (action == 5) {
      char *errmsg = NULL;
      sqlite3_exec(db,
                   "CREATE TABLE IF NOT EXISTS t(x TEXT);"
                   "INSERT INTO t(x) VALUES('a'),('b'),('c');"
                   "SELECT x FROM t ORDER BY x COLLATE needed16_auto;",
                   NULL, NULL, &errmsg);
      sqlite3_free(errmsg);
    } else if (action == 6) {
      char *name = make_utf8_string(&p, &remaining);
      sqlite3_create_collation(db, name ? name : "delc", SQLITE_UTF8, NULL, NULL);
      free(name);
    } else {
      void *name16 = make_utf16le_name(&p, &remaining);
      sqlite3_create_collation16(db, name16, SQLITE_UTF16LE, NULL, NULL);
      free(name16);
    }
  }

  sqlite3_close_v2(db);
  free(needed_arg);
  return 0;
}