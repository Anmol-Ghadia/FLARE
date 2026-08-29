// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_memory_alarm at sqlite3.c:15508:16 in sqlite3.h
// sqlite3_memory_alarm at sqlite3.c:15508:16 in sqlite3.h
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

static int read_int(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static const char *pick_dbname(const uint8_t **data, size_t *size) {
  static const char *names[] = { NULL, "main", "temp", "invalid" };
  if (*size == 0) return "main";
  uint8_t idx = **data;
  (*data)++;
  (*size)--;
  return names[idx % 4];
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

static void dummy_alarm(void *arg, sqlite3_int64 used, int n) {
  (void)arg;
  (void)used;
  (void)n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;

  write_dummy_file(Data, Size);

  (void)sqlite3_initialize();

  int open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_OPEN_URI
  if (remaining && (p[0] & 1)) open_flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
  if (remaining && (p[0] & 2)) open_flags |= SQLITE_OPEN_MEMORY;
#endif

  (void)sqlite3_open_v2("./dummy_file", &db, open_flags, NULL);

  for (int i = 0; i < 8; ++i) {
    int action = remaining ? (int)(*p++ % 6) : i;
    if (remaining) remaining--;

    switch (action) {
      case 0: {
        int n = read_int(&p, &remaining);
        void *mem = sqlite3_malloc(n);
        if (mem) {
          size_t fill = 0;
          if (remaining) {
            fill = remaining < 32 ? remaining : 32;
            memcpy(mem, p, fill);
            p += fill;
            remaining -= fill;
          }
          sqlite3_free(mem);
        }
        break;
      }

      case 1: {
        int n = read_int(&p, &remaining);
        (void)sqlite3_release_memory(n);
        break;
      }

      case 2: {
        sqlite3_int64 threshold = (sqlite3_int64)read_u32(&p, &remaining);
        (void)sqlite3_memory_alarm(dummy_alarm, NULL, threshold);
        (void)sqlite3_memory_alarm(NULL, NULL, 0);
        break;
      }

      case 3: {
        if (db) {
          const char *zDbName = pick_dbname(&p, &remaining);
          int which = remaining ? (*p++ % 6) : 0;
          if (remaining) remaining--;

          sqlite3_file *file_ptr = NULL;
          sqlite3_vfs *vfs_ptr = NULL;
          sqlite3_file *journal_ptr = NULL;
          int data_version = 0;
          int generic_arg = read_int(&p, &remaining);

          switch (which) {
            case 0:
              (void)sqlite3_file_control(db, zDbName, SQLITE_FCNTL_FILE_POINTER, &file_ptr);
              break;
            case 1:
              (void)sqlite3_file_control(db, zDbName, SQLITE_FCNTL_VFS_POINTER, &vfs_ptr);
              break;
            case 2:
              (void)sqlite3_file_control(db, zDbName, SQLITE_FCNTL_JOURNAL_POINTER, &journal_ptr);
              break;
            case 3:
              (void)sqlite3_file_control(db, zDbName, SQLITE_FCNTL_DATA_VERSION, &data_version);
              break;
            case 4:
              (void)sqlite3_file_control(db, zDbName, generic_arg, &generic_arg);
              break;
            default:
              (void)sqlite3_file_control(db, NULL, SQLITE_FCNTL_DATA_VERSION, &data_version);
              break;
          }
        }
        break;
      }

      case 4: {
#ifdef SQLITE_TESTCTRL_PRNG_RESET
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
#endif
        break;
      }

      case 5: {
#ifdef SQLITE_TESTCTRL_PENDING_BYTE
        unsigned int pending = (read_u32(&p, &remaining) | 0x40000000u);
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, (unsigned int)pending);
#endif
        break;
      }

      default:
        break;
    }
  }

  if (db) {
    sqlite3_close(db);
  }

  return 0;
}