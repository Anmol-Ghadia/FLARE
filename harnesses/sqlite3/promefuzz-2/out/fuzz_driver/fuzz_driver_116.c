// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
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
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_database_file_object at sqlite3.c:45334:26 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *Data, size_t Size, size_t *Off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4 && *Off < Size; i++, (*Off)++){
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static int rdbool(const uint8_t *Data, size_t Size, size_t *Off){
  if(*Off >= Size) return 0;
  return (int)(Data[(*Off)++] & 1);
}

static size_t rdlen(const uint8_t *Data, size_t Size, size_t *Off, size_t Max){
  if(*Off >= Size) return 0;
  return (size_t)(rd32(Data, Size, Off) % (Max + 1));
}

static char *dup_segment(const uint8_t *Data, size_t Size, size_t *Off, size_t MaxLen){
  size_t n = rdlen(Data, Size, Off, MaxLen);
  char *s = (char *)malloc(n + 1);
  size_t i;
  if(!s) return NULL;
  for(i = 0; i < n; i++){
    if(*Off < Size){
      unsigned char c = Data[(*Off)++];
      if(c == 0) c = 'A';
      s[i] = (char)c;
    }else{
      s[i] = 'B';
    }
  }
  s[n] = '\0';
  return s;
}

static char *make_nonempty_string(const uint8_t *Data, size_t Size, size_t *Off, size_t MaxLen){
  char *s = dup_segment(Data, Size, Off, MaxLen);
  if(!s) return NULL;
  if(s[0] == '\0'){
    free(s);
    s = (char *)malloc(2);
    if(!s) return NULL;
    s[0] = 'k';
    s[1] = '\0';
  }
  return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t off = 0;
  sqlite3 *db = NULL;
  char *errmsg = NULL;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, &errmsg);
  if(errmsg){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, &errmsg);
  if(errmsg){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  for(i = 0; i < 3; i++){
    char sql[128];
    int v = (int)rd32(Data, Size, &off);
    snprintf(sql, sizeof(sql), "INSERT INTO t(x) VALUES(%d);", v);
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if(errmsg){
      sqlite3_free(errmsg);
      errmsg = NULL;
    }
  }

  {
    const char *dbfn = sqlite3_db_filename(db, "main");
    if(dbfn){
      const char *j1 = sqlite3_filename_journal(dbfn);
      const char *w1 = sqlite3_filename_wal(dbfn);
      const char *d1 = sqlite3_filename_database(dbfn);

      (void)d1;
      if(j1){
        (void)sqlite3_filename_database(j1);
        (void)sqlite3_filename_journal(j1);
        (void)sqlite3_filename_wal(j1);
        (void)sqlite3_uri_parameter(j1, "mode");
      }
      if(w1){
        (void)sqlite3_filename_database(w1);
        (void)sqlite3_filename_journal(w1);
        (void)sqlite3_filename_wal(w1);
        (void)sqlite3_uri_parameter(w1, "cache");
      }
      (void)sqlite3_uri_parameter(dbfn, "mode");
      (void)sqlite3_uri_parameter(dbfn, "cache");
      (void)sqlite3_uri_parameter(dbfn, "vfs");
      (void)sqlite3_database_file_object(dbfn);
    }
  }

  {
    char *zDb = dup_segment(Data, Size, &off, 64);
    char *zJ = dup_segment(Data, Size, &off, 64);
    char *zW = dup_segment(Data, Size, &off, 64);
    int nParam = (int)(rd32(Data, Size, &off) % 4);
    const char **azParam = NULL;
    char **owned = NULL;

    if(!zDb || !zJ || !zW){
      free(zDb);
      free(zJ);
      free(zW);
      sqlite3_close(db);
      return 0;
    }

    if(rdbool(Data, Size, &off)){
      free(zDb);
      zDb = strdup("./dummy_file");
      if(!zDb){
        free(zJ);
        free(zW);
        sqlite3_close(db);
        return 0;
      }
    }

    if(nParam > 0){
      int total = nParam * 2;
      azParam = (const char **)calloc((size_t)total, sizeof(char *));
      owned = (char **)calloc((size_t)total, sizeof(char *));
      if(!azParam || !owned){
        free((void *)azParam);
        free(owned);
        free(zDb);
        free(zJ);
        free(zW);
        sqlite3_close(db);
        return 0;
      }
      for(i = 0; i < total; i += 2){
        owned[i] = make_nonempty_string(Data, Size, &off, 32);
        owned[i + 1] = dup_segment(Data, Size, &off, 32);
        if(!owned[i] || !owned[i + 1]){
          int k;
          for(k = 0; k < total; k++) free(owned[k]);
          free(owned);
          free((void *)azParam);
          free(zDb);
          free(zJ);
          free(zW);
          sqlite3_close(db);
          return 0;
        }
        azParam[i] = owned[i];
        azParam[i + 1] = owned[i + 1];
      }
    }

    {
      char *fname = sqlite3_create_filename(
        zDb, zJ, zW, nParam, azParam
      );
      if(fname){
        const char *db2 = sqlite3_filename_database(fname);
        const char *j2 = sqlite3_filename_journal(fname);
        const char *w2 = sqlite3_filename_wal(fname);

        (void)db2;
        (void)sqlite3_uri_parameter(fname, "mode");
        (void)sqlite3_uri_parameter(fname, "cache");
        (void)sqlite3_uri_parameter(fname, "immutable");

        if(nParam > 0){
          for(i = 0; i < nParam * 2; i += 2){
            (void)sqlite3_uri_parameter(fname, azParam[i]);
          }
        }

        if(j2){
          (void)sqlite3_filename_database(j2);
          (void)sqlite3_filename_journal(j2);
          (void)sqlite3_filename_wal(j2);
          (void)sqlite3_uri_parameter(j2, "mode");
        }
        if(w2){
          (void)sqlite3_filename_database(w2);
          (void)sqlite3_filename_journal(w2);
          (void)sqlite3_filename_wal(w2);
          (void)sqlite3_uri_parameter(w2, "cache");
        }

        sqlite3_free_filename(fname);
      }
    }

    if(owned){
      for(i = 0; i < nParam * 2; i++) free(owned[i]);
      free(owned);
    }
    free((void *)azParam);
    free(zDb);
    free(zJ);
    free(zW);
  }

  sqlite3_close(db);
  return 0;
}