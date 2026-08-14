#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t rd32(const uint8_t *p) {
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3 *db_file = NULL;
  unsigned char *ser1 = NULL;
  unsigned char *ser2 = NULL;
  sqlite3_int64 ser1_sz = 0;
  sqlite3_int64 ser2_sz = 0;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3_shutdown();

  if (Size > 0) {
    switch (Data[off++] % 4) {
      case 0:
        sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        break;
      case 1:
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        break;
      case 2:
        sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        break;
      default:
        sqlite3_config(0x7fffffff);
        break;
    }
  } else {
    sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
  }

  sqlite3_initialize();

  if (Size > off) {
    int n = (int)(Data[off++] % 128);
    void *p = sqlite3_malloc(n);
    sqlite3_free(p);
  } else {
    void *p = sqlite3_malloc(16);
    sqlite3_free(p);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
  }

  sqlite3_open("./dummy_file", &db_file);

  {
    const char *schema = (Size > off && (Data[off++] & 1)) ? "temp" : "main";
    unsigned int sflags = (Size > off && (Data[off++] & 1)) ? SQLITE_SERIALIZE_NOCOPY : 0;
    ser1 = sqlite3_serialize(db, schema, &ser1_sz, sflags);
    if (ser1 && !(sflags & SQLITE_SERIALIZE_NOCOPY)) {
      sqlite3_free(ser1);
      ser1 = NULL;
    }
  }

#if !defined(SQLITE_OMIT_DESERIALIZE)
  {
    size_t remain = (off <= Size) ? (Size - off) : 0;
    sqlite3_int64 n = (sqlite3_int64)remain;
    sqlite3_int64 m = n;
    unsigned int dflags = 0;
    unsigned char *buf = NULL;
    const char *schema = (Size > off && (Data[off++] & 1)) ? "temp" : "main";

    if (Size > off) {
      uint8_t b = Data[off++];
      if (b & 1) dflags |= SQLITE_DESERIALIZE_FREEONCLOSE;
      if (b & 2) dflags |= SQLITE_DESERIALIZE_RESIZEABLE;
      if (b & 4) dflags |= SQLITE_DESERIALIZE_READONLY;
    }

    if (remain > 0) {
      if (Size - off >= 4) {
        uint32_t extra = rd32(Data + off) % 64U;
        off += 4;
        m = n + (sqlite3_int64)extra;
      }

      buf = (unsigned char *)sqlite3_malloc((int)(m > 0x7fffffff ? 0x7fffffff : m));
      if (buf) {
        size_t copy_n = (size_t)n;
        size_t alloc_n = (size_t)m;
        if (copy_n > alloc_n) copy_n = alloc_n;
        if (copy_n > Size - off) copy_n = Size - off;
        memcpy(buf, Data + off, copy_n);
        if (alloc_n > copy_n) memset(buf + copy_n, 0, alloc_n - copy_n);

        rc = sqlite3_deserialize(db, schema, buf, n, m, dflags);
        if (rc != SQLITE_OK) {
          if (!(dflags & SQLITE_DESERIALIZE_FREEONCLOSE)) {
            sqlite3_free(buf);
          }
        } else {
          if (!(dflags & SQLITE_DESERIALIZE_FREEONCLOSE)) {
            sqlite3_free(buf);
          }
        }
      }
    } else {
      buf = (unsigned char *)sqlite3_malloc(1);
      if (buf) {
        buf[0] = 0;
        rc = sqlite3_deserialize(db, schema, buf, 0, 1, dflags);
        if (rc != SQLITE_OK) {
          if (!(dflags & SQLITE_DESERIALIZE_FREEONCLOSE)) {
            sqlite3_free(buf);
          }
        } else {
          if (!(dflags & SQLITE_DESERIALIZE_FREEONCLOSE)) {
            sqlite3_free(buf);
          }
        }
      }
    }
  }

  {
    const char *schema = (Size > off && (Data[off++] & 1)) ? "temp" : "main";
    unsigned int sflags = (Size > off && (Data[off++] & 1)) ? SQLITE_SERIALIZE_NOCOPY : 0;
    ser2 = sqlite3_serialize(db, schema, &ser2_sz, sflags);
    if (ser2 && !(sflags & SQLITE_SERIALIZE_NOCOPY)) {
      sqlite3_free(ser2);
      ser2 = NULL;
    }
  }
#endif

  sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);

  if (db_file) sqlite3_close(db_file);
  sqlite3_close(db);
  sqlite3_shutdown();
  return 0;
}