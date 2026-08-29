// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_reset_auto_extension at sqlite3.c:114881:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_reset_auto_extension at sqlite3.c:114881:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static int choose_flags(uint32_t bits) {
  int flags = 0;

  if (bits & 1U) flags |= SQLITE_OPEN_READONLY;
  if (bits & 2U) flags |= SQLITE_OPEN_READWRITE;
  if (bits & 4U) flags |= SQLITE_OPEN_CREATE;
  if (bits & 8U) flags |= SQLITE_OPEN_URI;
  if (bits & 16U) flags |= SQLITE_OPEN_MEMORY;
  if (bits & 32U) flags |= SQLITE_OPEN_NOMUTEX;
  if (bits & 64U) flags |= SQLITE_OPEN_FULLMUTEX;
  if (bits & 128U) flags |= SQLITE_OPEN_SHAREDCACHE;
  if (bits & 256U) flags |= SQLITE_OPEN_PRIVATECACHE;
#ifdef SQLITE_OPEN_EXRESCODE
  if (bits & 512U) flags |= SQLITE_OPEN_EXRESCODE;
#endif
#ifdef SQLITE_OPEN_NOFOLLOW
  if (bits & 1024U) flags |= SQLITE_OPEN_NOFOLLOW;
#endif

  if ((flags & (SQLITE_OPEN_READONLY | SQLITE_OPEN_READWRITE)) == 0) {
    flags |= SQLITE_OPEN_READWRITE;
  }

  return flags;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_vfs *default_vfs = NULL;
  char vfs_name_buf[128];
  const char *zVfs = NULL;
  int rc;
  int i;

  write_dummy_file(Data, Size);

  rc = sqlite3_initialize();
  (void)rc;

  sqlite3_reset_auto_extension();

  {
    int n1 = (int)(read_u32(Data, Size, &off) % 4096U);
    void *p1 = NULL;
    if ((off < Size) && (Data[off++] & 1U) && n1 > 0) {
      p1 = malloc((size_t)n1);
      if (p1) {
        memset(p1, 0, (size_t)n1);
      }
    }
    sqlite3_randomness(n1, p1);
    free(p1);
  }

  sqlite3_randomness(0, NULL);

  {
    int n2 = (int)(read_u32(Data, Size, &off) % 256U);
    unsigned char stackbuf[256];
    sqlite3_randomness(n2, stackbuf);
  }

  default_vfs = sqlite3_vfs_find(NULL);
  if (default_vfs) {
    sqlite3_vfs_register(default_vfs, (off < Size) ? (Data[off++] & 1U) : 0);
  }

  if (default_vfs && default_vfs->zName) {
    size_t len = strlen(default_vfs->zName);
    if (len >= sizeof(vfs_name_buf)) len = sizeof(vfs_name_buf) - 1;
    memcpy(vfs_name_buf, default_vfs->zName, len);
    vfs_name_buf[len] = '\0';
    if ((off < Size) && (Data[off++] & 1U)) {
      zVfs = vfs_name_buf;
    }
  }

  {
    int flags = choose_flags(read_u32(Data, Size, &off));
    const char *filename;

    if ((off < Size) && (Data[off++] & 1U)) {
      filename = ":memory:";
    } else if ((off < Size) && (Data[off++] & 1U)) {
      filename = "";
    } else {
      filename = "./dummy_file";
    }

    rc = sqlite3_open_v2(filename, &db, flags, zVfs);
    if (db) {
      sqlite3_close(db);
      db = NULL;
    }
    (void)rc;
  }

  for (i = 0; i < 3; ++i) {
    int flags = choose_flags(read_u32(Data, Size, &off));
    const char *filename = ((off < Size) && (Data[off++] & 1U)) ? ":memory:" : "./dummy_file";
    const char *use_vfs = ((off < Size) && (Data[off++] & 1U)) ? zVfs : NULL;

    rc = sqlite3_open_v2(filename, &db, flags, use_vfs);
    if (db) {
      sqlite3_close(db);
      db = NULL;
    }
    (void)rc;

    if ((off < Size) && (Data[off++] & 1U)) {
      sqlite3_reset_auto_extension();
    }

    {
      int n = (int)(read_u32(Data, Size, &off) % 512U);
      void *p = NULL;
      if (((off < Size) && (Data[off++] & 1U)) && n > 0) {
        p = malloc((size_t)n);
      }
      sqlite3_randomness(n, p);
      free(p);
    }
  }

  sqlite3_shutdown();
  return 0;
}