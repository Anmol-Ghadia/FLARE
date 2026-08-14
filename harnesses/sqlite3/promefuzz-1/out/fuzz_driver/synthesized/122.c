// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static size_t take_u8(const uint8_t *Data, size_t Size, size_t *Off){
  if(*Off >= Size) return 0;
  return Data[(*Off)++];
}

static int take_bool(const uint8_t *Data, size_t Size, size_t *Off){
  return (int)(take_u8(Data, Size, Off) & 1u);
}

static int take_int(const uint8_t *Data, size_t Size, size_t *Off, int maxv){
  if(maxv <= 0) return 0;
  return (int)(take_u8(Data, Size, Off) % (unsigned)maxv);
}

static char *take_cstring(const uint8_t *Data, size_t Size, size_t *Off, size_t maxlen){
  size_t remain, n;
  char *s;
  if(*Off > Size) return NULL;
  remain = Size - *Off;
  n = remain < maxlen ? remain : maxlen;
  s = (char *)malloc(n + 1);
  if(!s) return NULL;
  if(n) memcpy(s, Data + *Off, n);
  s[n] = '\0';
  *Off += n;
  return s;
}

static void write_dummy_file(const uint8_t *Data, size_t Size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(!fp) return;
  if(Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_122(const uint8_t *Data, size_t Size){
  size_t off = 0;
  sqlite3 *db = NULL;
  char *zDb = NULL, *zJournal = NULL, *zWal = NULL;
  char *created = NULL;
  const char *dbfn = NULL;
  const char *main_name = NULL;
  const char *jrnl_name = NULL;
  const char *wal_name = NULL;
  const char **azParam = NULL;
  int nParam = 0;
  int i;

  write_dummy_file(Data, Size);

  zDb = take_cstring(Data, Size, &off, 64);
  zJournal = take_cstring(Data, Size, &off, 64);
  zWal = take_cstring(Data, Size, &off, 64);
  if(!zDb || !zJournal || !zWal) goto cleanup;

  if(take_bool(Data, Size, &off)){
    free(zDb);
    zDb = strdup("./dummy_file");
    if(!zDb) goto cleanup;
  }
  if(take_bool(Data, Size, &off)){
    free(zJournal);
    zJournal = strdup("./dummy_file-journal");
    if(!zJournal) goto cleanup;
  }
  if(take_bool(Data, Size, &off)){
    free(zWal);
    zWal = strdup("./dummy_file-wal");
    if(!zWal) goto cleanup;
  }

  nParam = take_int(Data, Size, &off, 4);
  if(nParam > 0){
    azParam = (const char **)calloc((size_t)nParam * 2u, sizeof(char *));
    if(!azParam) goto cleanup;
    for(i = 0; i < nParam; i++){
      char *k = take_cstring(Data, Size, &off, 16);
      char *v = take_cstring(Data, Size, &off, 32);
      if(!k || !v){
        free(k);
        free(v);
        nParam = i;
        goto create_filename;
      }
      if(k[0] == '\0'){
        free(k);
        k = strdup("k");
        if(!k){
          free(v);
          nParam = i;
          goto create_filename;
        }
      }
      azParam[i * 2] = k;
      azParam[i * 2 + 1] = v;
    }
  }

create_filename:
  created = sqlite3_create_filename(
      zDb ? zDb : "",
      zJournal ? zJournal : "",
      zWal ? zWal : "",
      nParam,
      azParam
  );

  if(created){
    main_name = sqlite3_filename_database(created);
    jrnl_name = sqlite3_filename_journal(created);
    wal_name = sqlite3_filename_wal(created);

    if(main_name) (void)sqlite3_filename_database(main_name);
    if(jrnl_name) (void)sqlite3_filename_journal(created);
    if(wal_name) (void)sqlite3_filename_wal(created);
  }

  if(sqlite3_open("./dummy_file", &db) == SQLITE_OK && db){
    dbfn = sqlite3_db_filename(db, "main");
    if(dbfn){
      (void)sqlite3_filename_database(dbfn);
      (void)sqlite3_filename_journal(dbfn);
      (void)sqlite3_filename_wal(dbfn);
    }

    if(take_bool(Data, Size, &off)){
      (void)sqlite3_exec(db, "ATTACH './dummy_file' AS aux;", NULL, NULL, NULL);
      {
        const char *auxfn = sqlite3_db_filename(db, "aux");
        if(auxfn){
          (void)sqlite3_filename_database(auxfn);
          (void)sqlite3_filename_journal(auxfn);
          (void)sqlite3_filename_wal(auxfn);
        }
      }
      (void)sqlite3_exec(db, "DETACH aux;", NULL, NULL, NULL);
    }
  }

cleanup:
  if(db) sqlite3_close(db);
  if(created) sqlite3_free_filename(created);
  if(azParam){
    for(i = 0; i < nParam * 2; i++){
      free((void *)azParam[i]);
    }
    free(azParam);
  }
  free(zDb);
  free(zJournal);
  free(zWal);
  return 0;
}