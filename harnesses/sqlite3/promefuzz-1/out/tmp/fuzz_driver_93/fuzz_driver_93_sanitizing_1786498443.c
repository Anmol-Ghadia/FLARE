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

static char *make_sqlite_owned_string(const uint8_t *Data, size_t Size){
  char *tmp = dup_bytes_as_cstr(Data, Size);
  char *out;
  if (!tmp) return NULL;
  out = sqlite3_mprintf("%s", tmp);
  free(tmp);
  return out;
}

static char *wrap_vmprintf_s(const char *fmt, const char *s){
  va_list ap;
  char *res;
  va_start(ap, s);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_d(const char *fmt, int v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_u(const char *fmt, unsigned v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_x(const char *fmt, unsigned v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_ll(const char *fmt, sqlite3_int64 v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_f(const char *fmt, double v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_sd(const char *fmt, const char *s, int v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vmprintf_prec_s(const char *fmt, int prec, const char *s){
  va_list ap;
  char *res;
  va_start(ap, s);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vsnprintf_s(int n, char *buf, const char *fmt, const char *s){
  va_list ap;
  char *res;
  va_start(ap, s);
  res = sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vsnprintf_d(int n, char *buf, const char *fmt, int v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vsnprintf_sd(int n, char *buf, const char *fmt, const char *s, int v){
  va_list ap;
  char *res;
  va_start(ap, v);
  res = sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vsnprintf_prec_s(int n, char *buf, const char *fmt, int prec, const char *s){
  va_list ap;
  char *res;
  va_start(ap, s);
  res = sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
  return res;
}

static void wrap_log_s(int err, const char *fmt, const char *s){
  sqlite3_log(err, fmt, s);
}

static void wrap_log_d(int err, const char *fmt, int v){
  sqlite3_log(err, fmt, v);
}

static void wrap_log_u(int err, const char *fmt, unsigned v){
  sqlite3_log(err, fmt, v);
}

static void wrap_log_x(int err, const char *fmt, unsigned v){
  sqlite3_log(err, fmt, v);
}

static void wrap_log_ll(int err, const char *fmt, sqlite3_int64 v){
  sqlite3_log(err, fmt, v);
}

static void wrap_log_f(int err, const char *fmt, double v){
  sqlite3_log(err, fmt, v);
}

static void wrap_log_sd(int err, const char *fmt, const char *s, int v){
  sqlite3_log(err, fmt, s, v);
}

static void wrap_log_prec_s(int err, const char *fmt, int prec, const char *s){
  sqlite3_log(err, fmt, prec, s);
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

  sqlite_owned = make_sqlite_owned_string(Data, Size);

  n1 = pick_int(Data, Size, &off);
  n2 = pick_int(Data, Size, &off);
  errcode = pick_int(Data, Size, &off);
  u1 = (unsigned)pick_int(Data, Size, &off);
  u2 = (unsigned)pick_int(Data, Size, &off);
  ll1 = (sqlite3_int64)pick_int(Data, Size, &off) * 1234567;
  ll2 = (sqlite3_int64)pick_int(Data, Size, &off) * -7654321;
  d1 = (double)n1 / 3.0;
  d2 = (double)n2 / 7.0;
  prec1 = n1 < 0 ? -(n1 % 128) : (n1 % 128);
  prec2 = n2 < 0 ? -(n2 % 32) : (n2 % 32);

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

  v = wrap_vmprintf_s("%s", input);
  sqlite3_free(v);

  v = wrap_vmprintf_d("%d", n1);
  sqlite3_free(v);

  v = wrap_vmprintf_u("%u", u1);
  sqlite3_free(v);

  v = wrap_vmprintf_x("%x", u2);
  sqlite3_free(v);

  v = wrap_vmprintf_ll("%lld", ll2);
  sqlite3_free(v);

  v = wrap_vmprintf_f("%f", d2);
  sqlite3_free(v);

  v = wrap_vmprintf_sd("%s:%d", input, n2);
  sqlite3_free(v);

  v = wrap_vmprintf_s("%q", input);
  sqlite3_free(v);

  v = wrap_vmprintf_s("%Q", input);
  sqlite3_free(v);

  v = wrap_vmprintf_s("%w", input);
  sqlite3_free(v);

  v = wrap_vmprintf_prec_s("%.*s", prec2, input);
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

  wrap_vsnprintf_s((int)sizeof(stackbuf1), stackbuf1, "%s", input);
  wrap_vsnprintf_d((int)sizeof(stackbuf1), stackbuf1, "%d", n2);
  wrap_vsnprintf_sd((int)sizeof(stackbuf1), stackbuf1, "%s:%d", input, n1);
  wrap_vsnprintf_prec_s((int)sizeof(stackbuf2), stackbuf2, "%.*s", prec2, input);

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
    finished = sqlite3_str_finish(acc);
    acc = NULL;
    sqlite3_free(finished);
    finished = NULL;
  }

  wrap_log_s(errcode, "%s", input);
  wrap_log_d(errcode ^ 0x11, "%d", n1);
  wrap_log_u(errcode ^ 0x22, "%u", u1);
  wrap_log_x(errcode ^ 0x33, "%x", u2);
  wrap_log_ll(errcode ^ 0x44, "%lld", ll2);
  wrap_log_f(errcode ^ 0x55, "%f", d2);
  wrap_log_sd(errcode ^ 0x66, "%s:%d", input, n2);
  wrap_log_s(errcode ^ 0x77, "%q", input);
  wrap_log_s(errcode ^ 0x88, "%Q", input);
  wrap_log_s(errcode ^ 0x99, "%w", input);
  wrap_log_prec_s(errcode ^ 0xaa, "%.*s", prec2, input);

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