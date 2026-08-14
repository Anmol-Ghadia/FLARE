#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
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

static int read_int(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static void append_bytes(sqlite3_str *s, const uint8_t *buf, int len){
  if(!s) return;
  if(len <= 0){
    sqlite3_str_append(s, "", 0);
    return;
  }
  if(!buf) return;
  sqlite3_str_append(s, (const char *)buf, len);
}

static void safe_appendf_int(sqlite3_str *s, const char *fmt, int v){
  if(!s || !fmt) return;
  sqlite3_str_append(s, "x", 1);
  sqlite3_str_appendf(s, fmt, v);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_str *s = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int use_db = 0;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if(n > 0){
    use_db = (*p++) & 1;
    n--;
  }

  if(use_db){
    rc = sqlite3_open("./dummy_file", &db);
    if(rc != SQLITE_OK){
      if(db) sqlite3_close(db);
      db = NULL;
    }
  }

  if(n > 0 && db){
    int lim = read_int(&p, &n);
    if(lim < 0) lim = -lim;
    sqlite3_limit(db, SQLITE_LIMIT_LENGTH, lim % 65536);
  }

  (void)sqlite3_str_errcode(NULL);

  s = sqlite3_str_new(db);
  if(s){
    int rounds = (n > 0) ? ((int)(*p++) % 16) : 0;
    if(n > 0) n--;

    for(int i = 0; i < rounds; i++){
      int op = (n > 0) ? ((int)(*p++) % 6) : 0;
      if(n > 0) n--;

      switch(op){
        case 0: {
          int len = read_int(&p, &n);
          if(len < 0) len = -len;
          if((size_t)len > n) len = (int)n;
          append_bytes(s, p, len);
          p += len;
          n -= (size_t)len;
          break;
        }
        case 1: {
          int len = read_int(&p, &n);
          if(len < 0) len = -len;
          if((size_t)len > n) len = (int)n;
          append_bytes(s, p, len);
          p += len;
          n -= (size_t)len;
          break;
        }
        case 2: {
          int a = read_int(&p, &n);
          int b = read_int(&p, &n);
          char numbuf[64];
          int written = snprintf(numbuf, sizeof(numbuf), "%d:%x", a, b);
          if(written > 0){
            if(written > (int)sizeof(numbuf)) written = (int)sizeof(numbuf);
            sqlite3_str_append(s, numbuf, written);
          }
          if(n > 0){
            int len = (int)(n > 32 ? 32 : n);
            append_bytes(s, p, len);
            p += (size_t)len;
            n -= (size_t)len;
          }
          break;
        }
        case 3: {
          (void)sqlite3_str_errcode(s);
          break;
        }
        case 4: {
          sqlite3_str_reset(s);
          (void)sqlite3_str_errcode(s);
          break;
        }
        case 5: {
          safe_appendf_int(s, "%d", read_int(&p, &n));
          break;
        }
      }
    }

    (void)sqlite3_str_errcode(s);
    {
      char *out = sqlite3_str_finish(s);
      if(out){
        sqlite3_free(out);
      }
    }
  }

  s = sqlite3_str_new(db);
  if(s){
    sqlite3_str_reset(s);
    if(n > 0){
      int len = (int)(n > 128 ? 128 : n);
      append_bytes(s, p, len);
      safe_appendf_int(s, ":%d", len);
    }else{
      sqlite3_str_append(s, "x", 1);
      safe_appendf_int(s, ":%d", 0);
    }
    (void)sqlite3_str_errcode(s);
    {
      char *out2 = sqlite3_str_finish(s);
      if(out2){
        sqlite3_free(out2);
      }
    }
  }

  if(db){
    sqlite3_close(db);
  }
  return 0;
}