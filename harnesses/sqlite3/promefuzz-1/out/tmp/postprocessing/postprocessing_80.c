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

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; i++, (*Off)++) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static uint64_t read_u64(const uint8_t *Data, size_t Size, size_t *Off) {
  uint64_t v = 0;
  size_t i;
  for (i = 0; i < 8 && *Off < Size; i++, (*Off)++) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static int pick_flag(int bit) {
  return bit ? 1 : 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3 *filedb = NULL;
  unsigned char *buf1 = NULL;
  void *mem1 = NULL;
  void *mem2 = NULL;
  void *mem3 = NULL;
  int rc;
  int open_flags;
  uint32_t ctrl1, ctrl2, ctrl3;
  uint64_t bigsz;
  int heap_limit;
  sqlite3_int64 szDb, szBuf;
  unsigned dflags = 0;
  const char *schema = "main";
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  ctrl1 = read_u32(Data, Size, &off);
  ctrl2 = read_u32(Data, Size, &off);
  ctrl3 = read_u32(Data, Size, &off);
  bigsz = read_u64(Data, Size, &off);

  heap_limit = (int)(ctrl1 & 0x7fffffff);
  if (ctrl1 & 1U) heap_limit = -heap_limit;
  sqlite3_soft_heap_limit(heap_limit);

  mem1 = sqlite3_malloc64((sqlite3_uint64)(ctrl2 % 4096));
  mem2 = sqlite3_realloc(NULL, (int)(ctrl3 % 4096));
  mem3 = sqlite3_realloc64(NULL, (sqlite3_uint64)(bigsz % 4096));

  mem1 = sqlite3_realloc(mem1, (int)(ctrl1 % 8192));
  mem2 = sqlite3_realloc64(mem2, (sqlite3_uint64)(ctrl2 % 8192));
  mem3 = sqlite3_realloc(mem3, (int)(ctrl3 % 8192));

  open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (ctrl1 & 0x100) open_flags |= SQLITE_OPEN_URI;
  if (ctrl1 & 0x200) open_flags |= SQLITE_OPEN_MEMORY;

  rc = sqlite3_open_v2(":memory:", &db, open_flags, NULL);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    sqlite3_free(mem1);
    sqlite3_free(mem2);
    sqlite3_free(mem3);
    return 0;
  }

  sqlite3_open_v2("./dummy_file", &filedb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (filedb) {
    sqlite3_exec(filedb, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
    sqlite3_exec(filedb, "INSERT INTO t VALUES(1);", NULL, NULL, NULL);
    sqlite3_db_release_memory(filedb);
    sqlite3_close(filedb);
    filedb = NULL;
  }

  sqlite3_db_release_memory(db);

  if (ctrl2 & 1U) schema = "main";
  else if (ctrl2 & 2U) schema = "temp";
  else schema = "aux";

  szDb = (sqlite3_int64)((Size > off) ? (Size - off) : 0);
  szBuf = szDb;

  if (pick_flag(ctrl3 & 1U)) dflags |= SQLITE_DESERIALIZE_FREEONCLOSE;
  if (pick_flag(ctrl3 & 2U)) dflags |= SQLITE_DESERIALIZE_RESIZEABLE;
  if (pick_flag(ctrl3 & 4U)) dflags |= SQLITE_DESERIALIZE_READONLY;

  if (ctrl3 & 8U) {
    szBuf += (sqlite3_int64)(ctrl1 % 1024);
  }
  if (szBuf < szDb) szBuf = szDb;

  if (dflags & SQLITE_DESERIALIZE_FREEONCLOSE) {
    buf1 = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)szBuf);
    if (buf1) {
      if (szDb > 0 && Size > off) {
        memcpy(buf1, Data + off, (size_t)szDb);
      }
      if (szBuf > szDb) {
        memset(buf1 + (size_t)szDb, 0, (size_t)(szBuf - szDb));
      }
      sqlite3_deserialize(db, schema, buf1, szDb, szBuf, dflags);
      buf1 = NULL;
    }
  } else {
    buf1 = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)szBuf);
    if (buf1) {
      if (szDb > 0 && Size > off) {
        memcpy(buf1, Data + off, (size_t)szDb);
      }
      if (szBuf > szDb) {
        memset(buf1 + (size_t)szDb, 0, (size_t)(szBuf - szDb));
      }
      sqlite3_deserialize(db, schema, buf1, szDb, szBuf, dflags);
      sqlite3_free(buf1);
      buf1 = NULL;
    }
  }

  sqlite3_db_release_memory(db);

  if (ctrl1 & 0x400) {
    unsigned char *buf2;
    sqlite3_int64 sz2 = (sqlite3_int64)(ctrl2 % 256);
    sqlite3_int64 cap2 = sz2 + (sqlite3_int64)(ctrl3 % 256);
    unsigned flags2 = 0;
    if (ctrl1 & 0x800) flags2 |= SQLITE_DESERIALIZE_READONLY;
    if (ctrl1 & 0x1000) flags2 |= SQLITE_DESERIALIZE_RESIZEABLE;
    if (cap2 < sz2) cap2 = sz2;
    buf2 = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)cap2);
    if (buf2) {
      size_t copy_n = (size_t)sz2;
      if (copy_n > Size) copy_n = Size;
      if (copy_n) memcpy(buf2, Data, copy_n);
      if ((sqlite3_int64)copy_n < cap2) {
        memset(buf2 + copy_n, 0, (size_t)(cap2 - (sqlite3_int64)copy_n));
      }
      sqlite3_deserialize(db, "main", buf2, sz2, cap2, flags2);
      sqlite3_free(buf2);
    }
  }

  sqlite3_close(db);
  sqlite3_free(mem1);
  sqlite3_free(mem2);
  sqlite3_free(mem3);
  return 0;
}