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

static int fuzz_callback(void *opaque, int argc, char **argv, char **colnames) {
  (void)argv;
  (void)colnames;
  const uint8_t *data = (const uint8_t *)opaque;
  if (data == NULL) return 0;
  return (argc > 0 && (data[0] & 1)) ? 1 : 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  void *mem = NULL;
  char *sql = NULL;
  char *errmsg = NULL;
  int rc;

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    return 0;
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
      if (Size > 0) {
        fwrite(Data, 1, Size, fp);
      }
      fclose(fp);
    }
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (db == NULL) {
    return 0;
  }

  {
    sqlite3_uint64 alloc_size = 0;
    if (Size >= 8) {
      alloc_size =
          ((sqlite3_uint64)Data[0] << 56) ^
          ((sqlite3_uint64)Data[1] << 48) ^
          ((sqlite3_uint64)Data[2] << 40) ^
          ((sqlite3_uint64)Data[3] << 32) ^
          ((sqlite3_uint64)Data[4] << 24) ^
          ((sqlite3_uint64)Data[5] << 16) ^
          ((sqlite3_uint64)Data[6] << 8)  ^
          ((sqlite3_uint64)Data[7]);
    } else {
      alloc_size = (sqlite3_uint64)Size;
    }
    alloc_size %= (1U << 20);
    if (alloc_size == 0) {
      alloc_size = (sqlite3_uint64)Size;
    }
    mem = sqlite3_malloc64(alloc_size);
  }

  if (mem != NULL) {
    size_t copy_sz = 0;
    if ((sqlite3_uint64)Size < (sqlite3_uint64)(1U << 20)) {
      copy_sz = Size;
    } else {
      copy_sz = (size_t)((1U << 20) - 1);
    }
    if (copy_sz > 0) {
      memcpy(mem, Data, copy_sz);
    }
  }

  {
    unsigned char *dbbuf = (unsigned char *)mem;
    sqlite3_int64 szDb = (sqlite3_int64)(Size > 0 ? Size : 0);
    sqlite3_int64 szBuf = szDb;
    unsigned flags = 0;

    if (dbbuf != NULL) {
#ifdef SQLITE_DESERIALIZE_RESIZEABLE
      if (Size > 0 && (Data[0] & 1)) flags |= SQLITE_DESERIALIZE_RESIZEABLE;
#endif
#ifdef SQLITE_DESERIALIZE_READONLY
      if (Size > 1 && (Data[1] & 1)) flags |= SQLITE_DESERIALIZE_READONLY;
#endif
      rc = sqlite3_deserialize(db, "main", dbbuf, szDb, szBuf, flags);
      if (rc == SQLITE_OK) {
        mem = NULL;
      }
    }
  }

  {
    int op;
    void *arg = NULL;
    sqlite3_int64 i64arg = 0;
    sqlite3_file *fileptr = NULL;
    sqlite3_vfs *vfsptr = NULL;
    const char *zDbName = NULL;

    if (Size > 2) {
      switch (Data[2] % 4) {
        case 0: zDbName = NULL; break;
        case 1: zDbName = "main"; break;
        case 2: zDbName = "temp"; break;
        default: zDbName = "nosuch"; break;
      }
    }

    if (Size > 3) {
      switch (Data[3] % 4) {
#ifdef SQLITE_FCNTL_FILE_POINTER
        case 0:
          op = SQLITE_FCNTL_FILE_POINTER;
          arg = &fileptr;
          break;
#else
        case 0:
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        case 1:
          op = SQLITE_FCNTL_VFS_POINTER;
          arg = &vfsptr;
          break;
#else
        case 1:
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
        case 2:
          op = SQLITE_FCNTL_DATA_VERSION;
          arg = &i64arg;
          break;
#else
        case 2:
#endif
        default:
          op = (Size > 7) ? (int)((unsigned)Data[4] | ((unsigned)Data[5] << 8) |
                                  ((unsigned)Data[6] << 16) | ((unsigned)Data[7] << 24))
                          : 0x7fffffff;
          arg = &i64arg;
          break;
      }
    } else {
#ifdef SQLITE_FCNTL_FILE_POINTER
      op = SQLITE_FCNTL_FILE_POINTER;
      arg = &fileptr;
#else
      op = 0;
      arg = &i64arg;
#endif
    }

    (void)sqlite3_file_control(db, zDbName, op, arg);
  }

  sql = (char *)malloc(Size + 1);
  if (sql != NULL) {
    if (Size > 0) {
      memcpy(sql, Data, Size);
    }
    sql[Size] = '\0';
    (void)sqlite3_exec(
        db,
        sql,
        (Size > 0 && (Data[0] & 2)) ? fuzz_callback : NULL,
        (void *)Data,
        &errmsg);
  }

  if (errmsg != NULL) {
    sqlite3_free(errmsg);
  }
  free(sql);
  if (mem != NULL) {
    sqlite3_free(mem);
  }
  sqlite3_close(db);
  return 0;
}