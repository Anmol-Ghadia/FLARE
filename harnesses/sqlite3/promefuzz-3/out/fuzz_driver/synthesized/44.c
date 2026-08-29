// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_serialize at sqlite3.c:36892:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_serialize at sqlite3.c:36892:27 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_backup_init at sqlite3.c:64113:28 in sqlite3.h
// sqlite3_backup_step at sqlite3.c:64293:16 in sqlite3.h
// sqlite3_backup_remaining at sqlite3.c:64601:16 in sqlite3.h
// sqlite3_backup_pagecount at sqlite3.c:64615:16 in sqlite3.h
// sqlite3_backup_finish at sqlite3.c:64547:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg16 at sqlite3.c:154808:24 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
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

static uint32_t rd32(const uint8_t *p) {
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

static void touch_errmsg16(sqlite3 *db) {
  const void *msg16 = sqlite3_errmsg16(db);
  volatile const uint8_t *p = (volatile const uint8_t *)msg16;
  if (p) {
    (void)p[0];
  }
}

int LLVMFuzzerTestOneInput_44(const uint8_t *Data, size_t Size) {
  if (!Data) return 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  uint32_t ctrl = 0;
  if (Size >= 4) ctrl = rd32(Data);

  const char *src_name = (ctrl & 1) ? ":memory:" : "./dummy_file";
  const char *dst_name = (ctrl & 2) ? ":memory:" : "./dummy_file";

  int src_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  int dst_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (ctrl & 4) dst_flags = SQLITE_OPEN_READONLY;

  sqlite3 *src = NULL;
  sqlite3 *dst = NULL;
  sqlite3 *aux = NULL;

  int rc1 = sqlite3_open_v2(src_name, &src, src_flags, NULL);
  int rc2 = sqlite3_open_v2(dst_name, &dst, dst_flags, NULL);
  int rc3 = sqlite3_open_v2(":memory:", &aux, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  if (src && rc1 == SQLITE_OK) {
    exec_sql(src, "PRAGMA journal_mode=WAL;");
    exec_sql(src, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT, c BLOB);");
    exec_sql(src, "BEGIN;");
    if (Size > 4) {
      sqlite3_stmt *st = NULL;
      if (sqlite3_prepare_v2(src, "INSERT INTO t(a,b,c) VALUES(?,?,?);", -1, &st, NULL) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, (int)(ctrl & 0x7fffffff));
        sqlite3_bind_text(st, 2, (const char *)(Data + 4), (int)(Size - 4), SQLITE_STATIC);
        sqlite3_bind_blob(st, 3, (const void *)Data, (int)Size, SQLITE_STATIC);
        sqlite3_step(st);
      }
      sqlite3_finalize(st);
    }
    exec_sql(src, "COMMIT;");
    if (ctrl & 8) {
      exec_sql(src, "ATTACH DATABASE ':memory:' AS aux1;");
      exec_sql(src, "CREATE TABLE IF NOT EXISTS aux1.u(x);");
      exec_sql(src, "INSERT INTO aux1.u VALUES(1);");
    }
  }

  if (dst && rc2 == SQLITE_OK) {
    exec_sql(dst, "CREATE TABLE IF NOT EXISTS preexisting(x);");
    if (ctrl & 16) {
      exec_sql(dst, "BEGIN;");
    }
    if (ctrl & 32) {
      exec_sql(dst, "ATTACH DATABASE ':memory:' AS aux2;");
      exec_sql(dst, "CREATE TABLE IF NOT EXISTS aux2.v(y);");
    }
  }

  if (src) {
    sqlite3_mutex *m = sqlite3_db_mutex(src);
    (void)m;
    const char *fn_main = sqlite3_db_filename(src, "main");
    const char *fn_temp = sqlite3_db_filename(src, "temp");
    const char *fn_bad = sqlite3_db_filename(src, (ctrl & 64) ? "aux1" : "no_such_db");
    volatile const char *sink1 = fn_main;
    volatile const char *sink2 = fn_temp;
    volatile const char *sink3 = fn_bad;
    (void)sink1;
    (void)sink2;
    (void)sink3;
  }

  if (dst) {
    sqlite3_mutex *m = sqlite3_db_mutex(dst);
    (void)m;
    const char *fn_main = sqlite3_db_filename(dst, "main");
    const char *fn_other = sqlite3_db_filename(dst, (ctrl & 128) ? "aux2" : "temp");
    volatile const char *sink1 = fn_main;
    volatile const char *sink2 = fn_other;
    (void)sink1;
    (void)sink2;
  }

  if (src) {
    sqlite3_int64 ser_size = -1;
    unsigned int flags = (ctrl & 256) ? SQLITE_SERIALIZE_NOCOPY : 0;
    unsigned char *ser = sqlite3_serialize(src, (ctrl & 512) ? "temp" : "main", &ser_size, flags);
    if (ser) {
      volatile unsigned char b = ser_size > 0 ? ser[0] : 0;
      (void)b;
      if ((flags & SQLITE_SERIALIZE_NOCOPY) == 0) {
        sqlite3_free(ser);
      }
    } else {
      touch_errmsg16(src);
    }
  }

  if (dst) {
    sqlite3_int64 ser_size = -1;
    unsigned int flags = (ctrl & 1024) ? SQLITE_SERIALIZE_NOCOPY : 0;
    unsigned char *ser = sqlite3_serialize(dst, (ctrl & 2048) ? "temp" : "main", &ser_size, flags);
    if (ser) {
      volatile unsigned char b = ser_size > 0 ? ser[0] : 0;
      (void)b;
      if ((flags & SQLITE_SERIALIZE_NOCOPY) == 0) {
        sqlite3_free(ser);
      }
    } else {
      touch_errmsg16(dst);
    }
  }

  if (ctrl & 4096) {
    if (src) sqlite3_interrupt(src);
    if (dst) sqlite3_interrupt(dst);
    if (aux) sqlite3_interrupt(aux);
  }

  if (src && dst) {
    sqlite3 *backup_src = (ctrl & 8192) ? dst : src;
    sqlite3 *backup_dst = (ctrl & 16384) ? src : dst;
    if (ctrl & 32768) {
      backup_dst = backup_src;
    }

    const char *src_schema = "main";
    const char *dst_schema = "main";
    if (ctrl & 65536) src_schema = "temp";
    if (ctrl & 131072) dst_schema = "temp";
    if (ctrl & 262144) src_schema = "no_such_db";
    if (ctrl & 524288) dst_schema = "no_such_db";

    sqlite3_backup *bak = sqlite3_backup_init(backup_dst, dst_schema, backup_src, src_schema);
    if (bak) {
      int loops = (int)((ctrl >> 20) & 0x0f) + 1;
      int stepv[4];
      stepv[0] = -1;
      stepv[1] = 0;
      stepv[2] = 1;
      stepv[3] = (int)((ctrl >> 8) & 0x7f);

      for (int i = 0; i < loops; i++) {
        int n = stepv[i & 3];
        int rc = sqlite3_backup_step(bak, n);
        int rem = sqlite3_backup_remaining(bak);
        int pc = sqlite3_backup_pagecount(bak);
        volatile int sink = rem ^ pc ^ rc;
        (void)sink;

        if ((ctrl & 1048576) && i == 0 && backup_src) {
          exec_sql(backup_src, "INSERT INTO t(a,b,c) VALUES(123,'mutate',x'00');");
        }

        if (rc == SQLITE_DONE || rc == SQLITE_READONLY || rc == SQLITE_NOMEM ||
            rc == SQLITE_IOERR || rc == SQLITE_LOCKED || rc == SQLITE_BUSY) {
          if (rc != SQLITE_OK && rc != SQLITE_BUSY && rc != SQLITE_LOCKED) {
            touch_errmsg16(backup_dst);
          }
          if (rc == SQLITE_DONE || rc == SQLITE_READONLY || rc == SQLITE_NOMEM ||
              rc == SQLITE_IOERR) {
            break;
          }
        }
      }
      (void)sqlite3_backup_finish(bak);
    } else {
      touch_errmsg16(backup_dst);
    }
  }

  if (dst && (ctrl & 16)) {
    exec_sql(dst, "ROLLBACK;");
  }

  if (aux) sqlite3_close(aux);
  if (src) sqlite3_close(src);
  if (dst) sqlite3_close(dst);
  return 0;
}