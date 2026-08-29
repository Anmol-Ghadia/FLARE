// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strlike at sqlite3.c:107939:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
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

static unsigned int read_uint(const uint8_t **p, size_t *n) {
  return (unsigned int)read_u32(p, n);
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t max_len) {
  size_t len = *n;
  if (len > max_len) len = max_len;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static char *make_cstring_n(const uint8_t **p, size_t *n, size_t req_len) {
  size_t len = req_len;
  if (len > *n) len = *n;
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static int fuzz_collation_cmp(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  int mode = pArg ? *(int *)pArg : 0;
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  int min = n1 < n2 ? n1 : n2;

  switch (mode % 4) {
    case 0: {
      int rc = memcmp(a, b, (size_t)min);
      if (rc != 0) return rc;
      return (n1 > n2) - (n1 < n2);
    }
    case 1:
      return (n1 > n2) - (n1 < n2);
    case 2: {
      unsigned int sa = 0, sb = 0;
      for (int i = 0; i < n1; i++) sa += a[i];
      for (int i = 0; i < n2; i++) sb += b[i];
      return (sa > sb) - (sa < sb);
    }
    default:
      if (min > 0) return (int)a[0] - (int)b[0];
      return (n1 > n2) - (n1 < n2);
  }
}

int LLVMFuzzerTestOneInput_109(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  char *s1 = NULL, *s2 = NULL, *pat = NULL, *txt = NULL, *kw = NULL, *coll = NULL;
  sqlite3 *db = NULL;
  int *coll_arg = NULL;

  if (Size == 0) {
    sqlite3_stricmp(NULL, NULL);
    sqlite3_strnicmp(NULL, NULL, 0);
    sqlite3_strglob("", "");
    sqlite3_strlike("", "", 0);
    sqlite3_keyword_check("", 0);
    return 0;
  }

  uint32_t l1 = read_u32(&p, &n);
  uint32_t l2 = read_u32(&p, &n);
  uint32_t l3 = read_u32(&p, &n);
  uint32_t l4 = read_u32(&p, &n);
  uint32_t l5 = read_u32(&p, &n);
  uint32_t l6 = read_u32(&p, &n);

  size_t rem = n;
  size_t cap = rem / 6 + 1;
  if (cap > 4096) cap = 4096;

  s1 = make_cstring_n(&p, &n, l1 % (cap + 1));
  s2 = make_cstring_n(&p, &n, l2 % (cap + 1));
  pat = make_cstring_n(&p, &n, l3 % (cap + 1));
  txt = make_cstring_n(&p, &n, l4 % (cap + 1));
  kw = make_cstring_n(&p, &n, l5 % (cap + 1));
  coll = make_cstring_n(&p, &n, l6 % 64);

  if (!s1 || !s2 || !pat || !txt || !kw || !coll) goto cleanup;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (n) fwrite(p, 1, n, fp);
    fclose(fp);
  }

  const char *a1 = (Size & 1) ? s1 : NULL;
  const char *a2 = (Size & 2) ? s2 : NULL;
  const char *g1 = (Size & 4) ? pat : "";
  const char *g2 = (Size & 8) ? txt : "";
  unsigned int esc = read_uint(&p, &n) & 0xFF;
  int prefix_n = read_int(&p, &n);
  int kw_n = read_int(&p, &n);

  sqlite3_stricmp(a1, a2);
  sqlite3_stricmp(a2, a1);
  sqlite3_stricmp(s1, s2);
  sqlite3_stricmp("", s1);

  sqlite3_strnicmp(a1, a2, prefix_n);
  sqlite3_strnicmp(s1, s2, prefix_n < 0 ? -prefix_n : prefix_n);
  sqlite3_strnicmp(s1, s2, 0);
  sqlite3_strnicmp(NULL, NULL, prefix_n);

  sqlite3_strglob(g1, g2);
  sqlite3_strglob(pat, txt);
  sqlite3_strglob("", txt);
  sqlite3_strglob(pat, "");

  sqlite3_strlike(g1, g2, esc);
  sqlite3_strlike(pat, txt, 0);
  sqlite3_strlike("%", txt, esc);
  sqlite3_strlike(pat, "", esc);

  if (kw_n < 0) kw_n = -kw_n;
  if ((size_t)kw_n > strlen(kw)) kw_n = (int)strlen(kw);
  sqlite3_keyword_check(kw, kw_n);
  sqlite3_keyword_check(kw, (int)strlen(kw));
  sqlite3_keyword_check("", 0);

  if (sqlite3_open(":memory:", &db) == SQLITE_OK && db) {
    static const int encodings[] = {
      SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE, SQLITE_UTF16, SQLITE_UTF16_ALIGNED
    };
    int enc = encodings[read_u32(&p, &n) % (sizeof(encodings) / sizeof(encodings[0]))];

    if (coll[0] == '\0') {
      free(coll);
      coll = make_cstring((const uint8_t **)&Data, &Size, 16);
      if (!coll) goto cleanup;
      if (coll[0] == '\0') {
        free(coll);
        coll = strdup("fuzzcoll");
        if (!coll) goto cleanup;
      }
    }

    coll_arg = (int *)malloc(sizeof(int));
    if (coll_arg) {
      *coll_arg = read_int(&p, &n);
      sqlite3_create_collation(db, coll, enc, coll_arg, fuzz_collation_cmp);
      sqlite3_create_collation(db, coll, enc, coll_arg, fuzz_collation_cmp);
      sqlite3_create_collation(db, coll, enc, NULL, NULL);
      sqlite3_create_collation(db, coll, enc, coll_arg, fuzz_collation_cmp);
      sqlite3_create_collation(db, coll, enc, NULL, NULL);
      free(coll_arg);
      coll_arg = NULL;
    }

    sqlite3_create_collation(db, coll, enc, NULL, fuzz_collation_cmp);
    sqlite3_create_collation(db, coll, enc, NULL, NULL);
  }

cleanup:
  if (db) sqlite3_close(db);
  free(s1);
  free(s2);
  free(pat);
  free(txt);
  free(kw);
  free(coll);
  free(coll_arg);
  return 0;
}