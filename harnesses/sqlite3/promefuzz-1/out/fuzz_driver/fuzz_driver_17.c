// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static int consume_int(const uint8_t **data, size_t *size) {
  int v = 0;
  size_t n = *size < sizeof(v) ? *size : sizeof(v);
  if (n > 0) {
    memcpy(&v, *data, n);
    *data += n;
    *size -= n;
  }
  return v;
}

static size_t consume_bytes(const uint8_t **data, size_t *size, char *out, size_t max_out) {
  size_t n = *size < (max_out - 1) ? *size : (max_out - 1);
  if (max_out == 0) return 0;
  if (n > 0) {
    memcpy(out, *data, n);
    *data += n;
    *size -= n;
  }
  out[n] = '\0';
  return n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  int rc;
  sqlite3_vfs *vfs;
  const char *s1, *s2, *s3;
  char vfs_name[256];
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    return 0;
  }

  if (remaining == 0) {
    vfs = sqlite3_vfs_find(NULL);
    (void)vfs;
  } else {
    int mode = consume_int(&p, &remaining);

    switch ((unsigned int)mode % 4U) {
      case 0:
        vfs = sqlite3_vfs_find(NULL);
        break;
      case 1:
        consume_bytes(&p, &remaining, vfs_name, sizeof(vfs_name));
        vfs = sqlite3_vfs_find(vfs_name);
        break;
      case 2:
        vfs = sqlite3_vfs_find("");
        break;
      default:
        consume_bytes(&p, &remaining, vfs_name, sizeof(vfs_name));
        if (vfs_name[0] == '\0') {
          strcpy(vfs_name, "unix");
        }
        vfs = sqlite3_vfs_find(vfs_name);
        break;
    }
    (void)vfs;
  }

  s1 = sqlite3_libversion();
  s2 = sqlite3_sourceid();
  (void)s1;
  (void)s2;

  {
    int idx1 = consume_int(&p, &remaining);
    int idx2 = consume_int(&p, &remaining);
    int idx3 = consume_int(&p, &remaining);

    s3 = sqlite3_compileoption_get(idx1);
    (void)s3;
    s3 = sqlite3_compileoption_get(idx2);
    (void)s3;
    s3 = sqlite3_compileoption_get(idx3);
    (void)s3;

    if (idx1 >= 0) {
      int i;
      int limit = (idx1 & 31);
      for (i = 0; i < limit; i++) {
        s3 = sqlite3_compileoption_get(i);
        if (s3 == NULL) break;
      }
    }
  }

  return 0;
}