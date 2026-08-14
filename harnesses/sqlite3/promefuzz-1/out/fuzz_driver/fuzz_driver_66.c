// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

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

static uint64_t read_u64(const uint8_t **p, size_t *n) {
  uint64_t v = 0;
  size_t take = *n < 8 ? *n : 8;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static unsigned pick_flags(uint8_t f) {
  unsigned flags = 0;
  if (f & 1) flags |= SQLITE_DESERIALIZE_FREEONCLOSE;
  if (f & 2) flags |= SQLITE_DESERIALIZE_RESIZEABLE;
  if (f & 4) flags |= SQLITE_DESERIALIZE_READONLY;
  return flags;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  void *mem1 = NULL;
  void *mem2 = NULL;
  unsigned char *buf1 = NULL;
  unsigned char *buf2 = NULL;

  (void)sqlite3_initialize();

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  mem1 = sqlite3_malloc((int)(n > 1024 ? 1024 : n));
  if (mem1) {
    sqlite3_uint64 msz = sqlite3_msize(mem1);
    if (msz && n) {
      size_t copy_sz = (size_t)(msz < n ? msz : n);
      memcpy(mem1, p, copy_sz);
    }
  }

  mem2 = sqlite3_malloc64((sqlite3_uint64)(n > 2048 ? 2048 : n));
  if (mem2) {
    sqlite3_uint64 msz = sqlite3_msize(mem2);
    if (msz && n) {
      size_t copy_sz = (size_t)(msz < n ? msz : n);
      memcpy(mem2, p, copy_sz);
    }
    {
      sqlite3_uint64 new_sz = (sqlite3_uint64)(read_u64(&p, &n) % 4096);
      void *tmp = sqlite3_realloc64(mem2, new_sz);
      if (tmp || new_sz == 0) {
        mem2 = tmp;
      }
      (void)sqlite3_msize(mem2);
    }
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    sqlite3_free(mem1);
    sqlite3_free(mem2);
    return 0;
  }

  {
    sqlite3_uint64 alloc_sz = (sqlite3_uint64)(read_u32(&p, &n) % 8192);
    sqlite3_int64 szBuf = (sqlite3_int64)alloc_sz;
    sqlite3_int64 szDb = (sqlite3_int64)(n < alloc_sz ? n : alloc_sz);
    unsigned flags = n ? pick_flags(*p++) : 0;
    if (n) n--;

    if (alloc_sz > 0) {
      buf1 = (unsigned char *)sqlite3_malloc64(alloc_sz);
      if (buf1) {
        memset(buf1, 0, (size_t)alloc_sz);
        if (szDb > 0 && n > 0) {
          size_t copy_sz = (size_t)(szDb < (sqlite3_int64)n ? szDb : (sqlite3_int64)n);
          memcpy(buf1, p, copy_sz);
        }

        if (flags & SQLITE_DESERIALIZE_FREEONCLOSE) {
          (void)sqlite3_deserialize(db, "main", buf1, szDb, szBuf, flags);
          buf1 = NULL;
        } else {
          (void)sqlite3_deserialize(db, "main", buf1, szDb, szBuf, flags);
          sqlite3_free(buf1);
          buf1 = NULL;
        }
      }
    }
  }

  if (db) {
    sqlite3_close(db);
    db = NULL;
  }

  if (sqlite3_open(":memory:", &db) == SQLITE_OK && db != NULL) {
    sqlite3_uint64 alloc_sz = (sqlite3_uint64)(read_u32(&p, &n) % 8192);
    sqlite3_int64 szBuf = (sqlite3_int64)alloc_sz;
    sqlite3_int64 szDb = (sqlite3_int64)(n < alloc_sz ? n : alloc_sz);
    unsigned flags = n ? pick_flags(*p++) : 0;
    if (n) n--;

    buf2 = (unsigned char *)sqlite3_malloc64(alloc_sz);
    if (buf2) {
      sqlite3_uint64 actual = sqlite3_msize(buf2);
      if (actual > 0) {
        memset(buf2, 0xA5, (size_t)actual);
        if (n > 0) {
          size_t copy_sz = (size_t)(actual < n ? actual : n);
          memcpy(buf2, p, copy_sz);
        }
      }

      if ((flags & SQLITE_DESERIALIZE_RESIZEABLE) &&
          !(flags & SQLITE_DESERIALIZE_FREEONCLOSE)) {
        void *tmp = sqlite3_realloc64(buf2, alloc_sz + (read_u32(&p, &n) % 4096));
        if (tmp) {
          buf2 = (unsigned char *)tmp;
          szBuf = (sqlite3_int64)sqlite3_msize(buf2);
          if (szDb > szBuf) szDb = szBuf;
        }
      }

      if (flags & SQLITE_DESERIALIZE_FREEONCLOSE) {
        (void)sqlite3_deserialize(db, "main", buf2, szDb, szBuf, flags);
        buf2 = NULL;
      } else {
        (void)sqlite3_deserialize(db, "main", buf2, szDb, szBuf, flags);
        sqlite3_free(buf2);
        buf2 = NULL;
      }
    }

    sqlite3_close(db);
    db = NULL;
  }

  sqlite3_free(mem1);
  sqlite3_free(mem2);
  return 0;
}