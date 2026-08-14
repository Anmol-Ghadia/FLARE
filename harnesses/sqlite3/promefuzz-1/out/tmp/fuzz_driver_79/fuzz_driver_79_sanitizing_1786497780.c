#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

static int consume_u8(const uint8_t **data, size_t *size){
  if (*size == 0) return 0;
  int v = **data;
  (*data)++;
  (*size)--;
  return v;
}

static int consume_i32(const uint8_t **data, size_t *size){
  int v = 0;
  size_t n = *size < 4 ? *size : 4;
  for(size_t i = 0; i < n; i++){
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static char *make_cstring(const uint8_t *data, size_t len){
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, data, len);
  s[len] = '\0';
  return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t remaining = Size;

  int kw_count = sqlite3_keyword_count();

  int idx1 = consume_i32(&p, &remaining);
  int idx2 = consume_i32(&p, &remaining);
  int ncmp = consume_i32(&p, &remaining);

  size_t len1 = remaining / 2;
  size_t len2 = remaining - len1;

  char *s1 = make_cstring(p, len1);
  char *s2 = make_cstring(p + len1, len2);
  if (!s1 || !s2){
    free(s1);
    free(s2);
    return 0;
  }

  (void)sqlite3_stricmp(s1, s2);
  (void)sqlite3_stricmp(NULL, s1);
  (void)sqlite3_stricmp(s2, NULL);
  (void)sqlite3_stricmp(NULL, NULL);

  if (ncmp < 0) ncmp = -ncmp;
  (void)sqlite3_strnicmp(s1, s2, ncmp);
  (void)sqlite3_strnicmp(NULL, s1, ncmp);
  (void)sqlite3_strnicmp(s2, NULL, ncmp);
  (void)sqlite3_strnicmp(NULL, NULL, ncmp);

  (void)sqlite3_strglob(s1, s2);
  (void)sqlite3_strglob(s2, s1);
  (void)sqlite3_strglob("", s1);
  (void)sqlite3_strglob(s2, "");

  (void)sqlite3_keyword_check(s1, (int)len1);
  (void)sqlite3_keyword_check(s2, (int)len2);
  if (len1 > 0) (void)sqlite3_keyword_check(s1, (int)(len1 / 2));
  if (len2 > 0) (void)sqlite3_keyword_check(s2, (int)(len2 / 2));
  (void)sqlite3_keyword_check("", 0);

  {
    const char *kw = NULL;
    int kw_len = 0;

    (void)sqlite3_keyword_name(idx1, &kw, &kw_len);
    if (kw && kw_len >= 0){
      (void)sqlite3_keyword_check(kw, kw_len);
      (void)sqlite3_strnicmp(kw, s1, kw_len < ncmp ? kw_len : ncmp);
      (void)sqlite3_strglob(s1, kw);
    }

    (void)sqlite3_keyword_name(idx2, &kw, &kw_len);
    if (kw && kw_len >= 0){
      (void)sqlite3_keyword_check(kw, kw_len);
      (void)sqlite3_stricmp(kw, s2);
      (void)sqlite3_strglob(kw, s2);
    }
  }

  if (kw_count > 0){
    int bounded = idx1 % (kw_count + 2);
    if (bounded < 0) bounded = -bounded;

    for (int i = 0; i < 3; i++){
      int cur = bounded + i - 1;
      const char *kw = NULL;
      int kw_len = 0;
      int rc = sqlite3_keyword_name(cur, &kw, &kw_len);
      if (rc == SQLITE_OK && kw && kw_len >= 0){
        (void)sqlite3_keyword_check(kw, kw_len);
        (void)sqlite3_stricmp(kw, s1);
        (void)sqlite3_strnicmp(kw, s2, kw_len);
        (void)sqlite3_strglob(kw, s1);
      }
    }
  }else{
    const char *kw = NULL;
    int kw_len = 0;
    (void)sqlite3_keyword_name(0, &kw, &kw_len);
  }

  if (consume_u8(&p, &remaining) & 1){
    for (int i = 0; i < kw_count && i < 16; i++){
      const char *kw = NULL;
      int kw_len = 0;
      if (sqlite3_keyword_name(i, &kw, &kw_len) == SQLITE_OK && kw){
        (void)sqlite3_keyword_check(kw, kw_len);
        (void)sqlite3_stricmp(kw, kw);
        (void)sqlite3_strnicmp(kw, kw, kw_len);
      }
    }
  }

  free(s1);
  free(s2);
  return 0;
}