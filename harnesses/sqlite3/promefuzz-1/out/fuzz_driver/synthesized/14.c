// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
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

static unsigned char consume_u8(const uint8_t **data, size_t *size) {
  unsigned char v = 0;
  if (*size > 0) {
    v = **data;
    (*data)++;
    (*size)--;
  }
  return v;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;
  void *mem = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (remaining > 0) {
      fwrite(p, 1, remaining, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_shutdown();
  (void)rc;

  {
    int cfg1 = consume_u8(&p, &remaining) % 4;
    switch (cfg1) {
      case 0:
        rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        break;
      case 1:
        rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        break;
      case 2:
        rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        break;
      default:
        rc = sqlite3_config(SQLITE_CONFIG_URI, consume_int(&p, &remaining) & 1);
        break;
    }
    (void)rc;
  }

  {
    int cfg2 = consume_u8(&p, &remaining) % 3;
    switch (cfg2) {
      case 0:
        rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, consume_int(&p, &remaining) & 1);
        break;
      case 1:
        rc = sqlite3_config(SQLITE_CONFIG_URI, consume_int(&p, &remaining) & 1);
        break;
      default:
        rc = sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, consume_int(&p, &remaining) & 1);
        break;
    }
    (void)rc;
  }

  {
    int cfg3 = consume_u8(&p, &remaining) % 3;
    switch (cfg3) {
      case 0:
        rc = sqlite3_config(SQLITE_CONFIG_SORTERREF_SIZE, consume_int(&p, &remaining));
        break;
      case 1:
        rc = sqlite3_config(SQLITE_CONFIG_STMTJRNL_SPILL, consume_int(&p, &remaining));
        break;
      default:
        rc = sqlite3_config(SQLITE_CONFIG_LOOKASIDE, (void *)0, 0, 0);
        break;
    }
    (void)rc;
  }

  (void)sqlite3_threadsafe();

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    sqlite3_free(mem);
    sqlite3_shutdown();
    return 0;
  }

  rc = sqlite3_enable_shared_cache(consume_int(&p, &remaining) & 1);
  (void)rc;

  {
    const char *filename;
    switch (consume_u8(&p, &remaining) % 5) {
      case 0:
        filename = ":memory:";
        break;
      case 1:
        filename = "";
        break;
      case 2:
        filename = "./dummy_file";
        break;
      case 3:
        filename = "file:./dummy_file?mode=ro";
        break;
      default:
        filename = "file:./dummy_file?mode=memory&cache=shared";
        break;
    }

    rc = sqlite3_open(filename, &db);
    (void)rc;
  }

  rc = sqlite3_close(db);
  if (rc == SQLITE_BUSY && db != NULL) {
    sqlite3_close_v2(db);
    db = NULL;
  } else {
    db = NULL;
  }

  sqlite3_free(mem);
  sqlite3_shutdown();
  return 0;
}