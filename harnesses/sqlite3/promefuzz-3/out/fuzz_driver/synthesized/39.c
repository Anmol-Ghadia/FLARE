// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_keyword_name at sqlite3.c:151127:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_name at sqlite3.c:151127:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_keyword_name at sqlite3.c:151127:16 in sqlite3.h
// sqlite3_keyword_name at sqlite3.c:151127:16 in sqlite3.h
// sqlite3_keyword_name at sqlite3.c:151127:16 in sqlite3.h
// sqlite3_keyword_count at sqlite3.c:151133:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strnicmp at sqlite3.c:19662:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
// sqlite3_keyword_check at sqlite3.c:151134:16 in sqlite3.h
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
#include "sqlite3.h"

static int read_u32(const uint8_t *data, size_t size, size_t *off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4; i++){
    v <<= 8;
    if(*off < size){
      v |= data[*off];
      (*off)++;
    }
  }
  return (int)v;
}

static char *make_cstring(const uint8_t *src, size_t len){
  char *s = (char *)malloc(len + 1);
  if(!s) return NULL;
  if(len) memcpy(s, src, len);
  s[len] = '\0';
  return s;
}

int LLVMFuzzerTestOneInput_39(const uint8_t *Data, size_t Size){
  size_t off = 0;
  size_t len1, len2, len3, len4;
  char *s1 = NULL, *s2 = NULL, *s3 = NULL, *s4 = NULL;
  const char *kw = NULL;
  int kwlen = 0;
  int rc;
  int count;
  int ncmp;
  int idx;

  if(Data == NULL) return 0;

  len1 = (size_t)read_u32(Data, Size, &off);
  len2 = (size_t)read_u32(Data, Size, &off);
  len3 = (size_t)read_u32(Data, Size, &off);
  len4 = (size_t)read_u32(Data, Size, &off);

  if(len1 > Size - off) len1 = Size - off;
  s1 = make_cstring(Data + off, len1);
  if(!s1) goto cleanup;
  off += len1;

  if(len2 > Size - off) len2 = Size - off;
  s2 = make_cstring(Data + off, len2);
  if(!s2) goto cleanup;
  off += len2;

  if(len3 > Size - off) len3 = Size - off;
  s3 = make_cstring(Data + off, len3);
  if(!s3) goto cleanup;
  off += len3;

  if(len4 > Size - off) len4 = Size - off;
  s4 = make_cstring(Data + off, len4);
  if(!s4) goto cleanup;
  off += len4;

  count = sqlite3_keyword_count();

  sqlite3_stricmp(s1, s2);
  sqlite3_stricmp(s2, s3);
  sqlite3_stricmp(s3, s4);
  sqlite3_stricmp(NULL, NULL);
  sqlite3_stricmp(NULL, s1);
  sqlite3_stricmp(s1, NULL);

  ncmp = read_u32(Data, Size, &off);
  if(ncmp < 0) ncmp = -ncmp;
  sqlite3_strnicmp(s1, s2, ncmp);
  sqlite3_strnicmp(s2, s3, (int)len2);
  sqlite3_strnicmp(s3, s4, (int)len3);
  sqlite3_strnicmp(NULL, NULL, ncmp);
  sqlite3_strnicmp(NULL, s1, ncmp);
  sqlite3_strnicmp(s1, NULL, ncmp);

  sqlite3_strglob(s1, s2);
  sqlite3_strglob(s2, s3);
  sqlite3_strglob(s3, s4);
  sqlite3_strglob("", s1);
  sqlite3_strglob("*", s2);
  sqlite3_strglob("?", s3);

  sqlite3_keyword_check(s1, (int)len1);
  sqlite3_keyword_check(s2, (int)len2);
  sqlite3_keyword_check(s3, (int)len3);
  sqlite3_keyword_check(s4, (int)len4);

  idx = read_u32(Data, Size, &off);
  rc = sqlite3_keyword_name(idx, &kw, &kwlen);
  if(rc == SQLITE_OK && kw != NULL && kwlen >= 0){
    char *kwcopy = (char *)malloc((size_t)kwlen + 1);
    if(kwcopy){
      memcpy(kwcopy, kw, (size_t)kwlen);
      kwcopy[kwlen] = '\0';
      sqlite3_stricmp(kwcopy, s1);
      sqlite3_strnicmp(kwcopy, s2, kwlen);
      sqlite3_strglob(s3, kwcopy);
      sqlite3_keyword_check(kw, kwlen);
      free(kwcopy);
    }
  }

  if(count > 0){
    int valid_idx = idx % count;
    if(valid_idx < 0) valid_idx += count;
    rc = sqlite3_keyword_name(valid_idx, &kw, &kwlen);
    if(rc == SQLITE_OK && kw != NULL && kwlen >= 0){
      sqlite3_keyword_check(kw, kwlen);
      if((size_t)kwlen == len1){
        sqlite3_strnicmp(kw, s1, kwlen);
      }
    }
  }

  sqlite3_keyword_name(-1, &kw, &kwlen);
  sqlite3_keyword_name(count, &kw, &kwlen);
  sqlite3_keyword_name(count + 1, &kw, &kwlen);

cleanup:
  free(s1);
  free(s2);
  free(s3);
  free(s4);
  return 0;
}