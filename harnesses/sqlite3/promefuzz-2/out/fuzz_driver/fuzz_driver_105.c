// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_reset at sqlite3.c:17356:17 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_reset at sqlite3.c:17356:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_vappendf at sqlite3.c:16479:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int_bounded(const uint8_t **p, size_t *n, int maxv) {
  if (maxv <= 0) return 0;
  return (int)(read_u32(p, n) % (uint32_t)maxv);
}

static void call_vappendf_impl(sqlite3_str *s, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  sqlite3_str_vappendf(s, fmt, ap);
  va_end(ap);
}

static void safe_appendf(sqlite3_str *s, int selector, const char *a, const char *b, int x, int y) {
  switch (selector % 8) {
    case 0:
      sqlite3_str_appendf(s, "%s", a);
      break;
    case 1:
      sqlite3_str_appendf(s, "%d", x);
      break;
    case 2:
      sqlite3_str_appendf(s, "%s:%d", a, x);
      break;
    case 3:
      sqlite3_str_appendf(s, "[%s]-[%s]", a, b);
      break;
    case 4:
      sqlite3_str_appendf(s, "%08x", (unsigned)x);
      break;
    case 5:
      sqlite3_str_appendf(s, "%.*s", y & 31, a);
      break;
    case 6:
      sqlite3_str_appendf(s, "%c%c%c", (char)(x & 0x7f), (char)(y & 0x7f), 'Z');
      break;
    default:
      sqlite3_str_appendf(s, "%s|%d|%s|%d", a, x, b, y);
      break;
  }
}

static void safe_vappendf(sqlite3_str *s, int selector, const char *a, const char *b, int x, int y) {
  switch (selector % 8) {
    case 0:
      call_vappendf_impl(s, "%s", a);
      break;
    case 1:
      call_vappendf_impl(s, "%d", x);
      break;
    case 2:
      call_vappendf_impl(s, "%s:%d", a, x);
      break;
    case 3:
      call_vappendf_impl(s, "[%s]-[%s]", a, b);
      break;
    case 4:
      call_vappendf_impl(s, "%08x", (unsigned)x);
      break;
    case 5:
      call_vappendf_impl(s, "%.*s", y & 31, a);
      break;
    case 6:
      call_vappendf_impl(s, "%c%c%c", (char)(x & 0x7f), (char)(y & 0x7f), 'V');
      break;
    default:
      call_vappendf_impl(s, "%s|%d|%s|%d", a, x, b, y);
      break;
  }
}

static void ensure_nonempty_before_printf(sqlite3_str *s) {
  char *v = sqlite3_str_value(s);
  if (v == NULL) {
    sqlite3_str_append(s, "X", 1);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_str *s = NULL;
  char *finished = NULL;
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  s = sqlite3_str_new(db);
  if (!s) {
    sqlite3_close(db);
    return 0;
  }

  {
    size_t buf_len = Size < 4096 ? Size : 4096;
    char *buf1 = (char *)malloc(buf_len + 1);
    char *buf2 = (char *)malloc(buf_len + 1);
    if (!buf1 || !buf2) {
      free(buf1);
      free(buf2);
      finished = sqlite3_str_finish(s);
      sqlite3_free(finished);
      sqlite3_close(db);
      return 0;
    }

    if (buf_len > 0) {
      memcpy(buf1, Data, buf_len);
      memcpy(buf2, Data, buf_len);
    }
    buf1[buf_len] = '\0';
    buf2[buf_len] = '\0';

    for (int iter = 0; iter < 32; iter++) {
      int op = read_int_bounded(&p, &n, 7);
      int off1 = read_int_bounded(&p, &n, (int)(buf_len + 1));
      int off2 = read_int_bounded(&p, &n, (int)(buf_len + 1));
      int len1 = read_int_bounded(&p, &n, (int)(buf_len - (size_t)off1 + 1));
      int len2 = read_int_bounded(&p, &n, (int)(buf_len - (size_t)off2 + 1));
      int x = (int)read_u32(&p, &n);
      int y = (int)read_u32(&p, &n);

      char *a = buf1 + off1;
      char *b = buf2 + off2;

      if ((size_t)off1 + (size_t)len1 <= buf_len) buf1[off1 + len1] = '\0';
      if ((size_t)off2 + (size_t)len2 <= buf_len) buf2[off2 + len2] = '\0';

      switch (op) {
        case 0:
          sqlite3_str_append(s, a, len1);
          break;
        case 1:
          sqlite3_str_appendall(s, a);
          break;
        case 2:
          ensure_nonempty_before_printf(s);
          safe_appendf(s, x, a, b, x, y);
          break;
        case 3:
          ensure_nonempty_before_printf(s);
          safe_vappendf(s, x, a, b, x, y);
          break;
        case 4: {
          char *v = sqlite3_str_value(s);
          if (v) {
            volatile size_t l = strlen(v);
            (void)l;
          }
          break;
        }
        case 5:
          sqlite3_str_reset(s);
          break;
        case 6:
        default:
          sqlite3_str_append(s, "", 0);
          sqlite3_str_appendall(s, "");
          ensure_nonempty_before_printf(s);
          safe_appendf(s, y, a, b, x, y);
          break;
      }

      if ((x & 7) == 0) {
        char *v = sqlite3_str_value(s);
        if (v) {
          volatile unsigned char c = (unsigned char)v[0];
          (void)c;
        }
      }

      if ((y & 15) == 1) {
        sqlite3_str_reset(s);
      }
    }

    free(buf1);
    free(buf2);
  }

  finished = sqlite3_str_finish(s);
  sqlite3_free(finished);
  sqlite3_close(db);
  return 0;
}