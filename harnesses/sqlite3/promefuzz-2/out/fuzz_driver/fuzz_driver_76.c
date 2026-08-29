// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t i, m = *n < 4 ? *n : 4;
  for(i = 0; i < m; i++){
    v = (v << 8) | (*p)[i];
  }
  *p += m;
  *n -= m;
  return v;
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t max_len){
  size_t len;
  char *s;
  if(*n == 0){
    s = (char*)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }
  len = read_u32(p, n);
  if(max_len > 0) len %= max_len;
  if(len > *n) len = *n;
  s = (char*)malloc(len + 1);
  if(!s) return NULL;
  if(len) memcpy(s, *p, len);
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static char *dup_or_default(char *s, const char *defval){
  size_t dlen;
  char *out;
  if(s && s[0] != '\0') return s;
  free(s);
  dlen = strlen(defval);
  out = (char*)malloc(dlen + 1);
  if(!out) return NULL;
  memcpy(out, defval, dlen + 1);
  return out;
}

static void exercise_filename(const char *fname, const char *param1, const char *param2){
  const char *db;
  const char *jr;
  const char *wal;

  if(!fname) return;

  db = sqlite3_filename_database(fname);
  jr = sqlite3_filename_journal(fname);
  wal = sqlite3_filename_wal(fname);

  (void)sqlite3_uri_parameter(fname, param1);
  (void)sqlite3_uri_parameter(fname, param2);
  (void)sqlite3_uri_parameter(db, param1);
  (void)sqlite3_uri_parameter(jr, param1);
  (void)sqlite3_uri_parameter(wal, param1);

  if(db){
    (void)sqlite3_filename_database(db);
    (void)sqlite3_filename_journal(db);
    (void)sqlite3_filename_wal(db);
  }

  if(jr){
    (void)sqlite3_filename_database(jr);
    (void)sqlite3_filename_journal(jr);
    (void)sqlite3_filename_wal(jr);
    (void)sqlite3_uri_parameter(jr, param2);
  }

  if(wal){
    (void)sqlite3_filename_database(wal);
    (void)sqlite3_filename_journal(wal);
    (void)sqlite3_filename_wal(wal);
    (void)sqlite3_uri_parameter(wal, param2);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;
  char *db = NULL, *jr = NULL, *wal = NULL;
  char *k1 = NULL, *v1 = NULL, *k2 = NULL, *v2 = NULL;
  char *query1 = NULL, *query2 = NULL;
  char *created = NULL;
  const char *params[4];
  int nParam = 0;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  db = make_cstring(&p, &n, 256);
  jr = make_cstring(&p, &n, 256);
  wal = make_cstring(&p, &n, 256);
  k1 = make_cstring(&p, &n, 64);
  v1 = make_cstring(&p, &n, 128);
  k2 = make_cstring(&p, &n, 64);
  v2 = make_cstring(&p, &n, 128);
  query1 = make_cstring(&p, &n, 64);
  query2 = make_cstring(&p, &n, 64);

  if(!db || !jr || !wal || !k1 || !v1 || !k2 || !v2 || !query1 || !query2){
    free(db); free(jr); free(wal);
    free(k1); free(v1); free(k2); free(v2);
    free(query1); free(query2);
    return 0;
  }

  db = dup_or_default(db, "./dummy_file");
  jr = dup_or_default(jr, "./dummy_file-journal");
  wal = dup_or_default(wal, "./dummy_file-wal");
  k1 = dup_or_default(k1, "k1");
  k2 = dup_or_default(k2, "k2");
  query1 = dup_or_default(query1, "k1");
  query2 = dup_or_default(query2, "k2");

  if(!db || !jr || !wal || !k1 || !k2 || !query1 || !query2){
    free(db); free(jr); free(wal);
    free(k1); free(v1); free(k2); free(v2);
    free(query1); free(query2);
    return 0;
  }

  switch(read_u32(&p, &n) % 3){
    case 0:
      nParam = 0;
      created = sqlite3_create_filename(db, jr, wal, 0, NULL);
      break;
    case 1:
      nParam = 1;
      params[0] = k1;
      params[1] = v1;
      created = sqlite3_create_filename(db, jr, wal, nParam, params);
      break;
    default:
      nParam = 2;
      params[0] = k1;
      params[1] = v1;
      params[2] = k2;
      params[3] = v2;
      created = sqlite3_create_filename(db, jr, wal, nParam, params);
      break;
  }

  if(created){
    const char *jname;
    const char *wname;

    exercise_filename(created, query1, query2);

    if(nParam > 0){
      (void)sqlite3_uri_parameter(created, k1);
      jname = sqlite3_filename_journal(created);
      wname = sqlite3_filename_wal(created);
      (void)sqlite3_uri_parameter(jname, k1);
      (void)sqlite3_uri_parameter(wname, k1);
    }
    if(nParam > 1){
      (void)sqlite3_uri_parameter(created, k2);
      jname = sqlite3_filename_journal(created);
      wname = sqlite3_filename_wal(created);
      (void)sqlite3_uri_parameter(jname, k2);
      (void)sqlite3_uri_parameter(wname, k2);
    }

    sqlite3_free_filename(created);
  }

  free(db);
  free(jr);
  free(wal);
  free(k1);
  free(v1);
  free(k2);
  free(v2);
  free(query1);
  free(query2);
  return 0;
}