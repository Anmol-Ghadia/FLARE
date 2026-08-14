// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static size_t take_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  size_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static char *make_cstring_from_bytes(const uint8_t *src, size_t len) {
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, src, len);
  s[len] = '\0';
  return s;
}

static char *take_string_chunk(const uint8_t *Data, size_t Size, size_t *Off, size_t max_len) {
  size_t remain = (*Off < Size) ? (Size - *Off) : 0;
  size_t want = take_u32(Data, Size, Off);
  size_t len = remain ? (want % (remain + 1)) : 0;
  if (len > max_len) len = max_len;
  if (*Off + len > Size) len = Size - *Off;
  {
    char *s = make_cstring_from_bytes(Data + *Off, len);
    *Off += len;
    return s;
  }
}

static void touch_string(const char *s) {
  volatile unsigned char x = 0;
  if (s != NULL) x ^= (unsigned char)s[0];
  (void)x;
}

int LLVMFuzzerTestOneInput_89(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  char *db = take_string_chunk(Data, Size, &off, 256);
  char *jrnl = take_string_chunk(Data, Size, &off, 256);
  char *wal = take_string_chunk(Data, Size, &off, 256);

  if (!db || !jrnl || !wal) {
    free(db);
    free(jrnl);
    free(wal);
    sqlite3_free_filename(NULL);
    return 0;
  }

  {
    int nParam = (int)(take_u32(Data, Size, &off) % 8);
    const char **azParam = NULL;
    char **owned = NULL;
    char *fname = NULL;

    if (nParam > 0) {
      azParam = (const char **)calloc((size_t)nParam * 2, sizeof(const char *));
      owned = (char **)calloc((size_t)nParam * 2, sizeof(char *));
      if (!azParam || !owned) {
        free((void *)azParam);
        free(owned);
        free(db);
        free(jrnl);
        free(wal);
        sqlite3_free_filename(NULL);
        return 0;
      }

      for (i = 0; i < nParam * 2; ++i) {
        owned[i] = take_string_chunk(Data, Size, &off, 128);
        if (!owned[i]) {
          int j;
          for (j = 0; j < i; ++j) free(owned[j]);
          free(owned);
          free((void *)azParam);
          free(db);
          free(jrnl);
          free(wal);
          sqlite3_free_filename(NULL);
          return 0;
        }
        if ((i % 2) == 0 && owned[i][0] == '\0') {
          free(owned[i]);
          owned[i] = make_cstring_from_bytes((const uint8_t *)"k", 1);
          if (!owned[i]) {
            int j;
            for (j = 0; j < i; ++j) free(owned[j]);
            free(owned);
            free((void *)azParam);
            free(db);
            free(jrnl);
            free(wal);
            sqlite3_free_filename(NULL);
            return 0;
          }
        }
        azParam[i] = owned[i];
      }
    }

    fname = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);

    if (fname) {
      const char *db_name = sqlite3_filename_database(fname);
      const char *jrnl_name = sqlite3_filename_journal(fname);
      const char *wal_name = sqlite3_filename_wal(fname);

      touch_string(db_name);
      touch_string(jrnl_name);
      touch_string(wal_name);

      if (nParam > 0) {
        for (i = 0; i < nParam * 2; i += 2) {
          const char *val = sqlite3_uri_parameter(fname, azParam[i]);
          touch_string(val);
        }
      }

      {
        char *query_name = take_string_chunk(Data, Size, &off, 128);
        if (query_name) {
          const char *val = sqlite3_uri_parameter(fname, query_name);
          touch_string(val);
          free(query_name);
        }
      }

      /* Only call sqlite3_uri_parameter() on the original encoded filename.
         Do not pass db/journal/wal component strings to sqlite3_filename_*(),
         as those helpers expect the original SQLite filename blob pointer. */
      sqlite3_free_filename(fname);
    } else {
      sqlite3_free_filename(NULL);
    }

    if (owned) {
      for (i = 0; i < nParam * 2; ++i) free(owned[i]);
    }
    free(owned);
    free((void *)azParam);
  }

  free(db);
  free(jrnl);
  free(wal);

  return 0;
}