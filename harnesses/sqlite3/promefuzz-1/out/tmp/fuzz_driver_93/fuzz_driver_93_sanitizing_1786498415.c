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

static const char *pick_format(unsigned sel){
  static const char *fmts[] = {
    "%s",
    "%d",
    "%u",
    "%x",
    "%lld",
    "%f",
    "%s:%d",
    "%q",
    "%Q",
    "%w",
    "%z",
    "%.*s",
    "%d-%u-%x-%lld-%f-%s",
    "prefix[%s]suffix",
    "%% %d %s %Q"
  };
  return fmts[sel % (sizeof(fmts)/sizeof(fmts[0]))];
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

static char *wrap_vmprintf(const char *fmt, ...){
  va_list ap;
  char *res;
  va_start(ap, fmt);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *wrap_vsnprintf(int n, char *buf, const char *fmt, ...){
  va_list ap;
  char *res;
  va_start(ap, fmt);
  res = sqlite3_vsnprintf(n, buf, fmt, ap);
  va_end(ap);
  return res;
}

static void wrap_log(int err, const char *fmt, ...){
  va_list ap;
  char *tmp;
  va_start(ap, fmt);
  tmp = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  if (tmp){
    sqlite3_log(err, "%s", tmp);
    sqlite3_free(tmp);
  }else{
    sqlite3_log(err, "%s", "");
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_str *acc = NULL;
  char *input = NULL;
  char *sqlite_owned = NULL;
  char *m1 = NULL, *m2 = NULL, *m3 = NULL;
  char *v1 = NULL, *v2 = NULL;
  char *finished = NULL;
  char stackbuf1[256];
  char stackbuf2[8];
  int n1, n2, errcode;
  const char *fmt1, *fmt2, *fmt3;
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
  fmt1 = pick_format((unsigned)pick_int(Data, Size, &off));
  fmt2 = pick_format((unsigned)pick_int(Data, Size, &off));
  fmt3 = pick_format((unsigned)pick_int(Data, Size, &off));
  n1 = pick_int(Data, Size, &off);
  n2 = pick_int(Data, Size, &off);
  errcode = pick_int(Data, Size, &off);

  if (strcmp(fmt1, "%z") == 0){
    char *arg = sqlite_owned ? sqlite_owned : sqlite3_mprintf("%s", input);
    sqlite_owned = NULL;
    m1 = sqlite3_mprintf(fmt1, arg);
  }else{
    m1 = sqlite3_mprintf(fmt1,
                         input,
                         n1,
                         (unsigned)n1,
                         (sqlite3_int64)((int64_t)n1 * 1234567),
                         (double)n1 / 3.0);
  }
  sqlite3_free(m1);

  if (strcmp(fmt2, "%z") == 0){
    char *arg = sqlite3_mprintf("%s", input);
    m2 = sqlite3_mprintf(fmt2, arg);
  }else{
    m2 = sqlite3_mprintf(fmt2,
                         input,
                         n2,
                         (unsigned)n2,
                         (sqlite3_int64)((int64_t)n2 * -7654321),
                         (double)n2 / 7.0);
  }
  sqlite3_free(m2);

  if (sqlite_owned){
    m3 = sqlite3_mprintf("%z", sqlite_owned);
    sqlite_owned = NULL;
    sqlite3_free(m3);
  }

  if (strcmp(fmt3, "%z") == 0){
    char *arg = sqlite3_mprintf("%s", input);
    v1 = wrap_vmprintf(fmt3, arg);
  }else{
    v1 = wrap_vmprintf(fmt3,
                       input,
                       n1,
                       (unsigned)n1,
                       (sqlite3_int64)n2,
                       (double)(n1 ^ n2));
  }
  sqlite3_free(v1);

  v2 = wrap_vmprintf("%.*s",
                     (n1 < 0 ? -(n1 % 1024) : (n1 % 1024)),
                     input);
  sqlite3_free(v2);

  memset(stackbuf1, 0x41, sizeof(stackbuf1));
  memset(stackbuf2, 0x42, sizeof(stackbuf2));

  if (strcmp(fmt1, "%z") == 0){
    char *arg = sqlite3_mprintf("%s", input);
    sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, fmt1, arg);
  }else{
    sqlite3_snprintf((int)sizeof(stackbuf1), stackbuf1, fmt1,
                     input, n1, (unsigned)n1, (sqlite3_int64)n2, (double)n2);
  }

  sqlite3_snprintf((int)sizeof(stackbuf2), stackbuf2, "%.*s",
                   (n2 < 0 ? -(n2 % 32) : (n2 % 32)), input);

  if (strcmp(fmt2, "%z") == 0){
    char *arg = sqlite3_mprintf("%s", input);
    wrap_vsnprintf((n1 % 512), stackbuf1, fmt2, arg);
  }else{
    wrap_vsnprintf((n1 % 512), stackbuf1, fmt2,
                   input, n2, (unsigned)n2, (sqlite3_int64)n1, (double)n1);
  }

  wrap_vsnprintf((n2 % 16), stackbuf2, "%s:%d", input, n1);

  acc = sqlite3_str_new(db);
  if (acc){
    sqlite3_str_appendall(acc, input);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%d", n1);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%u", (unsigned)n2);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%Q", input);
    sqlite3_str_appendall(acc, ":");
    sqlite3_str_appendf(acc, "%.*s", (n1 < 0 ? -(n1 % 128) : (n1 % 128)), input);
    sqlite3_str_appendall(acc, ":");
    finished = sqlite3_str_finish(acc);
    acc = NULL;
    sqlite3_free(finished);
    finished = NULL;
  }

  wrap_log(errcode, "%s", input);
  if (strcmp(fmt1, "%z") == 0){
    char *arg = sqlite3_mprintf("%s", input);
    wrap_log(errcode ^ 0x55, fmt1, arg);
  }else{
    wrap_log(errcode ^ 0x55, fmt1,
             input, n1, (unsigned)n1, (sqlite3_int64)n2, (double)n2);
  }

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