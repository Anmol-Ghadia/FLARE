// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static unsigned consume_u32(const uint8_t **pData, size_t *pSize){
  unsigned v = 0;
  size_t n = *pSize < 4 ? *pSize : 4;
  for(size_t i = 0; i < n; i++){
    v = (v << 8) | (*pData)[i];
  }
  *pData += n;
  *pSize -= n;
  return v;
}

static int consume_bool(const uint8_t **pData, size_t *pSize){
  if(*pSize == 0) return 0;
  int v = (**pData) & 1;
  (*pData)++;
  (*pSize)--;
  return v;
}

static char *consume_string(const uint8_t **pData, size_t *pSize, size_t max_len){
  size_t len;
  char *out;

  if(*pSize == 0){
    out = (char *)malloc(1);
    if(out) out[0] = '\0';
    return out;
  }

  len = consume_u32(pData, pSize);
  if(max_len > 0 && len > max_len) len = max_len;
  if(len > *pSize) len = *pSize;

  out = (char *)malloc(len + 1);
  if(!out) return NULL;

  for(size_t i = 0; i < len; i++){
    unsigned char c = (*pData)[i];
    if(c == '\0') c = 'A';
    out[i] = (char)c;
  }
  out[len] = '\0';

  *pData += len;
  *pSize -= len;
  return out;
}

static char *dup_cstr(const char *s){
  size_t len = strlen(s);
  char *out = (char *)malloc(len + 1);
  if(!out) return NULL;
  memcpy(out, s, len + 1);
  return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;

  char *db = NULL;
  char *jrnl = NULL;
  char *wal = NULL;
  char *param_query = NULL;
  char *dummy_content = NULL;
  const char **azParam = NULL;
  char *fname = NULL;
  int nParam = 0;

  db = consume_string(&p, &n, 128);
  jrnl = consume_string(&p, &n, 128);
  wal = consume_string(&p, &n, 128);
  param_query = consume_string(&p, &n, 64);
  dummy_content = consume_string(&p, &n, 256);

  if(!db || !jrnl || !wal || !param_query || !dummy_content) goto cleanup;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      fwrite(dummy_content, 1, strlen(dummy_content), fp);
      fclose(fp);
    }
  }

  nParam = (int)(consume_u32(&p, &n) % 4);
  if(nParam > 0){
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(char *));
    if(!azParam) goto cleanup;

    for(int i = 0; i < nParam * 2; i++){
      char *s = consume_string(&p, &n, 64);
      if(!s) goto cleanup;
      if((i % 2) == 0 && s[0] == '\0'){
        free(s);
        s = dup_cstr("k");
        if(!s) goto cleanup;
      }
      azParam[i] = s;
    }
  }

  if(consume_bool(&p, &n)){
    free(db);
    db = dup_cstr("./dummy_file");
    if(!db) goto cleanup;
  }
  if(consume_bool(&p, &n)){
    free(jrnl);
    jrnl = dup_cstr("./dummy_file-journal");
    if(!jrnl) goto cleanup;
  }
  if(consume_bool(&p, &n)){
    free(wal);
    wal = dup_cstr("./dummy_file-wal");
    if(!wal) goto cleanup;
  }

  fname = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);
  if(fname){
    const char *main_db = sqlite3_filename_database(fname);
    const char *journal_name = sqlite3_filename_journal(fname);
    const char *wal_name = sqlite3_filename_wal(fname);
    const char *param_val = sqlite3_uri_parameter(fname, param_query);

    (void)main_db;
    (void)journal_name;
    (void)wal_name;
    (void)param_val;

    if(main_db){
      (void)sqlite3_filename_database(main_db);
      (void)sqlite3_filename_journal(main_db);
      (void)sqlite3_filename_wal(main_db);
      (void)sqlite3_uri_parameter(main_db, param_query);
    }

    if(journal_name){
      (void)sqlite3_uri_parameter(journal_name, param_query);
    }

    if(wal_name){
      (void)sqlite3_uri_parameter(wal_name, param_query);
    }

    sqlite3_free_filename(fname);
    fname = NULL;
  }

  (void)sqlite3_uri_parameter(NULL, param_query);

cleanup:
  if(fname) sqlite3_free_filename(fname);
  if(azParam){
    for(int i = 0; i < nParam * 2; i++){
      free((void *)azParam[i]);
    }
    free(azParam);
  }
  free(db);
  free(jrnl);
  free(wal);
  free(param_query);
  free(dummy_content);
  return 0;
}