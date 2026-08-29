// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_realloc at sqlite3.c:15947:18 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_msize at sqlite3.c:15806:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4; i++) {
    v <<= 8;
    if (*Off < Size) {
      v |= Data[*Off];
      (*Off)++;
    }
  }
  return v;
}

static sqlite3_uint64 read_u64(const uint8_t *Data, size_t Size, size_t *Off) {
  sqlite3_uint64 v = 0;
  size_t i;
  for (i = 0; i < 8; i++) {
    v <<= 8;
    if (*Off < Size) {
      v |= (sqlite3_uint64)Data[*Off];
      (*Off)++;
    }
  }
  return v;
}

static int clamp_int_size(uint32_t v) {
  return (int)(v % 4097U);
}

static sqlite3_uint64 clamp_u64_size(sqlite3_uint64 v) {
  return (sqlite3_uint64)(v % 65537ULL);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  void *ptrs[8] = {0};
  int i;

  (void)sqlite3_initialize();

  for (i = 0; i < 32; i++) {
    uint8_t op;
    uint8_t idx;
    uint32_t n32_raw;
    sqlite3_uint64 n64_raw;
    int n32;
    sqlite3_uint64 n64;
    void *p;
    sqlite3_uint64 msz;

    if (off >= Size) break;

    op = Data[off++];
    idx = (uint8_t)((off < Size ? Data[off++] : 0) % 8);

    switch (op % 8) {
      case 0:
        n32_raw = read_u32(Data, Size, &off);
        n32 = clamp_int_size(n32_raw);
        p = sqlite3_malloc(n32);
        if (p) {
          sqlite3_uint64 sz = sqlite3_msize(p);
          if (sz > 0 && off < Size) {
            size_t to_copy = (size_t)((Size - off) < sz ? (Size - off) : sz);
            if (to_copy > 0) memcpy(p, Data + off, to_copy);
            off += to_copy;
          }
          if (ptrs[idx]) sqlite3_free(ptrs[idx]);
          ptrs[idx] = p;
        }
        break;

      case 1:
        n64_raw = read_u64(Data, Size, &off);
        n64 = clamp_u64_size(n64_raw);
        p = sqlite3_malloc64(n64);
        if (p) {
          sqlite3_uint64 sz = sqlite3_msize(p);
          if (sz > 0 && off < Size) {
            size_t to_copy = (size_t)((Size - off) < sz ? (Size - off) : sz);
            if (to_copy > 0) memcpy(p, Data + off, to_copy);
            off += to_copy;
          }
          if (ptrs[idx]) sqlite3_free(ptrs[idx]);
          ptrs[idx] = p;
        }
        break;

      case 2:
        n32_raw = read_u32(Data, Size, &off);
        n32 = clamp_int_size(n32_raw);
        p = sqlite3_realloc(ptrs[idx], n32);
        if (p || n32 == 0) {
          ptrs[idx] = p;
        }
        break;

      case 3:
        n64_raw = read_u64(Data, Size, &off);
        n64 = clamp_u64_size(n64_raw);
        p = sqlite3_realloc64(ptrs[idx], n64);
        if (p || n64 == 0) {
          ptrs[idx] = p;
        }
        break;

      case 4:
        msz = sqlite3_msize(ptrs[idx]);
        if (ptrs[idx] && msz > 0 && off < Size) {
          size_t to_copy = (size_t)((Size - off) < msz ? (Size - off) : msz);
          if (to_copy > 0) memcpy(ptrs[idx], Data + off, to_copy);
          off += to_copy;
        }
        break;

      case 5:
        sqlite3_free(ptrs[idx]);
        ptrs[idx] = NULL;
        break;

      case 6:
        (void)sqlite3_msize(ptrs[idx]);
        break;

      case 7:
        sqlite3_free(NULL);
        break;
    }
  }

  for (i = 0; i < 8; i++) {
    sqlite3_free(ptrs[i]);
    ptrs[i] = NULL;
  }

  return 0;
}