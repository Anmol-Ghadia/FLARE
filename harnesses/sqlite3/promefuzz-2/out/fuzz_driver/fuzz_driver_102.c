// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off, uint32_t *Out) {
  if (*Off + 4 > Size) return 0;
  *Out = ((uint32_t)Data[*Off] << 24) |
         ((uint32_t)Data[*Off + 1] << 16) |
         ((uint32_t)Data[*Off + 2] << 8) |
         ((uint32_t)Data[*Off + 3]);
  *Off += 4;
  return 1;
}

static int choose_mutex_type(uint8_t b) {
  static const int types[] = {
    SQLITE_MUTEX_FAST,
    SQLITE_MUTEX_RECURSIVE,
    SQLITE_MUTEX_STATIC_MAIN,
    SQLITE_MUTEX_STATIC_MEM,
    SQLITE_MUTEX_STATIC_OPEN,
    SQLITE_MUTEX_STATIC_PRNG,
    SQLITE_MUTEX_STATIC_LRU,
    SQLITE_MUTEX_STATIC_PMEM,
    SQLITE_MUTEX_STATIC_APP1,
    SQLITE_MUTEX_STATIC_APP2,
    SQLITE_MUTEX_STATIC_APP3,
    SQLITE_MUTEX_STATIC_VFS1,
    SQLITE_MUTEX_STATIC_VFS2,
    SQLITE_MUTEX_STATIC_VFS3
  };
  return types[b % (sizeof(types) / sizeof(types[0]))];
}

static void close_db(sqlite3 **pdb) {
  if (*pdb) {
    sqlite3_close(*pdb);
    *pdb = NULL;
  }
}

static void free_dynamic_mutexes(sqlite3_mutex **dynamic_mutexes, size_t *dynamic_count) {
  while (*dynamic_count > 0) {
    if (dynamic_mutexes[*dynamic_count - 1]) {
      sqlite3_mutex_free(dynamic_mutexes[*dynamic_count - 1]);
      dynamic_mutexes[*dynamic_count - 1] = NULL;
    }
    (*dynamic_count)--;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  uint32_t nops = 0;
  sqlite3 *db = NULL;
  sqlite3_mutex *dynamic_mutexes[32];
  size_t dynamic_count = 0;
  int shutdown_called = 0;

  memset(dynamic_mutexes, 0, sizeof(dynamic_mutexes));

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  if (sqlite3_initialize() != SQLITE_OK) {
    sqlite3_thread_cleanup();
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
      close_db(&db);
    }
  } else {
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
      close_db(&db);
    }
  }

  if (!read_u32(Data, Size, &off, &nops)) {
    nops = (uint32_t)Size;
  }
  nops = (nops % 64U) + 1U;

  for (uint32_t i = 0; i < nops; i++) {
    uint8_t op = 0;
    uint8_t arg = 0;
    sqlite3_mutex *m = NULL;
    int rc;

    if (off < Size) op = Data[off++];
    if (off < Size) arg = Data[off++];

    switch (op % 8) {
      case 0: {
        int type = choose_mutex_type(arg);
        m = sqlite3_mutex_alloc(type);
        if (m && (type == SQLITE_MUTEX_FAST || type == SQLITE_MUTEX_RECURSIVE)) {
          if (dynamic_count < sizeof(dynamic_mutexes) / sizeof(dynamic_mutexes[0])) {
            dynamic_mutexes[dynamic_count++] = m;
          } else {
            sqlite3_mutex_free(m);
          }
        }
        break;
      }

      case 1: {
        if (dynamic_count > 0) {
          m = dynamic_mutexes[arg % dynamic_count];
        }
        rc = sqlite3_mutex_try(m);
        if (m && rc == SQLITE_OK) {
          sqlite3_mutex_leave(m);
        }
        break;
      }

      case 2: {
        m = sqlite3_db_mutex(db);
        rc = sqlite3_mutex_try(m);
        if (m && rc == SQLITE_OK) {
          sqlite3_mutex_leave(m);
        }
        break;
      }

      case 3: {
        if (dynamic_count > 0) {
          size_t idx = arg % dynamic_count;
          if (dynamic_mutexes[idx]) {
            sqlite3_mutex_free(dynamic_mutexes[idx]);
            dynamic_mutexes[idx] = dynamic_mutexes[dynamic_count - 1];
            dynamic_mutexes[dynamic_count - 1] = NULL;
            dynamic_count--;
          }
        } else {
          sqlite3_mutex_free(NULL);
        }
        break;
      }

      case 4: {
        sqlite3_thread_cleanup();
        break;
      }

      case 5: {
        close_db(&db);
        free_dynamic_mutexes(dynamic_mutexes, &dynamic_count);
        rc = sqlite3_shutdown();
        (void)rc;
        shutdown_called = 1;
        if (sqlite3_initialize() == SQLITE_OK) {
          shutdown_called = 0;
          if (arg & 1) {
            if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
              close_db(&db);
            }
          } else {
            if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
              close_db(&db);
            }
          }
        }
        break;
      }

      case 6: {
        rc = sqlite3_mutex_try(NULL);
        (void)rc;
        sqlite3_mutex_free(NULL);
        break;
      }

      case 7: {
        m = sqlite3_db_mutex(NULL);
        rc = sqlite3_mutex_try(m);
        if (m && rc == SQLITE_OK) {
          sqlite3_mutex_leave(m);
        }
        break;
      }
    }
  }

  close_db(&db);
  free_dynamic_mutexes(dynamic_mutexes, &dynamic_count);

  sqlite3_thread_cleanup();
  if (!shutdown_called) {
    sqlite3_shutdown();
  }
  return 0;
}