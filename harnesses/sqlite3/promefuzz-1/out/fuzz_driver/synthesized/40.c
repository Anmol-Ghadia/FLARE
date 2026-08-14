// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
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

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static size_t take_bytes(const uint8_t **pData, size_t *pSize, char *out, size_t max_out){
  size_t n = 0;
  if(max_out == 0) return 0;
  while(*pSize > 0 && n + 1 < max_out){
    uint8_t b = **pData;
    (*pData)++;
    (*pSize)--;
    if(b == 0) break;
    out[n++] = (char)b;
  }
  out[n] = '\0';
  return n;
}

static void sanitize_string(char *s){
  while(*s){
    unsigned char c = (unsigned char)*s;
    if(c < 32 || c == 127) *s = 'A' + (c % 26);
    s++;
  }
}

int LLVMFuzzerTestOneInput_40(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  char *created = NULL;
  FILE *fp = NULL;

  char dbname[128];
  char journal[128];
  char wal[128];
  char keybuf[8][64];
  char valbuf[8][64];
  const char *params[16];
  int nParam = 0;
  int i;

  if(Size < 8){
    sqlite3_uri_parameter(NULL, NULL);
    sqlite3_uri_int64(NULL, NULL, 0);
    sqlite3_uri_key(NULL, 0);
    sqlite3_free_filename(NULL);
    return 0;
  }

  memset(dbname, 0, sizeof(dbname));
  memset(journal, 0, sizeof(journal));
  memset(wal, 0, sizeof(wal));
  memset(keybuf, 0, sizeof(keybuf));
  memset(valbuf, 0, sizeof(valbuf));

  {
    const uint8_t *p = Data;
    size_t rem = Size;

    take_bytes(&p, &rem, dbname, sizeof(dbname));
    take_bytes(&p, &rem, journal, sizeof(journal));
    take_bytes(&p, &rem, wal, sizeof(wal));

    sanitize_string(dbname);
    sanitize_string(journal);
    sanitize_string(wal);

    if(dbname[0] == '\0') strcpy(dbname, "./dummy_file");
    if(journal[0] == '\0') strcpy(journal, "./dummy_file-journal");
    if(wal[0] == '\0') strcpy(wal, "./dummy_file-wal");

    if(rem > 0){
      nParam = (int)(*p % 8);
      p++;
      rem--;
    }

    for(i = 0; i < nParam; i++){
      take_bytes(&p, &rem, keybuf[i], sizeof(keybuf[i]));
      take_bytes(&p, &rem, valbuf[i], sizeof(valbuf[i]));
      sanitize_string(keybuf[i]);
      sanitize_string(valbuf[i]);

      if(keybuf[i][0] == '\0'){
        snprintf(keybuf[i], sizeof(keybuf[i]), "k%d", i);
      }
      params[2*i] = keybuf[i];
      params[2*i + 1] = valbuf[i];
    }

    fp = fopen("./dummy_file", "wb");
    if(fp != NULL){
      fwrite(Data, 1, Size, fp);
      fclose(fp);
      fp = NULL;
    }

    created = sqlite3_create_filename(dbname, journal, wal, nParam, nParam ? params : NULL);
    if(created != NULL){
      const char *r;
      sqlite3_int64 v;

      r = sqlite3_uri_parameter(created, "mode");
      (void)r;
      r = sqlite3_uri_parameter(created, "cache");
      (void)r;
      r = sqlite3_uri_parameter(created, "immutable");
      (void)r;

      for(i = -1; i < nParam + 3; i++){
        r = sqlite3_uri_key(created, i);
        (void)r;
      }

      for(i = 0; i < nParam; i++){
        r = sqlite3_uri_parameter(created, keybuf[i]);
        (void)r;
        v = sqlite3_uri_int64(created, keybuf[i], (sqlite3_int64)i);
        (void)v;
      }

      r = sqlite3_filename_database(created);
      (void)r;

      v = sqlite3_uri_int64(created, "v", -1);
      (void)v;
      v = sqlite3_uri_int64(created, "hex", 123);
      (void)v;
    }

    {
      int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
      int rc;
      char openname[512];
      uint32_t x = (rem >= 4) ? rd32(p) : 0;

      snprintf(openname, sizeof(openname),
               "file:./dummy_file?mode=%s&cache=%s&v=%u&hex=0x%x&k0=%s",
               (x & 1) ? "ro" : "rwc",
               (x & 2) ? "shared" : "private",
               (unsigned)(x % 100000U),
               (unsigned)x,
               (nParam > 0 && keybuf[0][0]) ? keybuf[0] : "default");

      rc = sqlite3_open_v2(openname, &db, flags, NULL);
      if(rc == SQLITE_OK && db != NULL){
        const char *fname;

        fname = sqlite3_db_filename(db, "main");
        if(fname != NULL){
          const char *r;
          sqlite3_int64 v;

          r = sqlite3_uri_parameter(fname, "mode");
          (void)r;
          r = sqlite3_uri_parameter(fname, "cache");
          (void)r;
          r = sqlite3_uri_parameter(fname, "v");
          (void)r;

          v = sqlite3_uri_int64(fname, "v", 0);
          (void)v;
          v = sqlite3_uri_int64(fname, "hex", -7);
          (void)v;

          for(i = -1; i < 8; i++){
            r = sqlite3_uri_key(fname, i);
            (void)r;
          }

          r = sqlite3_filename_database(fname);
          (void)r;
        }

        fname = sqlite3_db_filename(db, "temp");
        if(fname != NULL && fname[0] != '\0'){
          const char *r = sqlite3_filename_database(fname);
          (void)r;
        }
      }
    }
  }

  if(db != NULL){
    sqlite3_close(db);
    db = NULL;
  }
  if(created != NULL){
    sqlite3_free_filename(created);
    created = NULL;
  }

  sqlite3_uri_parameter(NULL, "x");
  sqlite3_uri_parameter(created, NULL);
  sqlite3_uri_int64(NULL, "x", 42);
  sqlite3_uri_key(NULL, -1);

  return 0;
}