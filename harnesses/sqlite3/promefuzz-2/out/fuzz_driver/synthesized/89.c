// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
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

static int fuzz_collation_cmp(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  (void)pArg;
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  int n = n1 < n2 ? n1 : n2;
  for (int i = 0; i < n; i++) {
    if (a[i] != b[i]) return (int)a[i] - (int)b[i];
  }
  return n1 - n2;
}

static void fuzz_collation_destroy(void *pArg) {
  free(pArg);
}

static void fuzz_scalar_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int sum = argc;
  for (int i = 0; i < argc; i++) {
    if (argv[i]) {
      const unsigned char *txt = sqlite3_value_text(argv[i]);
      if (txt) {
        sum += (int)strlen((const char *)txt);
      } else {
        sum += sqlite3_value_bytes(argv[i]);
      }
    }
  }
  sqlite3_result_int(ctx, sum);
}

static void fuzz_step_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, sizeof(int));
  if (!acc) return;
  *acc += argc;
  for (int i = 0; i < argc; i++) {
    if (argv[i]) {
      *acc += sqlite3_value_bytes(argv[i]);
    }
  }
}

static void fuzz_final_func(sqlite3_context *ctx) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int(ctx, acc ? *acc : 0);
}

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static char *take_cstring(const uint8_t **data, size_t *size, size_t max_len) {
  size_t len = *size ? ((*data)[0] % (max_len + 1)) : 0;
  if (*size) {
    (*data)++;
    (*size)--;
  }
  if (len > *size) len = *size;
  char *out = (char *)malloc(len + 1);
  if (!out) return NULL;
  if (len) memcpy(out, *data, len);
  out[len] = '\0';
  *data += len;
  *size -= len;
  return out;
}

int LLVMFuzzerTestOneInput_89(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  char *s1 = take_cstring(&p, &remaining, 255);
  char *s2 = take_cstring(&p, &remaining, 255);
  char *pattern = take_cstring(&p, &remaining, 255);
  char *text = take_cstring(&p, &remaining, 255);
  char *collname = take_cstring(&p, &remaining, 64);
  char *funcname = take_cstring(&p, &remaining, 64);
  char *logfmt = take_cstring(&p, &remaining, 128);

  if (!s1 || !s2 || !pattern || !text || !collname || !funcname || !logfmt) {
    free(s1); free(s2); free(pattern); free(text); free(collname); free(funcname); free(logfmt);
    return 0;
  }

  const char *a1 = (remaining && (p[0] & 1)) ? NULL : s1;
  if (remaining) { p++; remaining--; }
  const char *a2 = (remaining && (p[0] & 1)) ? NULL : s2;
  if (remaining) { p++; remaining--; }

  (void)sqlite3_stricmp(a1, a2);
  (void)sqlite3_stricmp(s1, s2);
  (void)sqlite3_stricmp("", s1);

  unsigned int esc = remaining ? (unsigned int)p[0] : 0;
  if (remaining) { p++; remaining--; }
  (void)sqlite3_strlike(pattern, text, esc);
  (void)sqlite3_strlike("%", text, 0);
  (void)sqlite3_strlike(pattern, "", '\\');

  int errcode = (int)(read_u32(&p, &remaining) % 512);
  sqlite3_log(errcode, "%s", logfmt);
  sqlite3_log(SQLITE_OK, "cmp=%d like=%d", sqlite3_stricmp(s1, s2), sqlite3_strlike(pattern, text, esc));

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) == SQLITE_OK && db) {
    int encodings[] = {
      SQLITE_UTF8,
      SQLITE_UTF16LE,
      SQLITE_UTF16BE,
      SQLITE_UTF16,
      SQLITE_UTF16_ALIGNED
    };

    int enc1 = encodings[remaining ? (p[0] % (sizeof(encodings)/sizeof(encodings[0]))) : 0];
    if (remaining) { p++; remaining--; }
    int enc2 = encodings[remaining ? (p[0] % (sizeof(encodings)/sizeof(encodings[0]))) : 0];
    if (remaining) { p++; remaining--; }

    void *coll_arg = malloc(1);
    if (coll_arg) {
      int rc = sqlite3_create_collation_v2(db, collname, enc1, coll_arg,
                                           fuzz_collation_cmp, fuzz_collation_destroy);
      if (rc != SQLITE_OK) {
        free(coll_arg);
      }
    }

    (void)sqlite3_create_collation(db, collname, enc2, NULL, fuzz_collation_cmp);
    (void)sqlite3_create_collation(db, collname, enc1, NULL, NULL);

    int nArgChoices[] = { -1, 0, 1, 2, 3, 4 };
    int nArg = nArgChoices[remaining ? (p[0] % (sizeof(nArgChoices)/sizeof(nArgChoices[0]))) : 0];
    if (remaining) { p++; remaining--; }

    int funcEnc = SQLITE_UTF8;
    if (remaining) {
      switch (p[0] % 4) {
        case 0: funcEnc = SQLITE_UTF8; break;
        case 1: funcEnc = SQLITE_UTF16LE; break;
        case 2: funcEnc = SQLITE_UTF16BE; break;
        default: funcEnc = SQLITE_UTF16; break;
      }
      p++;
      remaining--;
    }
    if (remaining && (p[0] & 1)) funcEnc |= SQLITE_DETERMINISTIC;
    if (remaining) { p++; remaining--; }
#ifdef SQLITE_DIRECTONLY
    if (remaining && (p[0] & 1)) funcEnc |= SQLITE_DIRECTONLY;
    if (remaining) { p++; remaining--; }
#endif

    (void)sqlite3_create_function(db, funcname, nArg, funcEnc, NULL,
                                  fuzz_scalar_func, NULL, NULL);
    (void)sqlite3_create_function(db, funcname, nArg, funcEnc, NULL,
                                  NULL, fuzz_step_func, fuzz_final_func);
    (void)sqlite3_create_function(db, funcname, nArg, funcEnc, NULL,
                                  NULL, NULL, NULL);

    sqlite3_close(db);
  }

  free(s1);
  free(s2);
  free(pattern);
  free(text);
  free(collname);
  free(funcname);
  free(logfmt);
  return 0;
}