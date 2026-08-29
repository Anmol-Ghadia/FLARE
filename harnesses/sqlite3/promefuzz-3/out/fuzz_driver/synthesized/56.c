// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_reset at sqlite3.c:17356:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_reset at sqlite3.c:17356:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

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

static char *make_cstring(const uint8_t **data, size_t *size, size_t max_len) {
  size_t n = *size;
  if (n > max_len) n = max_len;
  char *s = (char *)sqlite3_malloc64(n + 1);
  if (!s) return NULL;
  if (n > 0) memcpy(s, *data, n);
  s[n] = '\0';
  *data += n;
  *size -= n;
  return s;
}

int LLVMFuzzerTestOneInput_56(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;
  sqlite3_str *str = NULL;
  char *input1 = NULL;
  char *input2 = NULL;
  char *result = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  if (remaining >= 4) {
    int new_limit = (int)(read_u32(&p, &remaining) % 4096U);
    sqlite3_limit(db, SQLITE_LIMIT_LENGTH, new_limit);
  }

  str = sqlite3_str_new(db);
  if (!str) {
    sqlite3_close(db);
    return 0;
  }

  input1 = make_cstring(&p, &remaining, 1024);
  input2 = make_cstring(&p, &remaining, 1024);

  if (input1) {
    sqlite3_str_appendall(str, input1);
    (void)sqlite3_str_value(str);
  }

  if (input2) {
    sqlite3_str_appendall(str, input2);
    (void)sqlite3_str_value(str);
  }

  if (remaining >= 4) {
    uint32_t mode = read_u32(&p, &remaining) % 4U;
    switch (mode) {
      case 0: {
        int a = (int)read_u32(&p, &remaining);
        unsigned b = (unsigned)read_u32(&p, &remaining);
        sqlite3_str_appendf(str, "%d:%u", a, b);
        break;
      }
      case 1:
        sqlite3_str_appendf(str, "[%s]-[%s]", input1 ? input1 : "", input2 ? input2 : "");
        break;
      case 2:
        sqlite3_str_reset(str);
        if (input2) sqlite3_str_appendall(str, input2);
        sqlite3_str_appendf(str, "|%d|", (int)remaining);
        break;
      default: {
        char c1 = remaining > 0 ? (char)p[0] : 'A';
        char c2 = remaining > 1 ? (char)p[1] : 'B';
        char c3 = remaining > 2 ? (char)p[2] : 'C';
        sqlite3_str_appendall(str, "");
        sqlite3_str_appendf(str, "%c%c%c", c1, c2, c3);
        break;
      }
    }
    (void)sqlite3_str_value(str);
  }

  if (remaining & 1U) {
    sqlite3_str_reset(str);
    if (input1) sqlite3_str_appendall(str, input1);
    if (input2) sqlite3_str_appendall(str, input2);
    (void)sqlite3_str_value(str);
  }

  result = sqlite3_str_finish(str);
  if (result) {
    volatile size_t len = strlen(result);
    (void)len;
    sqlite3_free(result);
  }

  if (input1) sqlite3_free(input1);
  if (input2) sqlite3_free(input2);
  sqlite3_close(db);
  return 0;
}