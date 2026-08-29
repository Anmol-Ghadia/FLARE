// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
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

static int choose_dynamic_mutex_type(uint32_t v) {
  return (v & 1) ? SQLITE_MUTEX_RECURSIVE : SQLITE_MUTEX_FAST;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) {
    fwrite(data, 1, size, fp);
  }
  fclose(fp);
}

static void exercise_mutex(sqlite3_mutex *m, int is_recursive, uint32_t control) {
  int rc;

  (void)sqlite3_mutex_held(NULL);
  (void)sqlite3_mutex_notheld(NULL);
  (void)sqlite3_mutex_try(NULL);

  if (!m) return;

  (void)sqlite3_mutex_held(m);
  (void)sqlite3_mutex_notheld(m);

  if (control & 1U) {
    sqlite3_mutex_enter(m);
    (void)sqlite3_mutex_held(m);
    (void)sqlite3_mutex_notheld(m);

    if (is_recursive && (control & 2U)) {
      rc = sqlite3_mutex_try(m);
      if (rc == SQLITE_OK) {
        (void)sqlite3_mutex_held(m);
        (void)sqlite3_mutex_notheld(m);
        sqlite3_mutex_leave(m);
      }
    }

    sqlite3_mutex_leave(m);
    (void)sqlite3_mutex_notheld(m);
  } else {
    rc = sqlite3_mutex_try(m);
    if (rc == SQLITE_OK) {
      (void)sqlite3_mutex_held(m);
      (void)sqlite3_mutex_notheld(m);
      sqlite3_mutex_leave(m);
      (void)sqlite3_mutex_notheld(m);
    }
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;
  sqlite3_mutex *db_mutex = NULL;
  sqlite3_mutex *m1 = NULL;
  sqlite3_mutex *m2 = NULL;
  int rc;
  int t1, t2;

  write_dummy_file(Data, Size);

  (void)sqlite3_threadsafe();
  (void)sqlite3_mutex_held(NULL);
  (void)sqlite3_mutex_notheld(NULL);
  (void)sqlite3_mutex_try(NULL);

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    return 0;
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc == SQLITE_OK && db != NULL) {
    db_mutex = sqlite3_db_mutex(db);
    (void)sqlite3_mutex_held(db_mutex);
    (void)sqlite3_mutex_notheld(db_mutex);

    if (db_mutex) {
      rc = sqlite3_mutex_try(db_mutex);
      if (rc == SQLITE_OK) {
        (void)sqlite3_mutex_held(db_mutex);
        (void)sqlite3_mutex_notheld(db_mutex);
        sqlite3_mutex_leave(db_mutex);
      }
      (void)sqlite3_mutex_notheld(db_mutex);
    }
  }

  t1 = choose_dynamic_mutex_type(read_u32(&p, &remaining));
  t2 = choose_dynamic_mutex_type(read_u32(&p, &remaining));
  m1 = sqlite3_mutex_alloc(t1);
  m2 = sqlite3_mutex_alloc(t2);

  exercise_mutex(m1, t1 == SQLITE_MUTEX_RECURSIVE, read_u32(&p, &remaining));
  exercise_mutex(m2, t2 == SQLITE_MUTEX_RECURSIVE, read_u32(&p, &remaining));

  if (db) {
    sqlite3_close(db);
  }

  if (m1) {
    sqlite3_mutex_free(m1);
  }
  if (m2) {
    sqlite3_mutex_free(m2);
  }

  sqlite3_shutdown();
  return 0;
}