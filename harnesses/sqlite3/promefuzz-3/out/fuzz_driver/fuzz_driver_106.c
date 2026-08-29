// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4 && *n > 0; i++){
    v = (v << 8) | **p;
    (*p)++;
    (*n)--;
  }
  return v;
}

static int read_int(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static char *read_string(const uint8_t **p, size_t *n, size_t max_len){
  size_t len, i;
  char *s;

  if(*n == 0){
    s = (char *)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }

  len = read_u32(p, n);
  if(max_len > 0 && len > max_len) len = max_len;
  if(len > *n) len = *n;

  s = (char *)malloc(len + 1);
  if(!s) return NULL;

  for(i = 0; i < len; i++){
    unsigned char c = **p;
    (*p)++;
    (*n)--;
    if(c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  return s;
}

static char *dup_cstr(const char *s){
  size_t len;
  char *out;
  if(!s) return NULL;
  len = strlen(s);
  out = (char *)malloc(len + 1);
  if(!out) return NULL;
  memcpy(out, s, len + 1);
  return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;
  int i, iter_count;
  char *db = NULL, *jrnl = NULL, *wal = NULL;
  char *filename = NULL;
  char **owned_params = NULL;
  const char **azParam = NULL;
  int nParam = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if(n == 0){
    sqlite3_free_filename(NULL);
    (void)sqlite3_uri_parameter(NULL, NULL);
    (void)sqlite3_uri_boolean(NULL, NULL, 0);
    (void)sqlite3_uri_key(NULL, 0);
    return 0;
  }

  db = read_string(&p, &n, 256);
  jrnl = read_string(&p, &n, 256);
  wal = read_string(&p, &n, 256);
  if(!db || !jrnl || !wal) goto cleanup;

  if(db[0] == '\0'){
    free(db);
    db = dup_cstr("./dummy_file");
    if(!db) goto cleanup;
  }

  nParam = read_int(&p, &n);
  if(nParam < 0) nParam = -nParam;
  nParam %= 16;

  if(nParam > 0){
    owned_params = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(const char *));
    if(!owned_params || !azParam) goto cleanup;

    for(i = 0; i < nParam * 2; i += 2){
      owned_params[i] = read_string(&p, &n, 64);
      owned_params[i + 1] = read_string(&p, &n, 128);
      if(!owned_params[i] || !owned_params[i + 1]) goto cleanup;

      if(owned_params[i][0] == '\0'){
        free(owned_params[i]);
        owned_params[i] = dup_cstr("k");
        if(!owned_params[i]) goto cleanup;
      }

      azParam[i] = owned_params[i];
      azParam[i + 1] = owned_params[i + 1];
    }
  }

  filename = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);

  if(filename){
    const char *wal_name = sqlite3_filename_wal(filename);
    (void)wal_name;

    iter_count = read_int(&p, &n);
    if(iter_count < 0) iter_count = -iter_count;
    iter_count = (iter_count % 32) + 1;

    for(i = 0; i < iter_count; i++){
      char *param_name = read_string(&p, &n, 64);
      int defv = read_int(&p, &n);

      if(!param_name) break;

      (void)sqlite3_uri_parameter(filename, param_name);
      (void)sqlite3_uri_boolean(filename, param_name, defv);
      free(param_name);
    }

    for(i = -2; i < nParam + 4; i++){
      (void)sqlite3_uri_key(filename, i);
    }

    if(nParam > 0){
      for(i = 0; i < nParam; i++){
        (void)sqlite3_uri_parameter(filename, azParam[i * 2]);
        (void)sqlite3_uri_boolean(filename, azParam[i * 2], i & 1);
      }
    }

    sqlite3_free_filename(filename);
    filename = NULL;
  }

  sqlite3_free_filename(NULL);
  (void)sqlite3_uri_parameter(NULL, "x");
  (void)sqlite3_uri_boolean(NULL, "x", 1);
  (void)sqlite3_uri_key(NULL, 1);

cleanup:
  if(filename) sqlite3_free_filename(filename);

  if(owned_params){
    for(i = 0; i < nParam * 2; i++){
      free(owned_params[i]);
    }
  }
  free(owned_params);
  free(azParam);
  free(db);
  free(jrnl);
  free(wal);
  return 0;
}