#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
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
  if (*off < Size) return (int)Data[(*off)++];
  return 0;
}

static const char *pick_str(const uint8_t *Data, size_t Size, size_t *off, char *buf, size_t bufsz){
  size_t i = 0;
  if (bufsz == 0) return "";
  while (*off < Size && i + 1 < bufsz) {
    unsigned char c = Data[(*off)++];
    if (c == 0) break;
    buf[i++] = (char)((c >= 32 && c <= 126) ? c : ('A' + (c % 26)));
    if ((c & 0x1f) == 0) break;
  }
  buf[i] = '\0';
  return buf;
}

static char *call_vmprintf_wrapper(const char *fmt, ...){
  va_list ap;
  char *res;
  va_start(ap, fmt);
  res = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return res;
}

static char *call_vsnprintf_wrapper(int n, char *zBuf, const char *fmt, ...){
  va_list ap;
  char *res;
  va_start(ap, fmt);
  res = sqlite3_vsnprintf(n, zBuf, fmt, ap);
  va_end(ap);
  return res;
}

static void call_str_vappendf_wrapper(sqlite3_str *acc, const char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  sqlite3_str_vappendf(acc, fmt, ap);
  va_end(ap);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_str *acc = NULL;
  char s1[128], s2[128], s3[128];
  size_t off = 0;
  int rc;
  int i1, i2;
  char *m1 = NULL, *m2 = NULL, *m3 = NULL;
  char *vm1 = NULL, *vm2 = NULL;
  char buf1[256];
  char buf2[32];
  char *retbuf;
  char *finished = NULL;

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  i1 = pick_int(Data, Size, &off);
  i2 = pick_int(Data, Size, &off);
  pick_str(Data, Size, &off, s1, sizeof(s1));
  pick_str(Data, Size, &off, s2, sizeof(s2));
  pick_str(Data, Size, &off, s3, sizeof(s3));

  m1 = sqlite3_mprintf("%s", s1);
  m2 = sqlite3_mprintf("%d-%u-%x", i1, (unsigned)i2, (unsigned)(i1 ^ i2));
  m3 = sqlite3_mprintf("%q|%Q|%w", s1, s2, s3);

  vm1 = call_vmprintf_wrapper("%s:%d:%c", s2, i1, (char)('A' + ((unsigned)i2 % 26)));
  vm2 = call_vmprintf_wrapper("%q/%Q/%w", s1, s2, s3);

  memset(buf1, 0x41, sizeof(buf1));
  memset(buf2, 0x42, sizeof(buf2));
  retbuf = call_vsnprintf_wrapper((int)sizeof(buf1), buf1, "%s-%d-%u", s3, i2, (unsigned)i1);
  (void)retbuf;
  retbuf = call_vsnprintf_wrapper((int)sizeof(buf2), buf2, "%q|%Q", s1, s2);
  (void)retbuf;
  retbuf = call_vsnprintf_wrapper(0, buf2, "%s", s1);
  (void)retbuf;

  (void)sqlite3_str_value(NULL);

  acc = sqlite3_str_new(db);
  if (acc) {
    (void)sqlite3_str_value(acc);

    sqlite3_str_appendf(acc, "%s", s1);
    sqlite3_str_appendf(acc, "|%d|%u|%x|", i1, (unsigned)i2, (unsigned)(i1 + i2));
    sqlite3_str_appendf(acc, "%q|%Q|%w", s1, s2, s3);

    if (sqlite3_str_errcode(acc) == SQLITE_OK) {
      call_str_vappendf_wrapper(acc, "", 0);
    }
    if (sqlite3_str_errcode(acc) == SQLITE_OK) {
      call_str_vappendf_wrapper(acc, "|%s|%d|", s2, i2);
    }
    if (sqlite3_str_errcode(acc) == SQLITE_OK) {
      call_str_vappendf_wrapper(acc, "%q/%Q/%w", s1, s2, s3);
    }

    (void)sqlite3_str_value(acc);

    if (sqlite3_str_errcode(acc) == SQLITE_OK && m1) {
      sqlite3_str_appendf(acc, "|%z|", m1);
      m1 = NULL;
    }
    if (sqlite3_str_errcode(acc) == SQLITE_OK && vm1) {
      sqlite3_str_appendf(acc, "%z", vm1);
      vm1 = NULL;
    }
    if (sqlite3_str_errcode(acc) == SQLITE_OK && m2) {
      call_str_vappendf_wrapper(acc, "|%z|", m2);
      m2 = NULL;
    }

    (void)sqlite3_str_value(acc);

    finished = sqlite3_str_finish(acc);
    acc = NULL;
    sqlite3_free(finished);
  }

  sqlite3_free(m1);
  sqlite3_free(m2);
  sqlite3_free(m3);
  sqlite3_free(vm1);
  sqlite3_free(vm2);

  sqlite3_close(db);
  return 0;
}