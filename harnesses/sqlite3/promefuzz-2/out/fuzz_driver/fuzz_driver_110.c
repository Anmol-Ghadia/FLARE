// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t fuzz_u32(const uint8_t *Data, size_t Size, size_t *Off){
  uint32_t v = 0;
  for(size_t i = 0; i < 4; i++){
    v <<= 8;
    if(*Off < Size) v |= Data[(*Off)++];
  }
  return v;
}

static int fuzz_bool(const uint8_t *Data, size_t Size, size_t *Off){
  if(*Off >= Size) return 0;
  return (int)(Data[(*Off)++] & 1);
}

static char *fuzz_string(const uint8_t *Data, size_t Size, size_t *Off, size_t maxlen){
  size_t remain = (*Off < Size) ? (Size - *Off) : 0;
  size_t len = remain ? (fuzz_u32(Data, Size, Off) % (maxlen + 1)) : 0;
  if(*Off > Size) *Off = Size;
  if(len > Size - *Off) len = Size - *Off;

  char *s = (char *)malloc(len + 1);
  if(!s) return NULL;

  for(size_t i = 0; i < len; i++){
    unsigned char c = Data[(*Off)++];
    if(c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  return s;
}

static char *dup_cstr(const char *s){
  size_t n = strlen(s);
  char *r = (char *)malloc(n + 1);
  if(!r) return NULL;
  memcpy(r, s, n + 1);
  return r;
}

static void write_dummy_file(const uint8_t *Data, size_t Size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(!fp) return;
  if(Data && Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t off = 0;
  char *db = NULL;
  char *jrnl = NULL;
  char *wal = NULL;
  char **params = NULL;
  char *fname = NULL;
  int nParam = 0;

  write_dummy_file(Data, Size);

  db = fuzz_bool(Data, Size, &off) ? dup_cstr("./dummy_file") : fuzz_string(Data, Size, &off, 64);
  jrnl = fuzz_bool(Data, Size, &off) ? dup_cstr("./dummy_file-journal") : fuzz_string(Data, Size, &off, 64);
  wal = fuzz_bool(Data, Size, &off) ? dup_cstr("./dummy_file-wal") : fuzz_string(Data, Size, &off, 64);

  if(!db) db = dup_cstr("");
  if(!jrnl) jrnl = dup_cstr("");
  if(!wal) wal = dup_cstr("");
  if(!db || !jrnl || !wal) goto cleanup;

  nParam = (int)(fuzz_u32(Data, Size, &off) % 8);
  if(nParam > 0){
    params = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    if(!params) goto cleanup;
    for(int i = 0; i < nParam; i++){
      params[2*i] = fuzz_string(Data, Size, &off, 32);
      params[2*i+1] = fuzz_string(Data, Size, &off, 32);

      if(!params[2*i] || params[2*i][0] == '\0'){
        free(params[2*i]);
        params[2*i] = dup_cstr("k");
      }
      if(!params[2*i+1]){
        params[2*i+1] = dup_cstr("");
      }
      if(!params[2*i] || !params[2*i+1]) goto cleanup;
    }
  }

  fname = sqlite3_create_filename(db, jrnl, wal, nParam, (const char **)params);
  if(fname){
    const char *db_name = sqlite3_filename_database(fname);
    const char *jrnl_name = sqlite3_filename_journal(fname);
    const char *wal_name = sqlite3_filename_wal(fname);

    (void)db_name;
    (void)jrnl_name;
    (void)wal_name;

    (void)sqlite3_uri_parameter(fname, "k");
    (void)sqlite3_uri_parameter(fname, "mode");
    (void)sqlite3_uri_parameter(fname, "cache");
    (void)sqlite3_uri_parameter(NULL, "k");
    (void)sqlite3_uri_parameter(fname, NULL);

    for(int i = 0; i < nParam; i++){
      (void)sqlite3_uri_parameter(fname, params[2*i]);
    }

    if(jrnl_name){
      (void)sqlite3_uri_parameter(jrnl_name, "k");
      (void)sqlite3_uri_parameter(jrnl_name, "mode");
    }

    if(wal_name){
      (void)sqlite3_uri_parameter(wal_name, "k");
      (void)sqlite3_uri_parameter(wal_name, "cache");
    }

    sqlite3_free_filename(fname);
  }

cleanup:
  if(params){
    for(int i = 0; i < nParam * 2; i++){
      free(params[i]);
    }
    free(params);
  }
  free(db);
  free(jrnl);
  free(wal);
  return 0;
}