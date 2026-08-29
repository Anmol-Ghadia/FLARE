// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static sqlite3_int64 read_i64(const uint8_t **p, size_t *n) {
  sqlite3_int64 v = 0;
  size_t take = *n < 8 ? *n : 8;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int pick_status_op(uint32_t x) {
  static const int ops[] = {
#ifdef SQLITE_STATUS_MEMORY_USED
    SQLITE_STATUS_MEMORY_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_USED
    SQLITE_STATUS_PAGECACHE_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_OVERFLOW
    SQLITE_STATUS_PAGECACHE_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_SCRATCH_USED
    SQLITE_STATUS_SCRATCH_USED,
#endif
#ifdef SQLITE_STATUS_SCRATCH_OVERFLOW
    SQLITE_STATUS_SCRATCH_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_MALLOC_SIZE
    SQLITE_STATUS_MALLOC_SIZE,
#endif
#ifdef SQLITE_STATUS_PARSER_STACK
    SQLITE_STATUS_PARSER_STACK,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_SIZE
    SQLITE_STATUS_PAGECACHE_SIZE,
#endif
#ifdef SQLITE_STATUS_SCRATCH_SIZE
    SQLITE_STATUS_SCRATCH_SIZE,
#endif
#ifdef SQLITE_STATUS_MALLOC_COUNT
    SQLITE_STATUS_MALLOC_COUNT,
#endif
  };
  if (sizeof(ops) / sizeof(ops[0]) == 0) {
    return (int)x;
  }
  return ops[x % (sizeof(ops) / sizeof(ops[0]))];
}

int LLVMFuzzerTestOneInput_77(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  (void)sqlite3_initialize();
  (void)sqlite3_initialize();

  for (int i = 0; i < 32 && n > 0; i++) {
    uint32_t action = read_u32(&p, &n) % 8;

    switch (action) {
      case 0: {
        int cur = 0, hi = 0;
        int op = pick_status_op(read_u32(&p, &n));
        int reset = (int)(read_u32(&p, &n) & 1U);
        (void)sqlite3_status(op, &cur, &hi, reset);
        break;
      }

      case 1: {
        sqlite3_int64 cur64 = 0, hi64 = 0;
        int op = pick_status_op(read_u32(&p, &n));
        int reset = (int)(read_u32(&p, &n) & 1U);
        (void)sqlite3_status64(op, &cur64, &hi64, reset);
        break;
      }

      case 2: {
        (void)sqlite3_memory_used();
        break;
      }

      case 3: {
        sqlite3_int64 val;
        if (n > 0) {
          uint32_t mode = read_u32(&p, &n) % 4;
          if (mode == 0) {
            val = -1;
          } else if (mode == 1) {
            val = 0;
          } else if (mode == 2) {
            val = (sqlite3_int64)(read_u32(&p, &n) & 0xFFFF);
          } else {
            val = read_i64(&p, &n);
            if (val < -1) val = -1;
          }
        } else {
          val = -1;
        }
        (void)sqlite3_soft_heap_limit64(val);
        break;
      }

      case 4: {
        int reset = (int)(read_u32(&p, &n) & 1U);
        (void)sqlite3_memory_highwater(reset);
        break;
      }

      case 5: {
        int cur = 0, hi = 0;
        int op = (int)read_u32(&p, &n);
        int reset = (int)(read_u32(&p, &n) & 1U);
        (void)sqlite3_status(op, &cur, &hi, reset);
        break;
      }

      case 6: {
        sqlite3_int64 cur64 = 0, hi64 = 0;
        int op = (int)read_u32(&p, &n);
        int reset = (int)(read_u32(&p, &n) & 1U);
        (void)sqlite3_status64(op, &cur64, &hi64, reset);
        break;
      }

      case 7: {
        (void)sqlite3_initialize();
        break;
      }
    }
  }

  {
    int cur = 0, hi = 0;
    sqlite3_int64 cur64 = 0, hi64 = 0;
    (void)sqlite3_status(pick_status_op(0), &cur, &hi, 0);
    (void)sqlite3_status64(pick_status_op(1), &cur64, &hi64, 1);
    (void)sqlite3_memory_used();
    (void)sqlite3_memory_highwater(0);
    (void)sqlite3_soft_heap_limit64(-1);
  }

  return 0;
}