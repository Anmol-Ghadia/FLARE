// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_appendchar at sqlite3.c:17235:17 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
// sqlite3_str_reset at sqlite3.c:17356:17 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_str_length at sqlite3.c:17342:16 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_append at sqlite3.c:17263:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for(size_t i = 0; i < take; i++){
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int_bounded(const uint8_t **p, size_t *n, int limit){
  uint32_t v = read_u32(p, n);
  if(limit <= 0) return 0;
  return (int)(v % (uint32_t)limit);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_str *s = NULL;
  const uint8_t *p = Data;
  size_t n = Size;

  (void)sqlite3_open(":memory:", &db);

  if(n > 0 && (p[0] & 1)){
    s = sqlite3_str_new(db);
  }else{
    s = sqlite3_str_new(NULL);
  }

  if(!s){
    if(db) sqlite3_close(db);
    (void)sqlite3_str_errcode(NULL);
    (void)sqlite3_str_length(NULL);
    return 0;
  }

  if(n > 0){
    int ops = 1 + (int)(p[0] % 32);
    p++;
    n--;

    for(int i = 0; i < ops; i++){
      int op = read_int_bounded(&p, &n, 8);
      switch(op){
        case 0: {
          int count = read_int_bounded(&p, &n, 1024);
          char c = n ? (char)(*p++) : '\0';
          if(n) n--;
          sqlite3_str_appendchar(s, count, c);
          break;
        }
        case 1: {
          int len = read_int_bounded(&p, &n, 1024);
          if((size_t)len > n) len = (int)n;
          sqlite3_str_append(s, (const char *)p, len);
          p += len;
          n -= (size_t)len;
          break;
        }
        case 2: {
          sqlite3_str_reset(s);
          break;
        }
        case 3: {
          (void)sqlite3_str_errcode(s);
          break;
        }
        case 4: {
          (void)sqlite3_str_length(s);
          break;
        }
        case 5: {
          int len = read_int_bounded(&p, &n, 1024);
          if((size_t)len > n) len = (int)n;
          char buf[1025];
          if(len > 0){
            memcpy(buf, p, (size_t)len);
          }
          buf[len] = '\0';
          sqlite3_str_appendall(s, buf);
          p += len;
          n -= (size_t)len;
          break;
        }
        case 6: {
          sqlite3_str_append(s, NULL, 0);
          break;
        }
        case 7: {
          (void)sqlite3_str_errcode(NULL);
          (void)sqlite3_str_length(NULL);
          break;
        }
      }
    }
  }

  (void)sqlite3_str_errcode(s);
  (void)sqlite3_str_length(s);

  sqlite3_free(sqlite3_str_finish(s));
  if(db) sqlite3_close(db);
  return 0;
}