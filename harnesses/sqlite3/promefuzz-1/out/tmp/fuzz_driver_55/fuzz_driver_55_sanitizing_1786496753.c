#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static unsigned int read_u8(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  unsigned int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static char *dup_segment_nul(const uint8_t **p, size_t *n, size_t want) {
  size_t take = want < *n ? want : *n;
  char *out = (char *)malloc(take + 1);
  if (!out) return NULL;
  if (take) memcpy(out, *p, take);
  out[take] = '\0';
  *p += take;
  *n -= take;
  return out;
}

static int fuzz_collation_compare(void *pArg, int len1, const void *a, int len2, const void *b) {
  unsigned int mode = (unsigned int)(uintptr_t)pArg;
  const unsigned char *s1 = (const unsigned char *)a;
  const unsigned char *s2 = (const unsigned char *)b;
  int minlen = len1 < len2 ? len1 : len2;

  if (mode & 1U) {
    int rc = sqlite3_strnicmp((const char *)s1, (const char *)s2, minlen);
    if (rc != 0) return rc;
  } else {
    for (int i = 0; i < minlen; ++i) {
      if (s1[i] != s2[i]) return (int)s1[i] - (int)s2[i];
    }
  }

  if (len1 < len2) return -1;
  if (len1 > len2) return 1;
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  uint32_t len1 = read_u32(&p, &n);
  uint32_t len2 = read_u32(&p, &n);
  uint32_t len3 = read_u32(&p, &n);
  uint32_t len4 = read_u32(&p, &n);
  uint32_t len5 = read_u32(&p, &n);
  unsigned int esc = read_u8(&p, &n);
  unsigned int mode = read_u8(&p, &n);

  if (Size > 0) {
    len1 %= (Size + 1);
    len2 %= (Size + 1);
    len3 %= (Size + 1);
    len4 %= (Size + 1);
    len5 %= (Size + 1);
  } else {
    len1 = len2 = len3 = len4 = len5 = 0;
  }

  char *glob_pat = dup_segment_nul(&p, &n, len1);
  char *glob_str = dup_segment_nul(&p, &n, len2);
  char *like_pat = dup_segment_nul(&p, &n, len3);
  char *like_str = dup_segment_nul(&p, &n, len4);
  char *sql = dup_segment_nul(&p, &n, len5);

  if (!glob_pat || !glob_str || !like_pat || !like_str || !sql) {
    free(glob_pat);
    free(glob_str);
    free(like_pat);
    free(like_str);
    free(sql);
    return 0;
  }

  (void)sqlite3_strglob(glob_pat, glob_str);
  (void)sqlite3_strglob(glob_str, glob_pat);

  (void)sqlite3_strlike(like_pat, like_str, esc);
  (void)sqlite3_strlike(like_str, like_pat, 0);

  (void)sqlite3_stricmp(glob_pat, glob_str);
  (void)sqlite3_stricmp(glob_pat, NULL);
  (void)sqlite3_stricmp(NULL, glob_str);
  (void)sqlite3_stricmp(NULL, NULL);

  (void)sqlite3_strnicmp(glob_pat, glob_str, (int)(mode % 64));
  (void)sqlite3_strnicmp(like_pat, like_str, (int)((len3 + len4) % 128));

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) == SQLITE_OK && db != NULL) {
    const char *collname = (glob_pat[0] != '\0') ? glob_pat : "fuzzcoll";
    int encodings[] = {
      SQLITE_UTF8,
      SQLITE_UTF16LE,
      SQLITE_UTF16BE,
      SQLITE_UTF16,
      SQLITE_UTF16_ALIGNED
    };

    for (size_t i = 0; i < sizeof(encodings) / sizeof(encodings[0]); ++i) {
      (void)sqlite3_create_collation(
          db,
          collname,
          encodings[i],
          (void *)(uintptr_t)(mode + (unsigned int)i),
          fuzz_collation_compare);
    }

    (void)sqlite3_create_collation(db, collname, SQLITE_UTF8, NULL, NULL);

    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    int sql_len = (int)strlen(sql);
    (void)sqlite3_prepare_v2(db, sql, sql_len, &stmt, &tail);
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }

    if (n > 0) {
      char *sql2 = (char *)malloc(n + 1);
      if (sql2) {
        memcpy(sql2, p, n);
        sql2[n] = '\0';
        (void)sqlite3_prepare_v2(db, sql2, (int)n, &stmt, &tail);
        if (stmt) {
          sqlite3_finalize(stmt);
          stmt = NULL;
        }
        free(sql2);
      }
    }

    sqlite3_close(db);
  }

  free(glob_pat);
  free(glob_str);
  free(like_pat);
  free(like_str);
  free(sql);
  return 0;
}