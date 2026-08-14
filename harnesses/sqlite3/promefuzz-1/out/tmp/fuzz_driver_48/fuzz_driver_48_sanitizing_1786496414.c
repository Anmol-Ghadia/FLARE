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

  sqlite3_mutex *m1 = NULL;
  sqlite3_mutex *m2 = NULL;
  sqlite3 *db = NULL;
  sqlite3_mutex *dbm = NULL;
  int m1_locked = 0;
  int m2_locked = 0;
  int dbm_locked = 0;

  int open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (read_byte(&p, &remaining) & 1) open_flags |= SQLITE_OPEN_FULLMUTEX;
  if (read_byte(&p, &remaining) & 1) open_flags |= SQLITE_OPEN_URI;
  if (read_byte(&p, &remaining) & 1) open_flags |= SQLITE_OPEN_MEMORY;

  (void)sqlite3_open_v2("./dummy_file", &db, open_flags, NULL);
  if (db) {
    dbm = sqlite3_db_mutex(db);
  }

  int type1 = (read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST : SQLITE_MUTEX_RECURSIVE;
  int type2 = (read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST : SQLITE_MUTEX_RECURSIVE;

  m1 = sqlite3_mutex_alloc(type1);
  m2 = sqlite3_mutex_alloc(type2);

  for (int i = 0; i < 32 && remaining > 0; ++i) {
    int op = read_byte(&p, &remaining) % 12;
    int target = read_byte(&p, &remaining) % 4;
    sqlite3_mutex *mtx = NULL;
    int *locked = NULL;

    switch (target) {
      case 0:
        mtx = m1;
        locked = &m1_locked;
        break;
      case 1:
        mtx = m2;
        locked = &m2_locked;
        break;
      case 2:
        mtx = dbm;
        locked = &dbm_locked;
        break;
      default:
        mtx = NULL;
        locked = NULL;
        break;
    }

    switch (op) {
      case 0:
        sqlite3_mutex_enter(mtx);
        if (locked && mtx) {
          (*locked)++;
        }
        break;

      case 1: {
        int rc = sqlite3_mutex_try(mtx);
        if (locked && mtx && rc == SQLITE_OK) {
          (*locked)++;
        }
        break;
      }

      case 2:
        if (locked && mtx && *locked > 0) {
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
          m1 = sqlite3_mutex_alloc((read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST
                                                                    : SQLITE_MUTEX_RECURSIVE);
          m1_locked = 0;
        }
        break;

      case 6:
        if (!m2) {
          m2 = sqlite3_mutex_alloc((read_byte(&p, &remaining) & 1) ? SQLITE_MUTEX_FAST
                                                                    : SQLITE_MUTEX_RECURSIVE);
          m2_locked = 0;
        }
        break;

      case 7:
        if (db) {
          char sql[64];
          uint32_t v = read_u32(&p, &remaining);
          snprintf(sql, sizeof(sql),
                   "CREATE TABLE IF NOT EXISTS t(x);"
                   "INSERT INTO t VALUES(%u);",
                   v);
          (void)sqlite3_exec(db, sql, NULL, NULL, NULL);
          dbm = sqlite3_db_mutex(db);
        }
        break;

      case 8:
        if (db) {
          (void)sqlite3_exec(db, "PRAGMA integrity_check;", NULL, NULL, NULL);
          dbm = sqlite3_db_mutex(db);
        }
        break;

      case 9:
        dbm = db ? sqlite3_db_mutex(db) : NULL;
        break;

      case 10:
        (void)sqlite3_mutex_try(NULL);
        break;

      case 11:
        sqlite3_mutex_enter(NULL);
        sqlite3_mutex_leave(NULL);
        sqlite3_mutex_free(NULL);
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
  while (dbm && dbm_locked > 0) {
    sqlite3_mutex_leave(dbm);
    dbm_locked--;
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