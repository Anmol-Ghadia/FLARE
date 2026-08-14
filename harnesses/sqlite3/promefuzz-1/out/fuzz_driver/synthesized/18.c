// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sqlite3.h"

static sqlite3_int64 read_i64(const uint8_t *data, size_t size, size_t offset) {
  sqlite3_int64 v = 0;
  size_t i;
  for (i = 0; i < 8 && offset + i < size; ++i) {
    v |= ((sqlite3_int64)data[offset + i]) << (i * 8);
  }
  return v;
}

int LLVMFuzzerTestOneInput_18(const uint8_t *Data, size_t Size) {
  int reset1 = 0;
  int op = SQLITE_STATUS_MEMORY_USED;
  int current = 0;
  int highwater = 0;
  int reset2 = 0;
  sqlite3_int64 hard_n = -1;
  sqlite3_int64 soft_n = -1;
  sqlite3_int64 old_hard = -1;
  sqlite3_int64 old_soft = -1;
  sqlite3_int64 prev_hard;
  sqlite3_int64 prev_soft;
  sqlite3_int64 tmp64;

  if (Size > 0) reset1 = Data[0] & 1;
  if (Size > 1) {
    switch (Data[1] % 10) {
      case 0: op = SQLITE_STATUS_MEMORY_USED; break;
      case 1: op = SQLITE_STATUS_MALLOC_SIZE; break;
      case 2: op = SQLITE_STATUS_MALLOC_COUNT; break;
      case 3: op = SQLITE_STATUS_PAGECACHE_USED; break;
      case 4: op = SQLITE_STATUS_PAGECACHE_OVERFLOW; break;
      case 5: op = SQLITE_STATUS_PAGECACHE_SIZE; break;
      case 6: op = SQLITE_STATUS_SCRATCH_USED; break;
      case 7: op = SQLITE_STATUS_SCRATCH_OVERFLOW; break;
      case 8: op = SQLITE_STATUS_SCRATCH_SIZE; break;
      default: op = SQLITE_STATUS_PARSER_STACK; break;
    }
  }
  if (Size > 2) reset2 = Data[2] & 1;

  if (Size >= 11) {
    hard_n = read_i64(Data, Size, 3);
  } else if (Size > 3) {
    hard_n = (sqlite3_int64)((int8_t)Data[3]);
  }

  if (Size >= 19) {
    soft_n = read_i64(Data, Size, 11);
  } else if (Size > 11) {
    soft_n = (sqlite3_int64)((int8_t)Data[11]);
  }

  if (Size > 19) {
    switch (Data[19] % 4) {
      case 0: hard_n = -1; break;
      case 1: hard_n = 0; break;
      case 2: break;
      default: if (hard_n < 0) hard_n = -hard_n; break;
    }
  }
  if (Size > 20) {
    switch (Data[20] % 4) {
      case 0: soft_n = -1; break;
      case 1: soft_n = 0; break;
      case 2: break;
      default: if (soft_n < 0) soft_n = -soft_n; break;
    }
  }

  old_hard = sqlite3_hard_heap_limit64(-1);
  old_soft = sqlite3_soft_heap_limit64(-1);

  (void)sqlite3_memory_highwater(reset1);
  (void)sqlite3_memory_used();
  (void)sqlite3_status(op, &current, &highwater, reset2);
  (void)sqlite3_memory_used();
  prev_hard = sqlite3_hard_heap_limit64(hard_n);
  prev_soft = sqlite3_soft_heap_limit64(soft_n);

  if (Size > 21) {
    int op2 = SQLITE_STATUS_MEMORY_USED;
    int current2 = 0;
    int highwater2 = 0;
    int reset3 = Data[21] & 1;
    switch ((Size > 22 ? Data[22] : 0) % 6) {
      case 0: op2 = SQLITE_STATUS_MEMORY_USED; break;
      case 1: op2 = SQLITE_STATUS_MALLOC_COUNT; break;
      case 2: op2 = SQLITE_STATUS_PAGECACHE_USED; break;
      case 3: op2 = SQLITE_STATUS_PAGECACHE_OVERFLOW; break;
      case 4: op2 = SQLITE_STATUS_PAGECACHE_SIZE; break;
      default: op2 = SQLITE_STATUS_PARSER_STACK; break;
    }
    (void)sqlite3_memory_highwater(reset3);
    (void)sqlite3_memory_used();
    (void)sqlite3_status(op2, &current2, &highwater2, reset3 ^ 1);
    (void)sqlite3_memory_used();
    tmp64 = (Size > 30) ? read_i64(Data, Size, 23) : -1;
    if (Size > 31 && (Data[31] & 1) && tmp64 < 0) tmp64 = -tmp64;
    (void)sqlite3_hard_heap_limit64(tmp64);
    (void)sqlite3_soft_heap_limit64((Size > 39) ? read_i64(Data, Size, 32) : -1);
  }

  if (old_hard >= 0) {
    (void)sqlite3_hard_heap_limit64(old_hard);
  } else {
    (void)sqlite3_hard_heap_limit64(prev_hard);
  }

  if (old_soft >= 0) {
    (void)sqlite3_soft_heap_limit64(old_soft);
  } else {
    (void)sqlite3_soft_heap_limit64(prev_soft);
  }

  return 0;
}