#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"

static int read_u32(const uint8_t *data, size_t size, size_t *off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4; i++){
    v <<= 8;
    if(*off < size){
      v |= data[*off];
      (*off)++;
    }
  }
  return (int)v;
}

static char *make_cstring(const uint8_t *data, size_t size, size_t *off){
  size_t len;
  char *s;

  if(*off >= size){
    s = (char *)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }

  len = (size_t)(data[*off] % 64);
  (*off)++;

  if(len > size - *off){
    len = size - *off;
  }

  s = (char *)malloc(len + 1);
  if(!s) return NULL;

  if(len > 0){
    memcpy(s, data + *off, len);
    *off += len;
  }
  s[len] = '\0';
  return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t off = 0;
  char *s1 = NULL;
  char *s2 = NULL;
  char *p1 = NULL;
  char *p2 = NULL;
  const char *a1;
  const char *a2;
  int cfg_selector;
  int rc;

  sqlite3_shutdown();

  cfg_selector = (Size > 0) ? (Data[off++] % 8) : 0;
  switch(cfg_selector){
    case 0:
      rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
      (void)rc;
      break;
    case 1:
      rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
      (void)rc;
      break;
    case 2:
      rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
      (void)rc;
      break;
    case 3: {
      int onoff = (Size > off) ? (Data[off++] & 1) : 0;
      rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, onoff);
      (void)rc;
      break;
    }
    case 4: {
      int onoff = (Size > off) ? (Data[off++] & 1) : 0;
      rc = sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, onoff);
      (void)rc;
      break;
    }
    case 5: {
      int n = read_u32(Data, Size, &off) & 0xFFFF;
      rc = sqlite3_config(SQLITE_CONFIG_LOOKASIDE, NULL, n, 8);
      (void)rc;
      break;
    }
    case 6: {
      int n = read_u32(Data, Size, &off) & 0xFFFF;
      rc = sqlite3_config(SQLITE_CONFIG_PCACHE_HDRSZ, &n);
      (void)rc;
      break;
    }
    default:
      rc = sqlite3_config(-1);
      (void)rc;
      break;
  }

  s1 = make_cstring(Data, Size, &off);
  s2 = make_cstring(Data, Size, &off);
  p1 = make_cstring(Data, Size, &off);
  p2 = make_cstring(Data, Size, &off);

  if(!s1 || !s2 || !p1 || !p2){
    free(s1);
    free(s2);
    free(p1);
    free(p2);
    sqlite3_shutdown();
    return 0;
  }

  a1 = (Size > off && (Data[off++] & 1)) ? NULL : (const char *)s1;
  a2 = (Size > off && (Data[off++] & 1)) ? NULL : (const char *)s2;
  (void)sqlite3_stricmp(a1, a2);

  /* sqlite3_strglob() does not tolerate NULL pattern/string pointers. */
  (void)sqlite3_strglob((const char *)p1, (const char *)s1);
  (void)sqlite3_strglob((const char *)p2, (const char *)s2);

  free(s1);
  free(s2);
  free(p1);
  free(p2);

  sqlite3_shutdown();
  return 0;
}