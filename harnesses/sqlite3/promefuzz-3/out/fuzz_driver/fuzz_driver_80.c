// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_soft_heap_limit at sqlite3.c:15556:17 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_memory_alarm at sqlite3.c:15508:16 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_soft_heap_limit at sqlite3.c:15556:17 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for(size_t i = 0; i < take; i++){
    v |= ((uint32_t)(*p)[i]) << (8 * i);
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_i32(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static sqlite3_int64 read_i64(const uint8_t **p, size_t *n){
  uint64_t v = 0;
  size_t take = *n < 8 ? *n : 8;
  for(size_t i = 0; i < take; i++){
    v |= ((uint64_t)(*p)[i]) << (8 * i);
  }
  *p += take;
  *n -= take;
  return (sqlite3_int64)v;
}

static int pick_status_op(uint32_t x){
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
  if(sizeof(ops)/sizeof(ops[0]) == 0){
    return (int)x;
  }
  if((x & 1U) == 0){
    return ops[x % (sizeof(ops)/sizeof(ops[0]))];
  }
  return (int)x;
}

static void dummy_alarm(void *pArg, sqlite3_int64 used, int n){
  (void)pArg;
  (void)used;
  (void)n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp != NULL){
    if(Size > 0){
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlite3_initialize();

  for(int i = 0; i < 32 && n > 0; i++){
    uint32_t action = read_u32(&p, &n) % 8U;

    switch(action){
      case 0: {
        int limit = read_i32(&p, &n);
        sqlite3_soft_heap_limit(limit);
        break;
      }
      case 1: {
        sqlite3_int64 limit64 = read_i64(&p, &n);
        sqlite3_soft_heap_limit64(limit64);
        break;
      }
      case 2: {
        int op = pick_status_op(read_u32(&p, &n));
        int cur = 0, hi = 0;
        int resetFlag = read_i32(&p, &n) & 1;
        int mode = read_i32(&p, &n) % 4;
        int rc;
        if(mode == 0){
          rc = sqlite3_status(op, &cur, &hi, resetFlag);
        }else if(mode == 1){
          rc = sqlite3_status(op, NULL, &hi, resetFlag);
        }else if(mode == 2){
          rc = sqlite3_status(op, &cur, NULL, resetFlag);
        }else{
          rc = sqlite3_status(op, NULL, NULL, resetFlag);
        }
        (void)rc;
        break;
      }
      case 3: {
        int op = pick_status_op(read_u32(&p, &n));
        sqlite3_int64 cur64 = 0, hi64 = 0;
        int resetFlag = read_i32(&p, &n) & 1;
        int mode = read_i32(&p, &n) % 4;
        int rc;
        if(mode == 0){
          rc = sqlite3_status64(op, &cur64, &hi64, resetFlag);
        }else if(mode == 1){
          rc = sqlite3_status64(op, NULL, &hi64, resetFlag);
        }else if(mode == 2){
          rc = sqlite3_status64(op, &cur64, NULL, resetFlag);
        }else{
          rc = sqlite3_status64(op, NULL, NULL, resetFlag);
        }
        (void)rc;
        break;
      }
      case 4: {
        sqlite3_int64 threshold = read_i64(&p, &n);
        void *arg = (void *)(uintptr_t)read_u32(&p, &n);
        int use_cb = read_i32(&p, &n) & 1;
        int rc = sqlite3_memory_alarm(use_cb ? dummy_alarm : NULL, arg, threshold);
        (void)rc;
        break;
      }
      case 5: {
        int amt = read_i32(&p, &n);
        int freed = sqlite3_release_memory(amt);
        (void)freed;
        break;
      }
      case 6: {
        sqlite3_soft_heap_limit(-1);
        sqlite3_soft_heap_limit64(-1);
        break;
      }
      case 7: {
        int op = pick_status_op(read_u32(&p, &n));
        int cur = 0, hi = 0;
        sqlite3_int64 cur64 = 0, hi64 = 0;
        int rc1 = sqlite3_status(op, &cur, &hi, 1);
        int rc2 = sqlite3_status64(op, &cur64, &hi64, 1);
        (void)rc1;
        (void)rc2;
        break;
      }
    }
  }

  sqlite3_shutdown();
  return 0;
}