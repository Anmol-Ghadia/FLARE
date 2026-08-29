// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
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
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static size_t take_u32(const uint8_t *Data, size_t Size, size_t *Off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4; i++){
    v <<= 8;
    if(*Off < Size) v |= Data[(*Off)++];
  }
  return (size_t)v;
}

static char *make_cstring(const uint8_t *Data, size_t Size, size_t *Off){
  size_t rem, len;
  char *s;
  if(*Off >= Size){
    s = (char *)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }
  rem = Size - *Off;
  len = take_u32(Data, Size, Off);
  if(rem == 0) len = 0;
  else len %= (rem + 1);
  if(*Off + len > Size) len = Size - *Off;

  s = (char *)malloc(len + 1);
  if(!s) return NULL;
  if(len) memcpy(s, Data + *Off, len);
  s[len] = '\0';
  *Off += len;
  return s;
}

static char *make_nonnull_string(const uint8_t *Data, size_t Size, size_t *Off, const char *fallback){
  char *s = make_cstring(Data, Size, Off);
  if(!s){
    size_t n = strlen(fallback);
    s = (char *)malloc(n + 1);
    if(!s) return NULL;
    memcpy(s, fallback, n + 1);
  }
  return s;
}

int LLVMFuzzerTestOneInput_42(const uint8_t *Data, size_t Size){
  size_t off = 0;
  char *db = NULL, *jr = NULL, *wal = NULL;
  char *param_storage[16];
  const char *params[16];
  int i, nParam;
  char *fname = NULL;

  memset(param_storage, 0, sizeof(param_storage));
  memset(params, 0, sizeof(params));

  db = make_nonnull_string(Data, Size, &off, "./dummy_file");
  jr = make_nonnull_string(Data, Size, &off, "./dummy_file-journal");
  wal = make_nonnull_string(Data, Size, &off, "./dummy_file-wal");
  if(!db || !jr || !wal) goto cleanup;

  nParam = (int)(take_u32(Data, Size, &off) % 8);
  for(i = 0; i < nParam * 2; i++){
    const char *fallback = (i % 2 == 0) ? "k" : "v";
    param_storage[i] = make_nonnull_string(Data, Size, &off, fallback);
    if(!param_storage[i]) goto cleanup;
    if(i % 2 == 0 && param_storage[i][0] == '\0'){
      free(param_storage[i]);
      param_storage[i] = (char *)malloc(2);
      if(!param_storage[i]) goto cleanup;
      param_storage[i][0] = 'k';
      param_storage[i][1] = '\0';
    }
    params[i] = param_storage[i];
  }

  fname = sqlite3_create_filename(db, jr, wal, nParam, nParam ? params : NULL);
  if(fname){
    const char *fdb = sqlite3_filename_database(fname);
    const char *fjr = sqlite3_filename_journal(fname);
    const char *fwal = sqlite3_filename_wal(fname);

    /* Only call filename translation helpers on the original valid bundle
       pointer returned by sqlite3_create_filename(). The returned sub-pointers
       are safe for sqlite3_uri_parameter/sqlite3_uri_key, but feeding journal
       or wal pointers back into filename_wal()/filename_journal() can walk
       past internal storage boundaries. */
    (void)fdb;
    (void)fjr;
    (void)fwal;
    (void)sqlite3_filename_database(fname);
    (void)sqlite3_filename_journal(fname);
    (void)sqlite3_filename_wal(fname);

    for(i = -1; i < nParam + 3; i++){
      const char *k = sqlite3_uri_key(fname, i);
      if(k){
        (void)sqlite3_uri_parameter(fname, k);
        if(fdb) (void)sqlite3_uri_parameter(fdb, k);
        if(fjr) (void)sqlite3_uri_parameter(fjr, k);
        if(fwal) (void)sqlite3_uri_parameter(fwal, k);
      }
    }

    for(i = 0; i < nParam * 2; i += 2){
      (void)sqlite3_uri_parameter(fname, params[i]);
      if(fdb) (void)sqlite3_uri_parameter(fdb, params[i]);
      if(fjr) (void)sqlite3_uri_parameter(fjr, params[i]);
      if(fwal) (void)sqlite3_uri_parameter(fwal, params[i]);
    }

    (void)sqlite3_uri_parameter(fname, "");
    (void)sqlite3_uri_parameter(fname, "missing");
    (void)sqlite3_uri_parameter(NULL, "x");
    (void)sqlite3_uri_parameter(fname, NULL);

    sqlite3_free_filename(fname);
    fname = NULL;
  }

cleanup:
  if(fname) sqlite3_free_filename(fname);
  for(i = 0; i < 16; i++) free(param_storage[i]);
  free(db);
  free(jr);
  free(wal);
  return 0;
}