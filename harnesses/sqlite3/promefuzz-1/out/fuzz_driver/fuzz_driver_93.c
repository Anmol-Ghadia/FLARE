// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_vsnprintf at sqlite3.c:17488:18 in sqlite3.h
// sqlite3_vsnprintf at sqlite3.c:17488:18 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static int pick_int(const uint8_t *Data, size_t Size, size_t *off){
  if (*off + 4 <= Size){
    int v = (int)rd32(Data + *off);
    *off += 4;
    return v;
  }
  if (*off < Size){
    return (int)Data[(*off)++];
  }
  return 0;
}

static char *dup_bytes_as_cstr(const uint8_t *Data, size_t Size){
  char *s = (char *)malloc(Size + 1);
  if (!s) return NULL;
  if (Size) memcpy(s, Data, Size);
  s[Size] = '\0';
  return s;
}

static char *call_vmprintf(const char *fmt, ...){
  va_list ap;
  char *res;
  va_start(ap, fmt);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *call_vsnprintf(int n, char *buf, const char *fmt, ...){
  va_list ap;
  char *res;
  va_start(ap, fmt);
  res = sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
  return res;
}

static void call_log(int err, const char *fmt, ...){
  char stackbuf[512];
  va_list ap;
  va_start(ap, fmt);
  sqlite3_vsnprintf((int)sizeof(stackbuf), stackbuf, fmt, ap);
  va_end(ap);
  sqlite3_log(err, "%s", stackbuf);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_str *acc = NULL;
  char *input = NULL;
  char *sqlite_owned = NULL;
  char *m = NULL;
  char *v = NULL;
  char *finished = NULL;
  char stackbuf1[256];
  char stackbuf2[32];
  int n1, n2, errcode, prec1, prec2;
  unsigned u1, u2;
  sqlite3_int64 ll1, ll2;
  double d1, d2;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp){
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3_initialize();
  sqlite3_open(":memory:", &db);

  input = dup_bytes_as_cstr(Data, Size);
  if (!input) goto cleanup;

  sqlite_owned = sqlite3_mprintf("%s", input);

  n1 = pick_int(Data, Size, &off);
  n2 = pick_int(Data, Size, &off);
  errcode = pick_int(Data, Size, &off);
  u1 = (unsigned)pick_int(Data, Size, &off);
  u2 = (unsigned)pick_int(Data, Size, &off);
  ll1 = (sqlite3_int64)pick_int(Data, Size, &off) * 1234567;
  ll2 = (sqlite3_int64)pick_int(Data, Size, &off) * -7654321;
  d1 = (double)n1 / 3.0;
  d2 = (double)n2 / 7.0;
  prec1 = n1 % 128;
  if (prec1 < 0) prec1 = -prec1;
  prec2 = n2 % 32;
  if (prec2 < 0) prec2 = -prec2;

  m = sqlite3_mprintf("%s", input);
  sqlite3_free(m);
  m = sqlite3_mprintf("%d", n1);
  sqlite3_free(m);
  m = sqlite3_mprintf("%u", u1);
  sqlite3_free(m);
  m = sqlite3_mprintf("%x", u2);
  sqlite3_free(m);
  m = sqlite3_mprintf("%lld", ll1);
  sqlite3_free(m);
  m = sqlite3_mprintf("%f", d1);
  sqlite3_free(m);
  m = sqlite3_mprintf("%s:%d", input, n2);
  sqlite3_free(m);
  m = sqlite3_mprintf("%q", input);
  sqlite3_free(m);
  m = sqlite3_mprintf("%Q", input);
  sqlite3_free(m);
  m = sqlite3_mprintf("%w", input);
  sqlite3_free(m);
  if (sqlite_owned){
    m = sqlite3_mprintf("%z", sqlite_owned);
    sqlite_owned = NULL;
    sqlite3_free(m);
  }
  m = sqlite3_mprintf("%.*s", prec1, input);
  sqlite3_free(m);
  m = sqlite3_mprintf("%% %d %s %Q", n1, input, input);
  sqlite3_free(m);

  v = call_vmprintf("%s", input);
  sqlite3_free(v);
  v = call_vmprintf("%d", n1);
  sqlite3_free(v);
  v = call_vmprintf("%u", u1);
  sqlite3_free(v);
  v = call_vmprintf("%x", u2);
  sqlite3_free(v);
  v = call_vmprintf("%lld", ll2);
  sqlite3_free(v);
  v = call_vmprintf("%f", d2);
  sqlite3_free(v);
  v = call_vmprintf("%s:%d", input, n2);
  sqlite3_free(v);
  v = call_vmprintf("%q", input);
  sqlite3_free(v);
  v = call_vmprintf("%Q", input);
  sqlite3_free(v);
  v = call_vmprintf("%w", input);
  sqlite3_free(v);
  v = call_vmprintf("%.*s", prec2, input);
  sqlite3_free(v);

  memset(stackbuf1, 0x41, sizeof(stackbuf1));
  memset(stackbuf2, 0x42, sizeof(stackbuf2));

  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%s", input);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%d", n1);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%u", u1);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%x", u2);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%lld", ll1);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%f", d1);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%s:%d", input, n2);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%q", input);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%Q", input);
  sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, "%w", input);
  sqlite3_snprintf((int)sizeof(stackbuf2), stackbuf2, "%.*s", prec2, input);

  call_vsnprintf((int)sizeof(stackbuf1), stackbuf1, "%s", input);
  call_vsnprintf((int)sizeof(stackbuf1), stackbuf1, "%d", n2);
  call_vsnprintf((int)sizeof(stackbuf1), stackbuf1, "%s:%d", input, n1);
  call_vsnprintf((int)sizeof(stackbuf2), stackbuf2, "%.*s", prec2, input);

  acc = sqlite3_str_new(db);
  if (acc){
    sqlite3_str_appendall(acc, input);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%d", n1);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%u", u1);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%Q", input);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%w", input);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%.*s", prec1, input);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%s:%d", input, n2);
    (void)sqlite3_str_errcode(acc);
    finished = sqlite3_str_finish(acc);
    acc = NULL;
    sqlite3_free(finished);
    finished = NULL;
  }

  call_log(errcode, "%s", input);
  call_log(errcode ^ 0x11, "%d", n1);
  call_log(errcode ^ 0x22, "%u", u1);
  call_log(errcode ^ 0x33, "%x", u2);
  call_log(errcode ^ 0x44, "%lld", ll2);
  call_log(errcode ^ 0x55, "%f", d2);
  call_log(errcode ^ 0x66, "%s:%d", input, n2);
  call_log(errcode ^ 0x77, "%q", input);
  call_log(errcode ^ 0x88, "%Q", input);
  call_log(errcode ^ 0x99, "%w", input);
  call_log(errcode ^ 0xaa, "%.*s", prec2, input);

cleanup:
  if (acc){
    finished = sqlite3_str_finish(acc);
    sqlite3_free(finished);
  }
  sqlite3_free(sqlite_owned);
  free(input);
  if (db) sqlite3_close(db);
  return 0;
}