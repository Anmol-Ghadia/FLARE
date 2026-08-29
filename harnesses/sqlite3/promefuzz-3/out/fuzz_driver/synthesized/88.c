// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_load_extension at sqlite3.c:114732:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_load_extension at sqlite3.c:114732:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t i, m = *n < 4 ? *n : 4;
  for (i = 0; i < m; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += m;
  *n -= m;
  return v;
}

static int read_int(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static size_t take_bytes(const uint8_t **p, size_t *n, char *out, size_t max_out) {
  size_t want;
  if (*n == 0 || max_out == 0) {
    if (max_out) out[0] = '\0';
    return 0;
  }
  want = read_u32(p, n);
  want %= max_out;
  if (want > *n) want = *n;
  memcpy(out, *p, want);
  out[want] = '\0';
  *p += want;
  *n -= want;
  return want;
}

int LLVMFuzzerTestOneInput_88(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  char dbname[256];
  char extfile[256];
  char procname[256];
  char *errMsg = NULL;
  void *mem1 = NULL;
  void *mem2 = NULL;
  unsigned char *deser = NULL;
  sqlite3_int64 szDb = 0;
  sqlite3_int64 szBuf = 0;
  unsigned deserFlags = 0;
  int rc;
  int oldLimit;
  int limitId;
  int newVal;
  int enable = 0;
  int use_null_proc = 0;
  int use_memory_db = 0;
  int use_dummy_db = 0;
  FILE *fp;

  if (n == 0) {
    sqlite3_free_table(NULL);
    return 0;
  }

  enable = read_int(&p, &n) & 1;
  use_null_proc = read_int(&p, &n) & 1;
  use_memory_db = read_int(&p, &n) & 1;
  use_dummy_db = read_int(&p, &n) & 1;

  if (use_memory_db) {
    strcpy(dbname, ":memory:");
  } else if (use_dummy_db) {
    strcpy(dbname, "./dummy_file");
  } else {
    if (take_bytes(&p, &n, dbname, sizeof(dbname) - 1) == 0) {
      strcpy(dbname, "");
    }
  }

  rc = sqlite3_open(dbname, &db);
  if (db == NULL) {
    sqlite3_free_table(NULL);
    return 0;
  }

  limitId = read_int(&p, &n) % SQLITE_LIMIT_WORKER_THREADS;
  if (limitId < 0) limitId = -limitId;
  newVal = read_int(&p, &n);
  oldLimit = sqlite3_limit(db, limitId, newVal);
  (void)oldLimit;
  sqlite3_limit(db, limitId, -1);

  {
    int cfgOut = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, enable, &cfgOut);
  }

  {
    int alloc1 = read_int(&p, &n);
    int alloc2 = read_int(&p, &n);
    if (alloc1 < 0) alloc1 = -(alloc1 + 1);
    if (alloc2 < 0) alloc2 = -(alloc2 + 1);
    alloc1 %= 4096;
    alloc2 %= 4096;
    if (alloc1 > 0) mem1 = sqlite3_malloc(alloc1);
    if (alloc2 > 0) mem2 = sqlite3_malloc(alloc2);
    if (mem1 && n) memset(mem1, p[0], alloc1);
    if (mem2 && n > 1) memset(mem2, p[1], alloc2);
  }

  if (take_bytes(&p, &n, extfile, sizeof(extfile) - 1) == 0) {
    strcpy(extfile, "./dummy_file");
  }
  if (take_bytes(&p, &n, procname, sizeof(procname) - 1) == 0) {
    strcpy(procname, "sqlite3_extension_init");
  }

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (n > 0) fwrite(p, 1, n, fp);
    fclose(fp);
  }

  rc = sqlite3_load_extension(db,
                              (extfile[0] ? extfile : "./dummy_file"),
                              use_null_proc ? NULL : procname,
                              &errMsg);
  if (errMsg) {
    sqlite3_free(errMsg);
    errMsg = NULL;
  }

  rc = sqlite3_load_extension(db, "./dummy_file", NULL, &errMsg);
  if (errMsg) {
    sqlite3_free(errMsg);
    errMsg = NULL;
  }

#ifndef SQLITE_OMIT_DESERIALIZE
  {
    int alloc_sz = read_int(&p, &n);
    if (alloc_sz < 0) alloc_sz = -(alloc_sz + 1);
    alloc_sz %= 8192;

    if (alloc_sz > 0) {
      deser = (unsigned char *)sqlite3_malloc(alloc_sz);
      if (deser) {
        size_t copy_sz = n < (size_t)alloc_sz ? n : (size_t)alloc_sz;
        if (copy_sz > 0) memcpy(deser, p, copy_sz);
        if ((size_t)alloc_sz > copy_sz) memset(deser + copy_sz, 0, (size_t)alloc_sz - copy_sz);

        szBuf = alloc_sz;
        if (szBuf > 0) {
          szDb = (sqlite3_int64)(read_u32(&p, &n) % ((uint32_t)szBuf + 1));
        } else {
          szDb = 0;
        }

        deserFlags = 0;
#ifdef SQLITE_DESERIALIZE_FREEONCLOSE
        if (read_int(&p, &n) & 1) deserFlags |= SQLITE_DESERIALIZE_FREEONCLOSE;
#endif
#ifdef SQLITE_DESERIALIZE_RESIZEABLE
        if (read_int(&p, &n) & 1) deserFlags |= SQLITE_DESERIALIZE_RESIZEABLE;
#endif
#ifdef SQLITE_DESERIALIZE_READONLY
        if (read_int(&p, &n) & 1) deserFlags |= SQLITE_DESERIALIZE_READONLY;
#endif

        rc = sqlite3_deserialize(db, "main", deser, szDb, szBuf, deserFlags);
        if (rc != SQLITE_OK) {
#ifdef SQLITE_DESERIALIZE_FREEONCLOSE
          if ((deserFlags & SQLITE_DESERIALIZE_FREEONCLOSE) == 0) {
            sqlite3_free(deser);
          }
#else
          sqlite3_free(deser);
#endif
          deser = NULL;
        } else {
#ifdef SQLITE_DESERIALIZE_FREEONCLOSE
          if ((deserFlags & SQLITE_DESERIALIZE_FREEONCLOSE) == 0) {
            sqlite3_free(deser);
            deser = NULL;
          } else {
            deser = NULL;
          }
#else
          sqlite3_free(deser);
          deser = NULL;
#endif
        }
      }
    }
  }
#endif

  sqlite3_free_table(NULL);

  sqlite3_free(mem1);
  sqlite3_free(mem2);

  if (errMsg) sqlite3_free(errMsg);
  sqlite3_close(db);
  return 0;
}