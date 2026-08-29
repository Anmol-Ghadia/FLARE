// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_serialize at sqlite3.c:36892:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  (void)argv;
  (void)colv;
  unsigned char *state = (unsigned char *)ctx;
  if (state) {
    state[0] ^= (unsigned char)argc;
    if (state[1] & 1) {
      return argc > 8;
    }
  }
  return 0;
}

static int pick_int(const uint8_t *data, size_t size, size_t *off) {
  int v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < size; i++, (*off)++) {
    v = (v << 8) | data[*off];
  }
  return v;
}

static char *make_cstring(const uint8_t *data, size_t len) {
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, data, len);
  s[len] = '\0';
  return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  int rc;
  size_t off = 0;
  char *sql = NULL;
  char *errmsg = NULL;
  unsigned char cb_state[2] = {0, 0};
  const char *filename;
  FILE *fp;

  if (!Data) return 0;

  if (Size > 0) {
    switch (Data[off++] % 4) {
      case 0:
        filename = ":memory:";
        break;
      case 1:
        filename = "";
        break;
      case 2:
        filename = "./dummy_file";
        fp = fopen(filename, "wb");
        if (fp) {
          if (off < Size) fwrite(Data + off, 1, Size - off, fp);
          fclose(fp);
        }
        break;
      default:
        filename = "file:./dummy_file?mode=rwc&cache=private";
        fp = fopen("./dummy_file", "wb");
        if (fp) {
          if (off < Size) fwrite(Data + off, 1, Size - off, fp);
          fclose(fp);
        }
        break;
    }
  } else {
    filename = ":memory:";
  }

  rc = sqlite3_open(filename, &db);
  if (db == NULL) {
    return 0;
  }

  (void)rc;
  (void)sqlite3_errmsg(db);

#ifndef SQLITE_OMIT_DESERIALIZE
  if (off < Size) {
    size_t remaining = Size - off;
    sqlite3_int64 n = (sqlite3_int64)remaining;
    sqlite3_int64 m = n;
    unsigned int flags = 0;
    unsigned char *buf = NULL;

    if (off < Size) {
      unsigned int sel = Data[off++] % 4;
      if (sel & 2) flags |= SQLITE_DESERIALIZE_READONLY;
      if (sel & 1) flags |= SQLITE_DESERIALIZE_RESIZEABLE;
    }

    remaining = (off <= Size) ? (Size - off) : 0;
    n = (sqlite3_int64)remaining;
    m = n;

    buf = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)(m > 0 ? m : 1));
    if (buf) {
      if (remaining > 0) memcpy(buf, Data + off, remaining);
      rc = sqlite3_deserialize(
          db,
          "main",
          buf,
          n,
          m,
          flags
      );
      if (rc != SQLITE_OK) {
        sqlite3_free(buf);
      }
      (void)sqlite3_errmsg(db);
    }
  }
#endif

  {
    const char *schemas[] = { NULL, "main", "temp", "no_such_db" };
    int ops[] = {
#ifdef SQLITE_FCNTL_FILE_POINTER
      SQLITE_FCNTL_FILE_POINTER,
#endif
#ifdef SQLITE_FCNTL_JOURNAL_POINTER
      SQLITE_FCNTL_JOURNAL_POINTER,
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
      SQLITE_FCNTL_VFS_POINTER,
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
      SQLITE_FCNTL_DATA_VERSION,
#endif
      -1
    };
    int which_schema = 0;
    int which_op = 0;
    void *ptr_out = NULL;
    int int_out = 0;
    void *arg = &ptr_out;

    if (off < Size) which_schema = Data[off++] % 4;
    if (off < Size) which_op = Data[off++] % (int)(sizeof(ops) / sizeof(ops[0]));

    if (ops[which_op] == -1) {
      int_out = pick_int(Data, Size, &off);
      arg = &int_out;
      rc = sqlite3_file_control(db, schemas[which_schema], int_out, arg);
    } else {
      if (ops[which_op] == SQLITE_FCNTL_DATA_VERSION) {
        arg = &int_out;
      } else {
        arg = &ptr_out;
      }
      rc = sqlite3_file_control(db, schemas[which_schema], ops[which_op], arg);
    }
    (void)rc;
    (void)sqlite3_errmsg(db);
  }

#ifndef SQLITE_OMIT_DESERIALIZE
  {
    sqlite3_int64 out_size = 0;
    unsigned int sflags = 0;
    const char *schema = "main";
    unsigned char *ser;

    if (off < Size && (Data[off++] & 1)) schema = "temp";
#ifdef SQLITE_SERIALIZE_NOCOPY
    if (off < Size && (Data[off++] & 1)) sflags |= SQLITE_SERIALIZE_NOCOPY;
#endif

    ser = sqlite3_serialize(db, schema, &out_size, sflags);
    if (ser && !(sflags & SQLITE_SERIALIZE_NOCOPY)) {
      sqlite3_free(ser);
    }
  }
#endif

  {
    size_t sql_len = (off <= Size) ? (Size - off) : 0;
    sql = make_cstring(Data + off, sql_len);
    if (!sql) {
      sqlite3_close(db);
      return 0;
    }

    cb_state[0] = (Size > 0) ? Data[0] : 0;
    cb_state[1] = (Size > 1) ? Data[1] : 0;

    switch ((Size > 2) ? (Data[2] % 4) : 0) {
      case 0:
        rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        break;
      case 1:
        rc = sqlite3_exec(db, sql, fuzz_callback, cb_state, &errmsg);
        break;
      case 2:
        rc = sqlite3_exec(db, "", fuzz_callback, cb_state, &errmsg);
        break;
      default:
        rc = sqlite3_exec(db, "BEGIN;ROLLBACK;", NULL, NULL, &errmsg);
        break;
    }

    (void)rc;
    (void)sqlite3_errmsg(db);

    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    if (sql[0] != '\0') {
      rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
      (void)rc;
      (void)sqlite3_errmsg(db);
      if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
      }
    }
  }

  free(sql);
  sqlite3_close(db);
  return 0;
}