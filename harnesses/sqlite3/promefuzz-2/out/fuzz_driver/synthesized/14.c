// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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

static int consume_int(const uint8_t **data, size_t *size) {
  int v = 0;
  size_t n = *size < sizeof(int) ? *size : sizeof(int);
  if (n > 0) {
    memcpy(&v, *data, n);
    *data += n;
    *size -= n;
  }
  return v;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;
  void *mem = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  {
    int mode1 = consume_int(&p, &remaining);
    int mode2 = consume_int(&p, &remaining);
    int uri_enable = consume_int(&p, &remaining) & 1;
    int shared_cache = consume_int(&p, &remaining) & 1;
    int rc_open;
    const char *filename;

    switch ((unsigned)mode1 % 3) {
      case 0:
        sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        break;
      case 1:
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        break;
      default:
        sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        break;
    }

    sqlite3_config(SQLITE_CONFIG_URI, uri_enable);

    switch ((unsigned)mode2 % 3) {
      case 0:
        sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);
        break;
      case 1:
        sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 1);
        break;
      default:
        sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, (consume_int(&p, &remaining) & 1));
        break;
    }

    (void)sqlite3_threadsafe();
    (void)sqlite3_initialize();
    (void)sqlite3_enable_shared_cache(shared_cache);

    switch (remaining ? (p[0] % 4) : 0) {
      case 0:
        filename = ":memory:";
        break;
      case 1:
        filename = "";
        break;
      case 2:
        filename = "./dummy_file";
        break;
      default:
        filename = "file:./dummy_file?mode=rwc";
        break;
    }

    rc_open = sqlite3_open(filename, &db);
    (void)rc_open;
  }

  (void)sqlite3_close(db);

  mem = sqlite3_malloc((int)(remaining > 4096 ? 4096 : remaining));
  sqlite3_free(mem);

  return 0;
}