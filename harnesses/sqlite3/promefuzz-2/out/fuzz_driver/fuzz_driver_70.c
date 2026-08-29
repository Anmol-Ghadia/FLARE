// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_create_window_function at sqlite3.c:154256:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_create_function16 at sqlite3.c:154273:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_int_range(const uint8_t **data, size_t *size, int min, int max) {
  if (min >= max) return min;
  uint32_t v = read_u32(data, size);
  return min + (int)(v % (uint32_t)(max - min + 1));
}

static size_t take_bytes(const uint8_t **data, size_t *size, uint8_t *out, size_t maxlen) {
  size_t n = *size < maxlen ? *size : maxlen;
  if (n) {
    memcpy(out, *data, n);
    *data += n;
    *size -= n;
  }
  return n;
}

static char *make_utf8_name(const uint8_t **data, size_t *size) {
  size_t want = (size_t)read_int_range(data, size, 1, 255);
  char *s = (char *)malloc(want + 1);
  if (!s) return NULL;

  size_t n = take_bytes(data, size, (uint8_t *)s, want);
  if (n == 0) {
    s[0] = 'f';
    s[1] = '\0';
    return s;
  }

  for (size_t i = 0; i < n; ++i) {
    unsigned char c = (unsigned char)s[i];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (i > 0 && c >= '0' && c <= '9') || c == '_') {
      continue;
    }
    s[i] = (char)('a' + (c % 26));
  }
  if (!((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || s[0] == '_')) {
    s[0] = 'f';
  }
  s[n] = '\0';
  return s;
}

static void *make_utf16_name(const uint8_t **data, size_t *size, int *is_be) {
  size_t want_chars = (size_t)read_int_range(data, size, 1, 255);
  uint16_t *buf = (uint16_t *)malloc((want_chars + 1) * sizeof(uint16_t));
  if (!buf) return NULL;

  *is_be = read_int_range(data, size, 0, 1);
  for (size_t i = 0; i < want_chars; ++i) {
    uint16_t ch;
    if (*size >= 2) {
      ch = (uint16_t)(((*data)[0] << 8) | (*data)[1]);
      *data += 2;
      *size -= 2;
    } else if (*size == 1) {
      ch = (*data)[0];
      *data += 1;
      *size -= 1;
    } else {
      ch = (uint16_t)('a' + (i % 26));
    }

    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (i > 0 && ch >= '0' && ch <= '9') || ch == '_') {
      /* keep */
    } else {
      ch = (uint16_t)('a' + (ch % 26));
    }
    if (i == 0 && !((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')) {
      ch = 'f';
    }

    buf[i] = *is_be ? (uint16_t)((ch >> 8) | (ch << 8)) : ch;
  }

  buf[want_chars] = 0;
  return buf;
}

static int choose_etextrep(const uint8_t **data, size_t *size) {
  static const int reps[] = {
    SQLITE_UTF8,
    SQLITE_UTF16,
    SQLITE_UTF16LE,
    SQLITE_UTF16BE,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
    SQLITE_UTF8 | SQLITE_DIRECTONLY,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY,
    SQLITE_UTF16 | SQLITE_DETERMINISTIC,
    SQLITE_UTF16LE | SQLITE_DIRECTONLY,
    SQLITE_UTF16BE | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
  };
  uint32_t idx = read_u32(data, size);
  return reps[idx % (sizeof(reps) / sizeof(reps[0]))];
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3_result_error(ctx, "fuzz_error", -1);
}

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  if ((argc & 1) == 0) {
    sqlite3_result_error(ctx, "step_error", -1);
  } else if (argc > 0 && argv) {
    const unsigned char *txt = sqlite3_value_text(argv[0]);
    if (txt && txt[0] == 'x') {
      sqlite3_result_error(ctx, (const char *)txt, -1);
    }
  }
}

static void fuzz_final(sqlite3_context *ctx) {
  sqlite3_result_error(ctx, "final_error", -1);
}

static void fuzz_value(sqlite3_context *ctx) {
  sqlite3_result_error(ctx, "value_error", -1);
}

static void fuzz_inverse(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argv;
  if (argc > 1) {
    sqlite3_result_error(ctx, "inverse_error", -1);
  }
}

static void fuzz_destroy(void *p) {
  free(p);
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
}

static int choose_kind(const uint8_t **data, size_t *size) {
  return read_int_range(data, size, 0, 4);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA writable_schema=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(x);");
  exec_sql(db, "DELETE FROM t;");
  exec_sql(db, "INSERT INTO t(x) VALUES (1),('x'),(NULL);");

  const uint8_t *p = Data;
  size_t remaining = Size;

  for (int i = 0; i < 16; ++i) {
    int kind = choose_kind(&p, &remaining);
    int nArg = read_int_range(&p, &remaining, -1, 10);
    int eTextRep = choose_etextrep(&p, &remaining);

    char *name8 = make_utf8_name(&p, &remaining);
    int is_be = 0;
    void *name16 = make_utf16_name(&p, &remaining, &is_be);

    size_t app_sz = (size_t)read_int_range(&p, &remaining, 0, 64);
    void *app = malloc(app_sz ? app_sz : 1);
    if (app) {
      memset(app, 0, app_sz ? app_sz : 1);
      if (app_sz) take_bytes(&p, &remaining, (uint8_t *)app, app_sz);
    }

    if (!name8) name8 = strdup("f");
    if (!name8) {
      free(name16);
      free(app);
      break;
    }

    switch (kind) {
      case 0:
        (void)sqlite3_create_function_v2(
            db, name8, nArg, eTextRep, app,
            fuzz_scalar, NULL, NULL, fuzz_destroy);
        app = NULL;
        break;
      case 1:
        (void)sqlite3_create_function(
            db, name8, nArg, eTextRep, NULL,
            fuzz_scalar, NULL, NULL);
        free(app);
        app = NULL;
        break;
      case 2:
        (void)sqlite3_create_function(
            db, name8, nArg, eTextRep, NULL,
            NULL, fuzz_step, fuzz_final);
        free(app);
        app = NULL;
        break;
      case 3:
        (void)sqlite3_overload_function(db, name8, nArg);
        free(app);
        app = NULL;
        break;
      case 4:
      default:
        (void)sqlite3_create_window_function(
            db, name8, nArg, eTextRep, app,
            fuzz_step, fuzz_final, fuzz_value, fuzz_inverse, fuzz_destroy);
        app = NULL;
        break;
    }

    if (name16) {
      int enc16 = is_be ? SQLITE_UTF16BE : SQLITE_UTF16LE;
      if ((read_u32(&p, &remaining) & 1U) == 0) {
        (void)sqlite3_create_function16(
            db, name16, nArg, enc16, NULL,
            fuzz_scalar, NULL, NULL);
      } else {
        (void)sqlite3_create_function16(
            db, name16, nArg, enc16, NULL,
            NULL, fuzz_step, fuzz_final);
      }
    }

    {
      char sql[512];
      snprintf(sql, sizeof(sql), "SELECT \"%s\"(x) FROM t;", name8);
      exec_sql(db, sql);
      snprintf(sql, sizeof(sql), "SELECT \"%s\"() FROM t;", name8);
      exec_sql(db, sql);
      snprintf(sql, sizeof(sql), "SELECT \"%s\"(x,x) FROM t;", name8);
      exec_sql(db, sql);
      snprintf(sql, sizeof(sql), "SELECT \"%s\"(x) OVER () FROM t;", name8);
      exec_sql(db, sql);
    }

    free(name8);
    free(name16);
    free(app);
  }

  sqlite3_close(db);
  return 0;
}