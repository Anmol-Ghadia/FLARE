// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
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

static int read_u8(const uint8_t **p, size_t *n){
  if( *n < 1 ) return 0;
  int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static int read_u32(const uint8_t **p, size_t *n){
  int v = 0;
  int i;
  for(i = 0; i < 4 && *n > 0; i++){
    v = (v << 8) | **p;
    (*p)++;
    (*n)--;
  }
  return v;
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t max_len){
  size_t len, i;
  char *s;
  if( *n == 0 ){
    s = (char *)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }
  len = (size_t)read_u8(p, n);
  if( len > max_len ) len = max_len;
  if( len > *n ) len = *n;
  s = (char *)malloc(len + 1);
  if( !s ) return NULL;
  for(i = 0; i < len; i++){
    unsigned char c = (*p)[i];
    if( c == 0 ) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

int LLVMFuzzerTestOneInput_81(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;
  char *db = NULL, *jrnl = NULL, *wal = NULL;
  char *query_name1 = NULL, *query_name2 = NULL;
  char *created = NULL;
  const char **azParam = NULL;
  int i, nParam;

  db = make_cstring(&p, &n, 64);
  jrnl = make_cstring(&p, &n, 64);
  wal = make_cstring(&p, &n, 64);
  query_name1 = make_cstring(&p, &n, 32);
  query_name2 = make_cstring(&p, &n, 32);

  if( !db || !jrnl || !wal || !query_name1 || !query_name2 ) goto done;

  nParam = read_u8(&p, &n) % 8;
  if( nParam > 0 ){
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(char *));
    if( !azParam ) goto done;
    for(i = 0; i < nParam * 2; i++){
      char *s = make_cstring(&p, &n, 48);
      if( !s ) goto done;
      if( (i % 2) == 0 && s[0] == '\0' ){
        free(s);
        s = (char *)malloc(2);
        if( !s ) goto done;
        s[0] = 'k';
        s[1] = '\0';
      }
      azParam[i] = s;
    }
  }

  created = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);

  sqlite3_free_filename(NULL);

  if( created ){
    const char *v1 = sqlite3_uri_parameter(created, query_name1);
    const char *v2 = sqlite3_uri_parameter(created, query_name2);
    int b1 = sqlite3_uri_boolean(created, query_name1, read_u8(&p, &n));
    int b2 = sqlite3_uri_boolean(created, query_name2, read_u8(&p, &n) & 1);
    int idx1 = (int)(read_u8(&p, &n) % 12);
    int idx2 = (int)((int8_t)read_u8(&p, &n));
    const char *k1 = sqlite3_uri_key(created, idx1);
    const char *k2 = sqlite3_uri_key(created, idx2);
    const char *j1 = sqlite3_filename_journal(created);
    const char *j2 = sqlite3_filename_journal((const char *)j1);

    (void)v1;
    (void)v2;
    (void)b1;
    (void)b2;
    (void)k1;
    (void)k2;
    (void)j1;
    (void)j2;

    if( n >= 4 ){
      int rounds = read_u32(&p, &n) % 4;
      while( rounds-- > 0 ){
        int which = read_u8(&p, &n) % 4;
        const char *name = (which & 1) ? query_name1 : query_name2;
        int def = (which & 2) ? 1 : 0;
        (void)sqlite3_uri_parameter(created, name);
        (void)sqlite3_uri_boolean(created, name, def);
        (void)sqlite3_uri_key(created, read_u8(&p, &n) % 16);
      }
    }

    sqlite3_free_filename(created);
    created = NULL;
  }else{
    (void)sqlite3_uri_parameter(NULL, query_name1);
    (void)sqlite3_uri_boolean(NULL, query_name2, 1);
    (void)sqlite3_uri_key(NULL, 0);
    (void)sqlite3_filename_journal(NULL);
  }

done:
  if( azParam ){
    for(i = 0; i < nParam * 2; i++){
      free((void *)azParam[i]);
    }
    free(azParam);
  }
  free(db);
  free(jrnl);
  free(wal);
  free(query_name1);
  free(query_name2);
  sqlite3_free_filename(created);
  return 0;
}