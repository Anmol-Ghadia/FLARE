// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_byte(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  int v = **data;
  (*data)++;
  (*size)--;
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  (void)sqlite3_initialize();

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlite3 *db = NULL;
  sqlite3_mutex *m1 = NULL;
  sqlite3_mutex *m2 = NULL;
  int m1_type = SQLITE_MUTEX_FAST;
  int m2_type = SQLITE_MUTEX_RECURSIVE;
  int m1_locked = 0;
  int m2_locked = 0;

  int open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (read_byte(&p, &remaining) & 1) open_flags |= SQLITE_OPEN_FULLMUTEX;
  if (read_byte(&p, &remaining) & 1) open_flags |= SQLITE_OPEN_URI;
  if (read_byte(&p, &remaining) & 1) open_flags |= SQLITE_OPEN_MEMORY;

  (void)sqlite3_open_v2("./dummy_file", &db, open_flags, NULL);

  m1_type = (read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST : SQLITE_MUTEX_RECURSIVE;
  m2_type = (read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST : SQLITE_MUTEX_RECURSIVE;
  m1 = sqlite3_mutex_alloc(m1_type);
  m2 = sqlite3_mutex_alloc(m2_type);

  for (int i = 0; i < 64 && remaining > 0; ++i) {
    int op = read_byte(&p, &remaining) % 14;
    int target = read_byte(&p, &remaining) % 3;

    sqlite3_mutex *mtx = NULL;
    int *locked = NULL;
    int mtx_type = SQLITE_MUTEX_FAST;

    if (target == 0) {
      mtx = m1;
      locked = &m1_locked;
      mtx_type = m1_type;
    } else if (target == 1) {
      mtx = m2;
      locked = &m2_locked;
      mtx_type = m2_type;
    }

    switch (op) {
      case 0:
        if (mtx) {
          if (mtx_type == SQLITE_MUTEX_RECURSIVE || (locked && *locked == 0)) {
            sqlite3_mutex_enter(mtx);
            if (locked) (*locked)++;
          }
        } else {
          sqlite3_mutex_enter(NULL);
        }
        break;

      case 1:
        if (mtx) {
          if (mtx_type == SQLITE_MUTEX_RECURSIVE || (locked && *locked == 0)) {
            int rc = sqlite3_mutex_try(mtx);
            if (locked && rc == SQLITE_OK) {
              (*locked)++;
            }
          }
        } else {
          (void)sqlite3_mutex_try(NULL);
        }
        break;

      case 2:
        if (mtx && locked && *locked > 0) {
          sqlite3_mutex_leave(mtx);
          (*locked)--;
        } else {
          sqlite3_mutex_leave(NULL);
        }
        break;

      case 3:
        if (m1 && m1_locked == 0) {
          sqlite3_mutex_free(m1);
          m1 = NULL;
        }
        break;

      case 4:
        if (m2 && m2_locked == 0) {
          sqlite3_mutex_free(m2);
          m2 = NULL;
        }
        break;

      case 5:
        if (!m1) {
          m1_type = (read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST : SQLITE_MUTEX_RECURSIVE;
          m1 = sqlite3_mutex_alloc(m1_type);
          m1_locked = 0;
        }
        break;

      case 6:
        if (!m2) {
          m2_type = (read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST : SQLITE_MUTEX_RECURSIVE;
          m2 = sqlite3_mutex_alloc(m2_type);
          m2_locked = 0;
        }
        break;

      case 7:
        if (db) {
          char sql[96];
          uint32_t v = read_u32(&p, &remaining);
          snprintf(sql, sizeof(sql),
                   "CREATE TABLE IF NOT EXISTS t(x);"
                   "INSERT INTO t VALUES(%u);",
                   v);
          (void)sqlite3_exec(db, sql, NULL, NULL, NULL);
        }
        break;

      case 8:
        if (db) {
          (void)sqlite3_exec(db, "PRAGMA integrity_check;", NULL, NULL, NULL);
        }
        break;

      case 9:
        if (db) {
          sqlite3_mutex *dbm = sqlite3_db_mutex(db);
          if (dbm) {
            sqlite3_mutex_enter(dbm);
            sqlite3_mutex_leave(dbm);
          } else {
            sqlite3_mutex_enter(NULL);
            sqlite3_mutex_leave(NULL);
          }
        }
        break;

      case 10:
        if (db) {
          sqlite3_mutex *dbm = sqlite3_db_mutex(db);
          (void)dbm;
        }
        break;

      case 11:
        (void)sqlite3_mutex_try(NULL);
        break;

      case 12:
        sqlite3_mutex_enter(NULL);
        sqlite3_mutex_leave(NULL);
        sqlite3_mutex_free(NULL);
        break;

      case 13:
        if (db && (read_byte(&p, &remaining) & 1)) {
          sqlite3_close(db);
          db = NULL;
          (void)sqlite3_open_v2("./dummy_file", &db, open_flags, NULL);
        }
        break;
    }
  }

  while (m1 && m1_locked > 0) {
    sqlite3_mutex_leave(m1);
    m1_locked--;
  }
  while (m2 && m2_locked > 0) {
    sqlite3_mutex_leave(m2);
    m2_locked--;
  }

  if (m1) {
    sqlite3_mutex_free(m1);
    m1 = NULL;
  }
  if (m2) {
    sqlite3_mutex_free(m2);
    m2 = NULL;
  }

  if (db) {
    sqlite3_close(db);
    db = NULL;
  }

  return 0;
}