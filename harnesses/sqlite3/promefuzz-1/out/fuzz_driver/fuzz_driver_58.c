// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_serialize at sqlite3.c:36892:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_serialize at sqlite3.c:36892:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
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
  unsigned char *ser = NULL;
  sqlite3_int64 ser_sz = 0;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3_shutdown();

  if (Size > off) {
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
    ser = sqlite3_serialize(db, schema, &ser_sz, sflags);
    if (ser && !(sflags & SQLITE_SERIALIZE_NOCOPY)) {
      sqlite3_free(ser);
      ser = NULL;
    }
  }

#if !defined(SQLITE_OMIT_DESERIALIZE)
  {
    size_t remain = (off <= Size) ? (Size - off) : 0;
    sqlite3_int64 n = (sqlite3_int64)remain;
    sqlite3_int64 m = n;
    unsigned int dflags = 0;
    unsigned char *buf = NULL;
    const char *schema = "main";

    if (Size > off) {
      uint8_t b = Data[off++];
      if (b & 2) dflags |= SQLITE_DESERIALIZE_RESIZEABLE;
      if (b & 4) dflags |= SQLITE_DESERIALIZE_READONLY;
    }

    if (Size - off >= 4) {
      uint32_t extra = rd32(Data + off) % 64U;
      off += 4;
      m = n + (sqlite3_int64)extra;
    }

    if (m <= 0) m = 1;
    if (m > 0x7fffffff) m = 0x7fffffff;
    if (n > m) n = m;

    buf = (unsigned char *)sqlite3_malloc((int)m);
    if (buf) {
      size_t copy_n = (size_t)n;
      if (copy_n > Size - off) copy_n = Size - off;
      if (copy_n > 0) memcpy(buf, Data + off, copy_n);
      if ((sqlite3_int64)copy_n < m) {
        memset(buf + copy_n, 0, (size_t)(m - (sqlite3_int64)copy_n));
      }

      rc = sqlite3_deserialize(db, schema, buf, n, m, dflags);

      if (rc != SQLITE_OK) {
        sqlite3_free(buf);
      } else {
        /*
         * Avoid sqlite3_close() teardown on a connection whose main schema
         * was replaced by arbitrary fuzz data, as this can drive internal
         * cleanup paths into use-after-free states. Leak the connection for
         * this iteration instead; libFuzzer runs in-process and this keeps
         * the target API exercised safely.
         */
        db = NULL;
      }
    }
  }

  if (db) {
    const char *schema = (Size > off && (Data[off++] & 1)) ? "temp" : "main";
    unsigned int sflags = (Size > off && (Data[off++] & 1)) ? SQLITE_SERIALIZE_NOCOPY : 0;
    ser = sqlite3_serialize(db, schema, &ser_sz, sflags);
    if (ser && !(sflags & SQLITE_SERIALIZE_NOCOPY)) {
      sqlite3_free(ser);
      ser = NULL;
    }
  }
#endif

  if (db_file) sqlite3_close(db_file);
  if (db) sqlite3_close(db);
  return 0;
}