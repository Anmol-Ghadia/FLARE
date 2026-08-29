// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  (void)argv;
  (void)colv;
  if (ctx != NULL && argc >= 0) {
    const uint8_t *data = (const uint8_t *)ctx;
    return (int)(data[0] & 1U);
  }
  return 0;
}

int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  unsigned char *buf = NULL;
  char *errmsg = NULL;
  FILE *fp = NULL;
  int rc;
  sqlite3_int64 szDb = 0;
  sqlite3_int64 szBuf = 0;
  unsigned flags = 0;
  const char *zDbName = "main";
  size_t alloc_sz = 1;
  size_t db_img_len = 0;
  int use_callback = 0;

  int data_version = 0;
  sqlite3_file *file_ptr = NULL;
  sqlite3_file *journal_ptr = NULL;
  sqlite3_vfs *vfs_ptr = NULL;
  int lockstate = 0;
  void *pArg = &data_version;
  int op = SQLITE_FCNTL_DATA_VERSION;

  char *sql_buf = NULL;
  const char *sql = "";

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    return 0;
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (db == NULL) {
    return 0;
  }

  if (Size > 0) {
    alloc_sz = Size;
  }

  buf = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)alloc_sz);
  if (buf != NULL) {
    if (Size > 0) {
      memcpy(buf, Data, Size);
      db_img_len = Size;
    } else {
      buf[0] = 0;
      db_img_len = 0;
    }

    szDb = (sqlite3_int64)db_img_len;
    szBuf = (sqlite3_int64)alloc_sz;

    switch (Size > 0 ? (Data[0] & 3U) : 0U) {
      case 0:
        flags = 0;
        break;
      case 1:
        flags = SQLITE_DESERIALIZE_FREEONCLOSE;
        break;
      case 2:
        flags = SQLITE_DESERIALIZE_RESIZEABLE;
        break;
      default:
        flags = SQLITE_DESERIALIZE_READONLY;
        break;
    }

    rc = sqlite3_deserialize(db, "main", buf, szDb, szBuf, flags);

    if ((flags & SQLITE_DESERIALIZE_FREEONCLOSE) != 0) {
      buf = NULL;
    } else if (rc == SQLITE_OK) {
      if (sqlite3_serialize != NULL) {
        /* no-op to avoid warnings in some environments */
      }
      rc = sqlite3_deserialize(db, "main", NULL, 0, 0, 0);
      sqlite3_free(buf);
      buf = NULL;
    } else {
      sqlite3_free(buf);
      buf = NULL;
    }
  }

  if (Size > 1) {
    switch (Data[1] % 5U) {
      case 0:
        zDbName = "main";
        op = SQLITE_FCNTL_DATA_VERSION;
        pArg = &data_version;
        break;
      case 1:
        zDbName = NULL;
        op = SQLITE_FCNTL_FILE_POINTER;
        pArg = &file_ptr;
        break;
      case 2:
        zDbName = "temp";
        op = SQLITE_FCNTL_VFS_POINTER;
        pArg = &vfs_ptr;
        break;
      case 3:
        zDbName = "nosuchdb";
        op = SQLITE_FCNTL_JOURNAL_POINTER;
        pArg = &journal_ptr;
        break;
      default:
        zDbName = "main";
        op = SQLITE_FCNTL_LOCKSTATE;
        pArg = &lockstate;
        break;
    }
  }

  (void)sqlite3_file_control(db, zDbName, op, pArg);

  if (Size > 2) {
    size_t sql_len = Size - 2;
    sql_buf = (char *)sqlite3_malloc64((sqlite3_uint64)sql_len + 1U);
    if (sql_buf != NULL) {
      memcpy(sql_buf, Data + 2, sql_len);
      sql_buf[sql_len] = '\0';
      sql = sql_buf;
    }
    use_callback = (int)(Data[1] & 1U);
  }

  if (use_callback) {
    (void)sqlite3_exec(db, sql, fuzz_callback, (void *)Data, &errmsg);
  } else {
    (void)sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  }

  if (errmsg != NULL) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  if (sql_buf != NULL) {
    sqlite3_free(sql_buf);
    sql_buf = NULL;
  }

  sqlite3_close(db);
  return 0;
}