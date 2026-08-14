#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t i, m = *n < 4 ? *n : 4;
  for(i = 0; i < m; i++){
    v = (v << 8) | (*p)[i];
  }
  *p += m;
  *n -= m;
  return v;
}

static int read_int(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static char *dup_cstring_from_bytes(const uint8_t *src, size_t len){
  char *s = (char *)sqlite3_malloc64(len + 1);
  if(!s) return NULL;
  if(len) memcpy(s, src, len);
  s[len] = '\0';
  return s;
}

static char *take_string(const uint8_t **p, size_t *n){
  if(*n == 0){
    return dup_cstring_from_bytes((const uint8_t *)"", 0);
  }
  size_t len = read_u32(p, n);
  if(*n > 0) len %= (*n + 1);
  else len = 0;
  {
    char *s = dup_cstring_from_bytes(*p, len);
    *p += len;
    *n -= len;
    return s;
  }
}

static char *wrap_vmprintf(const char *fmt, ...){
  char *out;
  va_list ap;
  va_start(ap, fmt);
  out = sqlite3_vmprintf(fmt, ap);
  va_end(ap);
  return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_str *acc1 = NULL;
  sqlite3_str *acc2 = NULL;
  char *s1 = NULL, *s2 = NULL, *s3 = NULL, *s4 = NULL, *s5 = NULL;
  char *fin1 = NULL, *fin2 = NULL;
  const char *val1 = NULL, *val2 = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;
  int i, loops;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      if(Size) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK){
    if(db) sqlite3_close(db);
    db = NULL;
  }

  acc1 = sqlite3_str_new(db);
  acc2 = sqlite3_str_new(NULL);

  val1 = sqlite3_str_value(NULL);

  s1 = take_string(&p, &n);
  s2 = take_string(&p, &n);
  s3 = take_string(&p, &n);

  if(acc1){
    sqlite3_str_appendall(acc1, s1);
    val1 = sqlite3_str_value(acc1);
    sqlite3_str_appendf(acc1, "%s", s2 ? s2 : "");
    val1 = sqlite3_str_value(acc1);
    sqlite3_str_appendf(acc1, "%d:%u:%s", read_int(&p, &n), read_u32(&p, &n), s3 ? s3 : "");
    val1 = sqlite3_str_value(acc1);
  }

  if(acc2){
    sqlite3_str_appendall(acc2, s2);
    val2 = sqlite3_str_value(acc2);
    sqlite3_str_appendf(acc2, "%q", s1 ? s1 : "");
    val2 = sqlite3_str_value(acc2);
    sqlite3_str_appendall(acc2, s3);
    val2 = sqlite3_str_value(acc2);
  }

  s4 = sqlite3_mprintf("%s|%d|%u|%.*s",
                       s1 ? s1 : "",
                       read_int(&p, &n),
                       read_u32(&p, &n),
                       (int)(n > 32 ? 32 : n),
                       (const char *)p);

  s5 = wrap_vmprintf("%s-%Q-%d",
                     s2 ? s2 : "",
                     s3 ? s3 : "",
                     read_int(&p, &n));

  if(acc1 && s4){
    sqlite3_str_appendall(acc1, s4);
    val1 = sqlite3_str_value(acc1);
  }
  if(acc2 && s5){
    sqlite3_str_appendall(acc2, s5);
    val2 = sqlite3_str_value(acc2);
  }

  loops = (int)(n > 0 ? (p[0] % 8) : 0);
  if(n > 0){
    p++;
    n--;
  }

  for(i = 0; i < loops; i++){
    char *tmp = take_string(&p, &n);
    int choice = read_int(&p, &n) & 3;
    sqlite3_str *target = (choice & 1) ? acc1 : acc2;

    if(target){
      switch(choice){
        case 0:
          sqlite3_str_appendall(target, tmp);
          break;
        case 1:
          sqlite3_str_appendf(target, "%s:%d", tmp ? tmp : "", read_int(&p, &n));
          break;
        case 2:
          sqlite3_str_appendf(target, "%Q", tmp ? tmp : "");
          break;
        default:
          sqlite3_str_appendf(target, "%w-%u", tmp ? tmp : "", read_u32(&p, &n));
          break;
      }
      (void)sqlite3_str_value(target);
    }

    sqlite3_free(tmp);
  }

  fin1 = sqlite3_str_finish(acc1);
  acc1 = NULL;
  fin2 = sqlite3_str_finish(acc2);
  acc2 = NULL;

  sqlite3_free(fin1);
  sqlite3_free(fin2);
  sqlite3_free(s1);
  sqlite3_free(s2);
  sqlite3_free(s3);
  sqlite3_free(s4);
  sqlite3_free(s5);

  if(db){
    sqlite3_close(db);
  }
  return 0;
}