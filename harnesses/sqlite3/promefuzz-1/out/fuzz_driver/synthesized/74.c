// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_value at sqlite3.c:17347:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_appendall at sqlite3.c:17280:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
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
  size_t len;
  if(*n == 0){
    return dup_cstring_from_bytes((const uint8_t *)"", 0);
  }
  len = read_u32(p, n);
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

static void safe_appendall(sqlite3_str *acc, const char *s){
  if(acc) sqlite3_str_appendall(acc, s ? s : "");
}

static void safe_append_literal(sqlite3_str *acc, const char *s){
  if(acc) sqlite3_str_appendall(acc, s ? s : "");
}

static void safe_append_quoted(sqlite3_str *acc, const char *s){
  char *tmp;
  if(!acc) return;
  tmp = sqlite3_mprintf("%q", s ? s : "");
  if(tmp){
    sqlite3_str_appendall(acc, tmp);
    sqlite3_free(tmp);
  }
}

static void safe_append_double_quoted(sqlite3_str *acc, const char *s){
  char *tmp;
  if(!acc) return;
  tmp = sqlite3_mprintf("%Q", s ? s : "");
  if(tmp){
    sqlite3_str_appendall(acc, tmp);
    sqlite3_free(tmp);
  }
}

static void safe_append_ident_escaped(sqlite3_str *acc, const char *s){
  char *tmp;
  if(!acc) return;
  tmp = sqlite3_mprintf("%w", s ? s : "");
  if(tmp){
    sqlite3_str_appendall(acc, tmp);
    sqlite3_free(tmp);
  }
}

int LLVMFuzzerTestOneInput_74(const uint8_t *Data, size_t Size){
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

  (void)sqlite3_str_value(NULL);

  s1 = take_string(&p, &n);
  s2 = take_string(&p, &n);
  s3 = take_string(&p, &n);

  safe_appendall(acc1, s1);
  val1 = sqlite3_str_value(acc1);
  (void)val1;

  safe_append_literal(acc1, s2);
  val1 = sqlite3_str_value(acc1);
  (void)val1;

  {
    char *tmp = sqlite3_mprintf("%d:%u:%s", read_int(&p, &n), read_u32(&p, &n), s3 ? s3 : "");
    if(tmp){
      sqlite3_str_appendall(acc1, tmp);
      sqlite3_free(tmp);
    }
  }
  val1 = sqlite3_str_value(acc1);
  (void)val1;

  safe_appendall(acc2, s2);
  val2 = sqlite3_str_value(acc2);
  (void)val2;

  safe_append_quoted(acc2, s1);
  val2 = sqlite3_str_value(acc2);
  (void)val2;

  safe_appendall(acc2, s3);
  val2 = sqlite3_str_value(acc2);
  (void)val2;

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

  safe_appendall(acc1, s4);
  val1 = sqlite3_str_value(acc1);
  (void)val1;

  safe_appendall(acc2, s5);
  val2 = sqlite3_str_value(acc2);
  (void)val2;

  loops = (int)(n > 0 ? (p[0] % 8) : 0);
  if(n > 0){
    p++;
    n--;
  }

  for(i = 0; i < loops; i++){
    char *tmp = take_string(&p, &n);
    int choice = read_int(&p, &n) & 3;
    sqlite3_str *target = (choice & 1) ? acc1 : acc2;

    switch(choice){
      case 0:
        safe_appendall(target, tmp);
        break;
      case 1: {
        char *fmted = sqlite3_mprintf("%s:%d", tmp ? tmp : "", read_int(&p, &n));
        if(fmted){
          sqlite3_str_appendall(target, fmted);
          sqlite3_free(fmted);
        }
        break;
      }
      case 2:
        safe_append_double_quoted(target, tmp);
        break;
      default: {
        safe_append_ident_escaped(target, tmp);
        {
          char *fmted = sqlite3_mprintf("-%u", read_u32(&p, &n));
          if(fmted){
            sqlite3_str_appendall(target, fmted);
            sqlite3_free(fmted);
          }
        }
        break;
      }
    }
    (void)sqlite3_str_value(target);
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