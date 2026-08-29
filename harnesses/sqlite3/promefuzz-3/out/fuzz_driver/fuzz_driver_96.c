// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static sqlite3_uint64 read_u64(const uint8_t **p, size_t *n) {
  sqlite3_uint64 v = 0;
  size_t take = *n < 8 ? *n : 8;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static size_t bounded_copy_len(sqlite3_uint64 alloc_sz, size_t available) {
  size_t cap = 1024;
  size_t n = (size_t)(alloc_sz < (sqlite3_uint64)cap ? alloc_sz : (sqlite3_uint64)cap);
  return n < available ? n : available;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  (void)sqlite3_initialize();

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size) {
        (void)fwrite(Data, 1, Size, fp);
      }
      fclose(fp);
    }
  }

  int n1 = (int)(read_u32(&p, &n) % 4096U);
  int n2 = (int)(read_u32(&p, &n) % 4096U);
  sqlite3_uint64 n3 = (sqlite3_uint64)(read_u64(&p, &n) % 65536ULL);
  sqlite3_uint64 n4 = (sqlite3_uint64)(read_u64(&p, &n) % 65536ULL);
  unsigned deserFlags = read_u32(&p, &n) & (
#ifdef SQLITE_DESERIALIZE_FREEONCLOSE
      SQLITE_DESERIALIZE_FREEONCLOSE |
#endif
#ifdef SQLITE_DESERIALIZE_RESIZEABLE
      SQLITE_DESERIALIZE_RESIZEABLE |
#endif
#ifdef SQLITE_DESERIALIZE_READONLY
      SQLITE_DESERIALIZE_READONLY |
#endif
      0U);

  void *a = sqlite3_malloc(n1);
  if (a) {
    sqlite3_uint64 sz = sqlite3_msize(a);
    size_t copy = bounded_copy_len(sz, n);
    if (copy) {
      memcpy(a, p, copy);
    }
  }

  void *b = sqlite3_malloc64(n3);
  if (b) {
    sqlite3_uint64 sz = sqlite3_msize(b);
    size_t copy = bounded_copy_len(sz, n);
    if (copy) {
      memcpy(b, p, copy);
    }
  }

  if (a) {
    void *old_a = a;
    void *a2 = sqlite3_realloc64(a, n4);
    if (a2) {
      a = a2;
      sqlite3_uint64 sz = sqlite3_msize(a);
      size_t copy = bounded_copy_len(sz, n);
      if (copy) {
        memcpy(a, p, copy);
      }
    } else if (n4 == 0) {
      a = NULL;
    } else {
      a = old_a;
    }
  } else {
    a = sqlite3_realloc64(NULL, n4);
    if (a) {
      sqlite3_uint64 sz = sqlite3_msize(a);
      size_t copy = bounded_copy_len(sz, n);
      if (copy) {
        memcpy(a, p, copy);
      }
    }
  }

  if (b) {
    void *old_b = b;
    void *b2 = sqlite3_realloc64(b, (sqlite3_uint64)n2);
    if (b2) {
      b = b2;
      sqlite3_uint64 sz = sqlite3_msize(b);
      size_t copy = bounded_copy_len(sz, n);
      if (copy) {
        memcpy(b, p, copy);
      }
    } else if (n2 == 0) {
      b = NULL;
    } else {
      b = old_b;
    }
  } else {
    b = sqlite3_realloc64(NULL, (sqlite3_uint64)n2);
    if (b) {
      sqlite3_uint64 sz = sqlite3_msize(b);
      size_t copy = bounded_copy_len(sz, n);
      if (copy) {
        memcpy(b, p, copy);
      }
    }
  }

  (void)sqlite3_msize(NULL);
  if (a) {
    (void)sqlite3_msize(a);
  }
  if (b) {
    (void)sqlite3_msize(b);
  }

#ifndef SQLITE_OMIT_DESERIALIZE
  {
    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) == SQLITE_OK && db) {
      size_t payload = n;
      size_t extra = payload ? (size_t)(p[0] % 16U) : 0U;
      size_t allocsz = payload + extra;
      unsigned char *buf = NULL;
      int free_by_sqlite = 0;

#ifdef SQLITE_DESERIALIZE_FREEONCLOSE
      free_by_sqlite = ((deserFlags & SQLITE_DESERIALIZE_FREEONCLOSE) != 0);
#endif

      if (allocsz > 0) {
        buf = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)allocsz);
      } else if (free_by_sqlite) {
        buf = (unsigned char *)sqlite3_malloc64(1);
        if (buf) {
          allocsz = 1;
        }
      }

      if (buf) {
        if (payload) {
          memcpy(buf, p, payload);
        }

        sqlite3_int64 szDb = (sqlite3_int64)(payload ? (payload / 2 + (payload % 2)) : 0);
        sqlite3_int64 szBuf = (sqlite3_int64)allocsz;
        if (szDb > szBuf) {
          szDb = szBuf;
        }

        if (sqlite3_deserialize(db, "main", buf, szDb, szBuf, deserFlags) != SQLITE_OK) {
          if (!free_by_sqlite) {
            sqlite3_free(buf);
          }
        } else {
          if (!free_by_sqlite) {
            sqlite3_free(buf);
          }
        }
      }

      sqlite3_close(db);
    } else if (db) {
      sqlite3_close(db);
    }
  }
#endif

  sqlite3_free(a);
  sqlite3_free(b);
  return 0;
}