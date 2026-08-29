// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sqlite3.h>

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static int pick_int(const uint8_t *Data, size_t Size, size_t *off){
  if (*off + 4 <= Size) {
    int v = (int)rd32(Data + *off);
    *off += 4;
    return v;
  }
  if (*off < Size) {
    return (int)Data[(*off)++];
  }
  return 0;
}

static const char *pick_cstr(const uint8_t *Data, size_t Size, size_t *off,
                             char *buf, size_t bufsz){
  size_t i = 0;
  if (bufsz == 0) return "";
  while (*off < Size && i + 1 < bufsz) {
    uint8_t c = Data[(*off)++];
    if (c == 0) break;
    buf[i++] = (char)c;
  }
  buf[i] = '\0';
  return buf;
}

static void safe_vsnprintf_call(int n, char *zBuf, const char *fmt,
                                int i1, sqlite3_int64 i64v,
                                const char *s1, const char *s2){
  if (!zBuf || !fmt) return;
  sqlite3_snprintf(n, zBuf, fmt, i1, i64v, s1, s2);
}

static char *safe_vmprintf_call(const char *fmt,
                                int i1, sqlite3_int64 i64v,
                                const char *s1, const char *s2){
  if (!fmt) return NULL;
  return sqlite3_mprintf(fmt, i1, i64v, s1, s2);
}

static void safe_str_vappendf_call(sqlite3_str *acc, const char *fmt,
                                   int i1, sqlite3_int64 i64v,
                                   const char *s1, const char *s2){
  if (!acc || !fmt) return;
  sqlite3_str_appendf(acc, fmt, i1, i64v, s1, s2);
}

int LLVMFuzzerTestOneInput_71(const uint8_t *Data, size_t Size){
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_str *acc = NULL;
  char fmt1[128], fmt2[128], s1[128], s2[128];
  int rc;
  int n1, n2;
  int a1, a2;
  sqlite3_int64 a3;
  char stackbuf1[256];
  char stackbuf2[64];
  char *heapbuf = NULL;
  char *m1 = NULL;
  char *m2 = NULL;
  char *finished = NULL;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  n1 = pick_int(Data, Size, &off);
  n2 = pick_int(Data, Size, &off);
  a1 = pick_int(Data, Size, &off);
  a2 = pick_int(Data, Size, &off);
  a3 = (sqlite3_int64)(int64_t)pick_int(Data, Size, &off);

  pick_cstr(Data, Size, &off, fmt1, sizeof(fmt1));
  pick_cstr(Data, Size, &off, fmt2, sizeof(fmt2));
  pick_cstr(Data, Size, &off, s1, sizeof(s1));
  pick_cstr(Data, Size, &off, s2, sizeof(s2));

  if (fmt1[0] == '\0') strcpy(fmt1, "%d %lld %s %s");
  if (fmt2[0] == '\0') strcpy(fmt2, "%d|%lld|%q|%Q");

  memset(stackbuf1, 0x41, sizeof(stackbuf1));
  memset(stackbuf2, 0x42, sizeof(stackbuf2));

  sqlite3_snprintf(n1, stackbuf1, fmt1, a1, a3, s1, s2);
  sqlite3_snprintf((int)sizeof(stackbuf2), stackbuf2, fmt2, a2, a3, s1, s2);

  safe_vsnprintf_call(n2, stackbuf1, fmt2, a1, a3, s1, s2);
  safe_vsnprintf_call(0, stackbuf2, fmt1, a2, a3, s1, s2);

  m1 = sqlite3_mprintf(fmt1, a1, a3, s1, s2);
  if (m1) {
    sqlite3_free(m1);
    m1 = NULL;
  }

  m2 = sqlite3_mprintf(fmt2, a2, a3, s1, s2);
  if (m2) {
    sqlite3_free(m2);
    m2 = NULL;
  }

  m1 = safe_vmprintf_call(fmt1, a1, a3, s1, s2);
  if (m1) {
    sqlite3_free(m1);
    m1 = NULL;
  }

  m2 = safe_vmprintf_call(fmt2, a2, a3, s1, s2);
  if (m2) {
    sqlite3_free(m2);
    m2 = NULL;
  }

  acc = sqlite3_str_new(db);
  if (acc) {
    sqlite3_str_appendf(acc, fmt1, a1, a3, s1, s2);
    sqlite3_str_appendf(acc, fmt2, a2, a3, s1, s2);
    safe_str_vappendf_call(acc, fmt1, a1, a3, s1, s2);
    safe_str_vappendf_call(acc, fmt2, a2, a3, s1, s2);
    (void)sqlite3_str_errcode(acc);
    finished = sqlite3_str_finish(acc);
    sqlite3_free(finished);
    acc = NULL;
  }

  heapbuf = (char *)malloc(32);
  if (heapbuf) {
    memset(heapbuf, 0, 32);
    sqlite3_snprintf(32, heapbuf, "%s", s1);
    safe_vsnprintf_call(32, heapbuf, "%d-%lld-%s-%s", a1, a3, s1, s2);
    free(heapbuf);
  }

  sqlite3_close(db);
  return 0;
}