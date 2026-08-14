// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t rd32(const uint8_t *data, size_t size, size_t *off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4 && *off < size; i++, (*off)++){
    v = (v << 8) | data[*off];
  }
  return v;
}

static int pick_len(const uint8_t *data, size_t size, size_t *off, int maxlen){
  if(*off >= size) return 0;
  return (int)(rd32(data, size, off) % (uint32_t)(maxlen + 1));
}

static char *make_cstr(const uint8_t *data, size_t size, size_t *off, int maxlen){
  int len = pick_len(data, size, off, maxlen);
  char *s = (char *)malloc((size_t)len + 1);
  int i;
  if(!s) return NULL;
  for(i = 0; i < len; i++){
    unsigned char c = (*off < size) ? data[(*off)++] : 0;
    if(c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  return s;
}

static void touch_string(const char *s){
  if(s){
    volatile unsigned char x = (unsigned char)s[0];
    (void)x;
  }
}

int LLVMFuzzerTestOneInput_123(const uint8_t *Data, size_t Size){
  size_t off = 0;
  int i;
  int nParam;
  char *db = NULL, *jrnl = NULL, *wal = NULL;
  char *filename = NULL;
  const char **azParam = NULL;
  sqlite3 *dbconn = NULL;
  const char *dbfn = NULL;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  db = make_cstr(Data, Size, &off, 128);
  jrnl = make_cstr(Data, Size, &off, 128);
  wal = make_cstr(Data, Size, &off, 128);
  if(!db || !jrnl || !wal) goto cleanup;

  nParam = (int)(pick_len(Data, Size, &off, 8));
  if(nParam > 0){
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(char *));
    if(!azParam) goto cleanup;
    for(i = 0; i < nParam * 2; i++){
      char *s = make_cstr(Data, Size, &off, 64);
      if(!s) goto cleanup;
      if((i % 2) == 0 && s[0] == '\0'){
        free(s);
        s = (char *)malloc(2);
        if(!s) goto cleanup;
        s[0] = 'k';
        s[1] = '\0';
      }
      azParam[i] = s;
    }
  }

  filename = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);
  if(filename){
    const char *p_db = sqlite3_filename_database(filename);
    const char *p_j = sqlite3_filename_journal(filename);
    const char *p_w = sqlite3_filename_wal(filename);

    touch_string(p_db);
    touch_string(p_j);
    touch_string(p_w);

    if(nParam > 0){
      for(i = 0; i < nParam * 2; i += 2){
        const char *v = sqlite3_uri_parameter(filename, azParam[i]);
        touch_string(v);
      }
    }

    {
      char *query = make_cstr(Data, Size, &off, 64);
      if(query){
        const char *v = sqlite3_uri_parameter(filename, query);
        touch_string(v);
        free(query);
      }
    }

    touch_string(sqlite3_uri_parameter(filename, ""));
    touch_string(sqlite3_uri_parameter(NULL, "x"));
    touch_string(sqlite3_uri_parameter(filename, NULL));

    if(p_db) touch_string(sqlite3_uri_parameter(p_db, "mode"));
    if(p_j) touch_string(sqlite3_uri_parameter(p_j, "cache"));
    if(p_w) touch_string(sqlite3_uri_parameter(p_w, "vfs"));
  }

  if(sqlite3_open_v2("./dummy_file", &dbconn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI, NULL) == SQLITE_OK){
    dbfn = sqlite3_db_filename(dbconn, "main");
    if(dbfn){
      const char *a = sqlite3_filename_database(dbfn);
      const char *b = sqlite3_filename_journal(dbfn);
      const char *c = sqlite3_filename_wal(dbfn);
      touch_string(a);
      touch_string(b);
      touch_string(c);
      touch_string(sqlite3_uri_parameter(dbfn, "mode"));
      touch_string(sqlite3_uri_parameter(dbfn, "cache"));
      touch_string(sqlite3_uri_parameter(dbfn, "immutable"));
    }
  }

cleanup:
  if(dbconn) sqlite3_close(dbconn);
  if(filename) sqlite3_free_filename(filename);
  if(azParam){
    for(i = 0; i < nParam * 2; i++){
      free((void *)azParam[i]);
    }
    free(azParam);
  }
  free(db);
  free(jrnl);
  free(wal);
  return 0;
}