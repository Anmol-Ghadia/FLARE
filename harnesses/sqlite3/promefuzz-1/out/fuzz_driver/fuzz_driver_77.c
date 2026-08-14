// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v |= ((uint32_t)(*p)[i]) << (8 * i);
  }
  *p += take;
  *n -= take;
  return v;
}

static int pick_flag(uint32_t bits) {
  int flags = 0;
  if (bits & 0x0001) flags |= SQLITE_OPEN_READONLY;
  if (bits & 0x0002) flags |= SQLITE_OPEN_READWRITE;
  if (bits & 0x0004) flags |= SQLITE_OPEN_CREATE;
  if (bits & 0x0008) flags |= SQLITE_OPEN_URI;
  if (bits & 0x0010) flags |= SQLITE_OPEN_MEMORY;
  if (bits & 0x0020) flags |= SQLITE_OPEN_NOMUTEX;
  if (bits & 0x0040) flags |= SQLITE_OPEN_FULLMUTEX;
  if (bits & 0x0080) flags |= SQLITE_OPEN_SHAREDCACHE;
  if (bits & 0x0100) flags |= SQLITE_OPEN_PRIVATECACHE;
#ifdef SQLITE_OPEN_NOFOLLOW
  if (bits & 0x0200) flags |= SQLITE_OPEN_NOFOLLOW;
#endif
#ifdef SQLITE_OPEN_EXRESCODE
  if (bits & 0x0400) flags |= SQLITE_OPEN_EXRESCODE;
#endif
  if ((flags & (SQLITE_OPEN_READONLY | SQLITE_OPEN_READWRITE)) == 0) {
    flags |= SQLITE_OPEN_READWRITE;
  }
  return flags;
}

static void make_utf16le_path(const char *src, unsigned char *dst, size_t dstsz) {
  size_t i = 0;
  if (dstsz < 2) return;
  while (src[i] != '\0' && (i * 2 + 1) < dstsz - 1) {
    dst[i * 2] = (unsigned char)src[i];
    dst[i * 2 + 1] = 0;
    i++;
  }
  dst[i * 2] = 0;
  if (i * 2 + 1 < dstsz) dst[i * 2 + 1] = 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  char utf8name[128];
  memset(utf8name, 0, sizeof(utf8name));
  if (n > 0) {
    size_t copy = n < sizeof(utf8name) - 1 ? n : sizeof(utf8name) - 1;
    memcpy(utf8name, p, copy);
    for (size_t i = 0; i < copy; ++i) {
      if (utf8name[i] == '\0' || utf8name[i] == '/' || utf8name[i] == '\\' ||
          utf8name[i] == ':' || utf8name[i] == '*' || utf8name[i] == '?' ||
          utf8name[i] == '"' || utf8name[i] == '<' || utf8name[i] == '>' ||
          utf8name[i] == '|') {
        utf8name[i] = '_';
      }
    }
  }

  const char *chosen_name;
  switch (n ? (p[0] % 4) : 0) {
    case 0:
      chosen_name = "./dummy_file";
      break;
    case 1:
      chosen_name = "";
      break;
    case 2:
      chosen_name = ":memory:";
      break;
    default:
      chosen_name = utf8name[0] ? utf8name : "./dummy_file";
      break;
  }

  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;

  int flags = pick_flag(read_u32(&p, &n));
  const char *vfs = NULL;
  if (n > 0 && (p[0] & 1)) {
    vfs = "no_such_vfs";
    p++;
    n--;
  }

  int rc1 = sqlite3_open_v2(chosen_name, &db1, flags, vfs);
  if (db1) {
    (void)sqlite3_extended_errcode(db1);
    (void)sqlite3_system_errno(db1);
    (void)sqlite3_get_autocommit(db1);

    int cur = 0, hi = 0;
    int ops[] = {
#ifdef SQLITE_DBSTATUS_LOOKASIDE_USED
      SQLITE_DBSTATUS_LOOKASIDE_USED,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_HIT
      SQLITE_DBSTATUS_LOOKASIDE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
      SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE,
#endif
#ifdef SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
      SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED
      SQLITE_DBSTATUS_CACHE_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_HIT
      SQLITE_DBSTATUS_CACHE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_MISS
      SQLITE_DBSTATUS_CACHE_MISS,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_WRITE
      SQLITE_DBSTATUS_CACHE_WRITE,
#endif
#ifdef SQLITE_DBSTATUS_SCHEMA_USED
      SQLITE_DBSTATUS_SCHEMA_USED,
#endif
#ifdef SQLITE_DBSTATUS_STMT_USED
      SQLITE_DBSTATUS_STMT_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_SPILL
      SQLITE_DBSTATUS_CACHE_SPILL,
#endif
#ifdef SQLITE_DBSTATUS_DEFERRED_FKS
      SQLITE_DBSTATUS_DEFERRED_FKS,
#endif
    };
    size_t ops_count = sizeof(ops) / sizeof(ops[0]);
    for (size_t i = 0; i < ops_count; ++i) {
      int reset = (n > 0) ? (p[0] & 1) : 0;
      if (n > 0) {
        p++;
        n--;
      }
      (void)sqlite3_db_status(db1, ops[i], &cur, &hi, reset);
    }

    (void)sqlite3_exec(db1, "BEGIN", NULL, NULL, NULL);
    (void)sqlite3_get_autocommit(db1);
    (void)sqlite3_exec(db1, "CREATE TABLE IF NOT EXISTS t(x)", NULL, NULL, NULL);
    (void)sqlite3_exec(db1, "INSERT INTO t VALUES(1)", NULL, NULL, NULL);
    (void)sqlite3_exec(db1, "ROLLBACK", NULL, NULL, NULL);
    (void)sqlite3_get_autocommit(db1);

    if (rc1 != SQLITE_OK) {
      (void)sqlite3_extended_errcode(db1);
      (void)sqlite3_system_errno(db1);
    }
  }

  unsigned char utf16name[256];
  memset(utf16name, 0, sizeof(utf16name));
  make_utf16le_path(chosen_name, utf16name, sizeof(utf16name));

  const void *name16 = utf16name;
  if (n > 0 && (p[0] & 1)) {
    name16 = NULL;
  }

  int rc2 = sqlite3_open16(name16, &db2);
  if (db2) {
    (void)sqlite3_extended_errcode(db2);
    (void)sqlite3_system_errno(db2);
    (void)sqlite3_get_autocommit(db2);

    int cur = 0, hi = 0;
#ifdef SQLITE_DBSTATUS_CACHE_USED
    (void)sqlite3_db_status(db2, SQLITE_DBSTATUS_CACHE_USED, &cur, &hi, rc2 & 1);
#endif
#ifdef SQLITE_DBSTATUS_SCHEMA_USED
    (void)sqlite3_db_status(db2, SQLITE_DBSTATUS_SCHEMA_USED, &cur, &hi, (rc2 >> 1) & 1);
#endif

    (void)sqlite3_exec(db2, "BEGIN", NULL, NULL, NULL);
    (void)sqlite3_get_autocommit(db2);
    (void)sqlite3_exec(db2, "COMMIT", NULL, NULL, NULL);
    (void)sqlite3_get_autocommit(db2);

    if (rc2 != SQLITE_OK) {
      (void)sqlite3_extended_errcode(db2);
      (void)sqlite3_system_errno(db2);
    }
  }

  (void)sqlite3_system_errno(NULL);

  if (db2) sqlite3_close(db2);
  if (db1) sqlite3_close(db1);
  return 0;
}