// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_boolean at sqlite3.c:156684:16 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
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

static int read_i32(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static sqlite3_int64 read_i64(const uint8_t **p, size_t *n){
  sqlite3_int64 v = 0;
  size_t i, m = *n < 8 ? *n : 8;
  for(i = 0; i < m; i++){
    v = (v << 8) | (*p)[i];
  }
  *p += m;
  *n -= m;
  return v;
}

static char *take_string(const uint8_t **p, size_t *n, size_t max_len){
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
    unsigned char c = (*p)[i];
    if(c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';

  *p += len;
  *n -= len;
  return s;
}

static void write_dummy_file(const uint8_t *data, size_t size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(!fp) return;
  if(data && size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  char *open_uri = NULL;
  char *created = NULL;
  const char *dbfile = NULL;
  const char *param_name1 = NULL;
  const char *param_name2 = NULL;
  const char *param_name3 = NULL;
  char *owned_param1 = NULL;
  char *owned_param2 = NULL;
  char *owned_param3 = NULL;
  char *db_name = NULL;
  char *zDatabase = NULL;
  char *zJournal = NULL;
  char *zWal = NULL;
  const char **azParam = NULL;
  int rc;
  int i, nParamPairs;

  write_dummy_file(Data, Size);

  rc = sqlite3_initialize();
  if(rc != SQLITE_OK){
    return 0;
  }

  owned_param1 = take_string(&p, &n, 64);
  owned_param2 = take_string(&p, &n, 64);
  owned_param3 = take_string(&p, &n, 64);
  db_name = take_string(&p, &n, 32);
  zDatabase = take_string(&p, &n, 128);
  zJournal = take_string(&p, &n, 128);
  zWal = take_string(&p, &n, 128);
  open_uri = take_string(&p, &n, 256);

  if(!owned_param1 || !owned_param2 || !owned_param3 || !db_name ||
     !zDatabase || !zJournal || !zWal || !open_uri){
    goto cleanup;
  }

  param_name1 = owned_param1[0] ? owned_param1 : "mode";
  param_name2 = owned_param2[0] ? owned_param2 : "cache";
  param_name3 = owned_param3[0] ? owned_param3 : "immutable";

  if(zDatabase[0] == '\0'){
    free(zDatabase);
    zDatabase = strdup("./dummy_file");
    if(!zDatabase) goto cleanup;
  }
  if(zJournal[0] == '\0'){
    free(zJournal);
    zJournal = strdup("./dummy_file-journal");
    if(!zJournal) goto cleanup;
  }
  if(zWal[0] == '\0'){
    free(zWal);
    zWal = strdup("./dummy_file-wal");
    if(!zWal) goto cleanup;
  }

  nParamPairs = (int)(read_u32(&p, &n) % 4);
  if(nParamPairs > 0){
    azParam = (const char **)calloc((size_t)nParamPairs * 2, sizeof(char *));
    if(!azParam) goto cleanup;
    for(i = 0; i < nParamPairs * 2; i += 2){
      char *k = take_string(&p, &n, 64);
      char *v = take_string(&p, &n, 128);
      if(!k || !v){
        free(k);
        free(v);
        goto cleanup;
      }
      if(k[0] == '\0'){
        k[0] = 'k';
        if(k[1] == '\0') k[1] = '\0';
      }
      azParam[i] = k;
      azParam[i + 1] = v;
    }
  }

  created = sqlite3_create_filename(
      zDatabase,
      zJournal,
      zWal,
      nParamPairs,
      azParam
  );

  if(created){
    (void)sqlite3_uri_boolean(created, param_name1, read_i32(&p, &n));
    (void)sqlite3_uri_boolean(created, param_name2, read_i32(&p, &n));
    (void)sqlite3_uri_boolean(created, param_name3, read_i32(&p, &n));

    (void)sqlite3_uri_parameter(created, param_name1);
    (void)sqlite3_uri_parameter(created, param_name2);
    (void)sqlite3_uri_parameter(created, param_name3);
    (void)sqlite3_uri_parameter(created, "");
    (void)sqlite3_uri_parameter(created, NULL);

    (void)sqlite3_uri_int64(created, param_name1, read_i64(&p, &n));
    (void)sqlite3_uri_int64(created, param_name2, read_i64(&p, &n));
    (void)sqlite3_uri_int64(created, param_name3, read_i64(&p, &n));

    for(i = -1; i < 8; i++){
      (void)sqlite3_uri_key(created, i);
    }
  }

  if(open_uri[0] == '\0'){
    free(open_uri);
    open_uri = strdup("file:./dummy_file?mode=rwc&cache=shared");
    if(!open_uri) goto cleanup;
  }

  rc = sqlite3_open_v2(open_uri, &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
                       NULL);
  if(rc == SQLITE_OK && db){
    const char *names[] = {
      db_name[0] ? db_name : "main",
      "main",
      "temp",
      "aux",
      ""
    };
    size_t j;
    for(j = 0; j < sizeof(names)/sizeof(names[0]); j++){
      dbfile = sqlite3_db_filename(db, names[j]);
      if(dbfile){
        (void)sqlite3_uri_parameter(dbfile, param_name1);
        (void)sqlite3_uri_parameter(dbfile, param_name2);
        (void)sqlite3_uri_parameter(dbfile, param_name3);

        (void)sqlite3_uri_boolean(dbfile, param_name1, read_i32(&p, &n));
        (void)sqlite3_uri_boolean(dbfile, param_name2, read_i32(&p, &n));
        (void)sqlite3_uri_boolean(dbfile, param_name3, read_i32(&p, &n));

        (void)sqlite3_uri_int64(dbfile, param_name1, read_i64(&p, &n));
        (void)sqlite3_uri_int64(dbfile, param_name2, read_i64(&p, &n));
        (void)sqlite3_uri_int64(dbfile, param_name3, read_i64(&p, &n));

        for(i = -1; i < 8; i++){
          (void)sqlite3_uri_key(dbfile, i);
        }
      }
    }
  }

  (void)sqlite3_uri_boolean(NULL, param_name1, read_i32(&p, &n));
  (void)sqlite3_uri_boolean(NULL, NULL, read_i32(&p, &n));
  (void)sqlite3_uri_parameter(NULL, param_name1);
  (void)sqlite3_uri_parameter(NULL, NULL);
  (void)sqlite3_uri_int64(NULL, param_name1, read_i64(&p, &n));
  (void)sqlite3_uri_key(NULL, -1);
  (void)sqlite3_uri_key(NULL, 0);

cleanup:
  if(db){
    sqlite3_close(db);
  }
  if(created){
    sqlite3_free_filename(created);
  }
  if(azParam){
    for(i = 0; i < nParamPairs * 2; i++){
      free((void *)azParam[i]);
    }
    free(azParam);
  }
  free(open_uri);
  free(owned_param1);
  free(owned_param2);
  free(owned_param3);
  free(db_name);
  free(zDatabase);
  free(zJournal);
  free(zWal);
  return 0;
}