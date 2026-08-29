// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
// sqlite3_str_appendchar at sqlite3.c:17235:17 in sqlite3.h
// sqlite3_str_reset at sqlite3.c:17356:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

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

static int read_int(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static size_t take_bytes(const uint8_t **p, size_t *n, size_t want, const uint8_t **out) {
  size_t m = (*n < want) ? *n : want;
  *out = *p;
  *p += m;
  *n -= m;
  return m;
}

int LLVMFuzzerTestOneInput_68(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  sqlite3_str *s = NULL;
  FILE *fp = NULL;
  char *finished = NULL;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
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
    int rounds = 1;
    if (n > 0) {
      rounds = (int)(p[0] % 32) + 1;
      p++;
      n--;
    }

    for (int i = 0; i < rounds; i++) {
      int op = 0;
      if (n > 0) {
        op = *p % 6;
        p++;
        n--;
      }

      switch (op) {
        case 0: {
          int req = read_int(&p, &n);
          size_t want = (req < 0) ? (size_t)(-(req + 1)) : (size_t)req;
          const uint8_t *buf = NULL;
          size_t got = take_bytes(&p, &n, want, &buf);
          sqlite3_str_append(s, (const char *)buf, (int)got);
          break;
        }

        case 1: {
          int count = read_int(&p, &n);
          char c = 0;
          if (n > 0) {
            c = (char)*p;
            p++;
            n--;
          }
          if (count < 0) count = -(count & 0x7f);
          else count &= 0x7f;
          sqlite3_str_appendchar(s, count, c);
          break;
        }

        case 2: {
          sqlite3_str_reset(s);
          break;
        }

        case 3: {
          (void)sqlite3_str_length(s);
          break;
        }

        case 4: {
          (void)sqlite3_str_errcode(s);
          break;
        }

        default: {
          const uint8_t *buf = NULL;
          size_t got = take_bytes(&p, &n, n > 64 ? 64 : n, &buf);
          if (got > 0) {
            sqlite3_str_append(s, (const char *)buf, (int)got);
          } else {
            sqlite3_str_append(s, "", 0);
          }
          (void)sqlite3_str_length(NULL);
          (void)sqlite3_str_errcode(NULL);
          break;
        }
      }

      (void)sqlite3_str_length(s);
      (void)sqlite3_str_errcode(s);
    }
  }

  finished = sqlite3_str_finish(s);
  if (finished) {
    sqlite3_free(finished);
  }

  sqlite3_close(db);
  return 0;
}