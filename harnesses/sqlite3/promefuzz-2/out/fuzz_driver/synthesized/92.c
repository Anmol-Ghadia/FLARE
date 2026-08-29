// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
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

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  sqlite3_int64 sum = 0;
  for (i = 0; i < argc; ++i) {
    sum += sqlite3_value_bytes(argv[i]);
  }
  sqlite3_result_int64(ctx, sum);
}

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *p = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  int i;
  if (!p) return;
  for (i = 0; i < argc; ++i) {
    *p += sqlite3_value_bytes(argv[i]);
  }
}

static void fuzz_final(sqlite3_context *ctx) {
  sqlite3_int64 *p = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int64(ctx, p ? *p : 0);
}

static int consume_u32(const uint8_t **data, size_t *size, uint32_t *out) {
  if (*size < 4) return 0;
  *out = ((uint32_t)(*data)[0]) |
         ((uint32_t)(*data)[1] << 8) |
         ((uint32_t)(*data)[2] << 16) |
         ((uint32_t)(*data)[3] << 24);
  *data += 4;
  *size -= 4;
  return 1;
}

static int consume_i32(const uint8_t **data, size_t *size, int *out) {
  uint32_t v;
  if (!consume_u32(data, size, &v)) return 0;
  *out = (int)v;
  return 1;
}

static char *make_cstring(const uint8_t *data, size_t len) {
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, data, len);
  s[len] = '\0';
  return s;
}

int LLVMFuzzerTestOneInput_92(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  uint32_t split1 = 0, split2 = 0, split3 = 0, split4 = 0;
  size_t len1, len2, len3, len4;
  char *s1 = NULL, *s2 = NULL, *fmt = NULL, *fname = NULL;
  char buf1[256];
  char buf2[32];
  sqlite3 *db = NULL;
  int rc;

  if (!consume_u32(&p, &remaining, &split1)) return 0;
  if (!consume_u32(&p, &remaining, &split2)) return 0;
  if (!consume_u32(&p, &remaining, &split3)) return 0;
  if (!consume_u32(&p, &remaining, &split4)) return 0;

  len1 = remaining ? (split1 % (remaining + 1)) : 0;
  s1 = make_cstring(p, len1);
  if (!s1) return 0;
  p += len1;
  remaining -= len1;

  len2 = remaining ? (split2 % (remaining + 1)) : 0;
  s2 = make_cstring(p, len2);
  if (!s2) {
    free(s1);
    return 0;
  }
  p += len2;
  remaining -= len2;

  len3 = remaining ? (split3 % (remaining + 1)) : 0;
  fmt = make_cstring(p, len3);
  if (!fmt) {
    free(s1);
    free(s2);
    return 0;
  }
  p += len3;
  remaining -= len3;

  len4 = remaining ? (split4 % (remaining + 1)) : 0;
  fname = make_cstring(p, len4);
  if (!fname) {
    free(s1);
    free(s2);
    free(fmt);
    return 0;
  }

  (void)sqlite3_stricmp(s1, s2);
  (void)sqlite3_stricmp(NULL, s1);
  (void)sqlite3_stricmp(s2, NULL);
  (void)sqlite3_stricmp(NULL, NULL);

  (void)sqlite3_strnicmp(s1, s2, (int)(len1 + len2));
  (void)sqlite3_strnicmp(s1, s2, (int)len1);
  (void)sqlite3_strnicmp(NULL, s1, (int)len2);
  (void)sqlite3_strnicmp(s2, NULL, (int)len1);
  (void)sqlite3_strnicmp(NULL, NULL, 0);

  sqlite3_snprintf((int)sizeof(buf1), buf1, "%s", fmt ? fmt : "");
  sqlite3_snprintf((int)sizeof(buf2), buf2, "%d:%u:%s", (int)len1, split1, s1 ? s1 : "");

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc == SQLITE_OK && db != NULL) {
    int nArgChoices[] = { -1, 0, 1, 2, 3, 127 };
    int encChoices[] = {
      SQLITE_UTF8,
      SQLITE_UTF16LE,
      SQLITE_UTF16BE,
      SQLITE_UTF16,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC,
      SQLITE_UTF8 | SQLITE_DIRECTONLY,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
    };
    size_t i;

    for (i = 0; i < sizeof(nArgChoices) / sizeof(nArgChoices[0]); ++i) {
      int nArg = nArgChoices[i];
      int enc = encChoices[(split1 + (uint32_t)i) % (sizeof(encChoices) / sizeof(encChoices[0]))];

      (void)sqlite3_create_function(db, s1, nArg, enc, NULL, fuzz_scalar, NULL, NULL);
      (void)sqlite3_create_function(db, s2, nArg, enc, NULL, NULL, fuzz_step, fuzz_final);
      (void)sqlite3_create_function(db, fname, nArg, enc, NULL, NULL, NULL, NULL);
    }
  }

  {
    int idx1 = (int)(split2 % 512U);
    int idx2 = (int)(split3 % 512U);
    int idx3 = -1;
    const char *opt;

    opt = sqlite3_compileoption_get(idx1);
    if (opt) {
      (void)sqlite3_stricmp(opt, s1);
      (void)sqlite3_strnicmp(opt, s2, (int)len2);
    }

    opt = sqlite3_compileoption_get(idx2);
    if (opt) {
      sqlite3_snprintf((int)sizeof(buf1), buf1, "%s", opt);
    }

    (void)sqlite3_compileoption_get(idx3);
  }

#ifdef SQLITE_TESTCTRL_PRNG_RESET
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
#ifdef SQLITE_TESTCTRL_BYTEORDER
  (void)sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
#endif
#ifdef SQLITE_TESTCTRL_ISINIT
  (void)sqlite3_test_control(SQLITE_TESTCTRL_ISINIT);
#endif

  if (db) {
    sqlite3_close(db);
  }
  free(s1);
  free(s2);
  free(fmt);
  free(fname);
  return 0;
}