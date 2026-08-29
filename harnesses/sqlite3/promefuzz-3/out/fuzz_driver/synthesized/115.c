// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_vsnprintf at sqlite3.c:17488:18 in sqlite3.h
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_str_vappendf at sqlite3.c:16479:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static void call_sqlite3_vsnprintf(int n, char *zBuf, const char *zFormat, ...){
  va_list ap;
  va_start(ap, zFormat);
  (void)sqlite3_vsnprintf(n, zBuf, zFormat, ap);
  va_end(ap);
}

static char *call_sqlite3_vmprintf(const char *zFormat, ...){
  va_list ap;
  char *rc;
  va_start(ap, zFormat);
  rc = sqlite3_vmprintf(zFormat, ap);
  va_end(ap);
  return rc;
}

static void call_sqlite3_str_vappendf(sqlite3_str *pStr, const char *zFormat, ...){
  va_list ap;
  va_start(ap, zFormat);
  sqlite3_str_vappendf(pStr, zFormat, ap);
  va_end(ap);
}

int LLVMFuzzerTestOneInput_115(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_str *acc = NULL;
  char *dyn1 = NULL;
  char *dyn2 = NULL;
  char *finished = NULL;
  char stackBuf[256];
  char *fmt = NULL;
  char *arg1 = NULL;
  char *arg2 = NULL;
  int n1, width;
  int64_t i64v;
  double dv;
  size_t fmtLen, a1Len, a2Len;
  int rc;
  const char *safeFmt;

  if( Size < 12 ){
    return 0;
  }

  memset(stackBuf, 0, sizeof(stackBuf));

  fmtLen = (size_t)(Data[0] % (Size - 2));
  a1Len = (size_t)(Data[1] % (Size - fmtLen - 1));
  a2Len = Size - 2 - fmtLen - a1Len;

  fmt = (char *)malloc(fmtLen + 1);
  arg1 = (char *)malloc(a1Len + 1);
  arg2 = (char *)malloc(a2Len + 1);
  if( !fmt || !arg1 || !arg2 ){
    free(fmt);
    free(arg1);
    free(arg2);
    return 0;
  }

  memcpy(fmt, Data + 2, fmtLen);
  fmt[fmtLen] = '\0';
  memcpy(arg1, Data + 2 + fmtLen, a1Len);
  arg1[a1Len] = '\0';
  memcpy(arg2, Data + 2 + fmtLen + a1Len, a2Len);
  arg2[a2Len] = '\0';

  n1 = (int)(rd32(Data + 2) % 512);
  width = (int)(Data[10] % 64);
  i64v = (int64_t)(int32_t)rd32(Data + 2);
  dv = (double)((int32_t)rd32(Data + 6)) / 17.0;

  rc = sqlite3_open(":memory:", &db);
  if( rc != SQLITE_OK ){
    if( db ) sqlite3_close(db);
    free(fmt);
    free(arg1);
    free(arg2);
    return 0;
  }

  safeFmt = "%s|%s|%d|%lld|%f";

  (void)sqlite3_snprintf((int)sizeof(stackBuf), stackBuf, safeFmt,
                         arg1, arg2, width, (long long)i64v, dv);
  call_sqlite3_vsnprintf(n1, stackBuf, safeFmt,
                         arg1, arg2, width, (long long)i64v, dv);

  dyn1 = sqlite3_mprintf(safeFmt, arg1, arg2, width, (long long)i64v, dv);
  if( dyn1 ){
    sqlite3_free(dyn1);
    dyn1 = NULL;
  }

  dyn2 = call_sqlite3_vmprintf(safeFmt, arg1, arg2, width, (long long)i64v, dv);
  if( dyn2 ){
    sqlite3_free(dyn2);
    dyn2 = NULL;
  }

  dyn1 = sqlite3_mprintf("%s", fmt);
  if( dyn1 ){
    sqlite3_free(dyn1);
    dyn1 = NULL;
  }

  acc = sqlite3_str_new(db);
  if( acc ){
    sqlite3_str_appendf(acc, safeFmt, arg1, arg2, width, (long long)i64v, dv);
    call_sqlite3_str_vappendf(acc, safeFmt, arg1, arg2, width, (long long)i64v, dv);
    sqlite3_str_appendf(acc, "%s", stackBuf);
    sqlite3_str_appendf(acc, "%s", arg1);
    sqlite3_str_appendf(acc, "%*s", width, arg2);
    sqlite3_str_appendf(acc, "%s", fmt);
    (void)sqlite3_str_errcode(acc);
    finished = sqlite3_str_finish(acc);
    if( finished ){
      sqlite3_free(finished);
      finished = NULL;
    }
  }

  sqlite3_close(db);
  free(fmt);
  free(arg1);
  free(arg2);
  return 0;
}