// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_backup_remaining at sqlite3.c:64601:16 in sqlite3.h
// sqlite3_backup_pagecount at sqlite3.c:64615:16 in sqlite3.h
// sqlite3_backup_step at sqlite3.c:64293:16 in sqlite3.h
// sqlite3_backup_finish at sqlite3.c:64547:16 in sqlite3.h
// sqlite3_backup_init at sqlite3.c:64113:28 in sqlite3.h
// sqlite3_backup_finish at sqlite3.c:64547:16 in sqlite3.h
// sqlite3_backup_init at sqlite3.c:64113:28 in sqlite3.h
// sqlite3_backup_finish at sqlite3.c:64547:16 in sqlite3.h
// sqlite3_backup_init at sqlite3.c:64113:28 in sqlite3.h
// sqlite3_backup_init at sqlite3.c:64113:28 in sqlite3.h
// sqlite3_backup_remaining at sqlite3.c:64601:16 in sqlite3.h
// sqlite3_backup_pagecount at sqlite3.c:64615:16 in sqlite3.h
// sqlite3_backup_step at sqlite3.c:64293:16 in sqlite3.h
// sqlite3_backup_remaining at sqlite3.c:64601:16 in sqlite3.h
// sqlite3_backup_pagecount at sqlite3.c:64615:16 in sqlite3.h
// sqlite3_backup_finish at sqlite3.c:64547:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static const char *choose_db_name(uint8_t selector) {
  switch (selector % 5) {
    case 0: return "main";
    case 1: return "temp";
    case 2: return "";
    case 3: return "aux";
    default: return "nonexistent";
  }
}

static int choose_checkpoint_mode(uint8_t selector) {
  switch (selector % 4) {
    case 0: return SQLITE_CHECKPOINT_PASSIVE;
    case 1: return SQLITE_CHECKPOINT_FULL;
    case 2: return SQLITE_CHECKPOINT_RESTART;
    default: return SQLITE_CHECKPOINT_TRUNCATE;
  }
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void populate_db(sqlite3 *db, const uint8_t *data, size_t size) {
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT, c BLOB);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS u(x);");
  exec_sql(db, "BEGIN;");
  size_t rows = size > 32 ? 32 : size;
  for (size_t i = 0; i < rows; ++i) {
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db,
                           "INSERT INTO t(a,b,c) VALUES(?1,?2,?3);",
                           -1, &stmt, NULL) == SQLITE_OK) {
      sqlite3_bind_int(stmt, 1, (int)data[i]);
      sqlite3_bind_text(stmt, 2, (const char *)data, (int)(size > 64 ? 64 : size),
                        SQLITE_TRANSIENT);
      sqlite3_bind_blob(stmt, 3, data, (int)(size > 128 ? 128 : size),
                        SQLITE_TRANSIENT);
      sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
  }
  exec_sql(db, "COMMIT;");
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *src = NULL;
  sqlite3 *dst = NULL;
  sqlite3 *same = NULL;

  sqlite3_open(":memory:", &src);
  sqlite3_open(":memory:", &dst);
  sqlite3_open("./dummy_file", &same);

  if (!src || !dst || !same) {
    if (src) sqlite3_close(src);
    if (dst) sqlite3_close(dst);
    if (same) sqlite3_close(same);
    return 0;
  }

  exec_sql(src, "PRAGMA journal_mode=WAL;");
  exec_sql(dst, "PRAGMA journal_mode=WAL;");
  exec_sql(same, "PRAGMA journal_mode=WAL;");
  exec_sql(src, "ATTACH DATABASE './dummy_file' AS aux;");
  exec_sql(dst, "ATTACH DATABASE ':memory:' AS aux;");

  populate_db(src, Data, Size);

  if (remaining > 0 && (p[0] & 1)) {
    exec_sql(src, "BEGIN;");
    exec_sql(src, "INSERT INTO t(a,b,c) VALUES(123,'lock',x'00');");
  }

  {
    int pnLog = -2, pnCkpt = -2;
    const char *zDb = NULL;
    if (remaining > 0) {
      uint8_t sel = *p++;
      remaining--;
      if (sel % 3 == 1) zDb = choose_db_name(sel);
      else if (sel % 3 == 2) zDb = NULL;
      else zDb = "";
      sqlite3_wal_checkpoint_v2(src, zDb, choose_checkpoint_mode(sel), &pnLog, &pnCkpt);
      sqlite3_wal_checkpoint_v2(dst, zDb, choose_checkpoint_mode(sel >> 2), NULL, NULL);
    } else {
      sqlite3_wal_checkpoint_v2(src, NULL, SQLITE_CHECKPOINT_PASSIVE, &pnLog, &pnCkpt);
    }
  }

  sqlite3_backup_remaining(NULL);
  sqlite3_backup_pagecount(NULL);
  sqlite3_backup_step(NULL, 1);
  sqlite3_backup_finish(NULL);

  {
    sqlite3_backup *bad1 = sqlite3_backup_init(dst, "main", dst, "main");
    if (bad1) sqlite3_backup_finish(bad1);
  }

  {
    sqlite3_backup *bad2 = sqlite3_backup_init(dst, "nonexistent", src, "main");
    if (bad2) sqlite3_backup_finish(bad2);
  }

  {
    const char *destName = choose_db_name((uint8_t)(remaining ? *p : 0));
    if (remaining) { p++; remaining--; }
    const char *srcName = choose_db_name((uint8_t)(remaining ? *p : 0));
    if (remaining) { p++; remaining--; }

    sqlite3_backup *bk = sqlite3_backup_init(dst, destName, src, srcName);
    if (!bk) {
      bk = sqlite3_backup_init(dst, "main", src, "main");
    }

    if (bk) {
      int loops = 0;
      int rc = SQLITE_OK;

      (void)sqlite3_backup_remaining(bk);
      (void)sqlite3_backup_pagecount(bk);

      while (loops < 16) {
        int nPage;
        if (remaining > 0) {
          nPage = read_int(&p, &remaining);
          if (nPage > 32) nPage = 32;
          if (nPage < -32) nPage = -32;
        } else {
          nPage = (loops % 3 == 0) ? -1 : (loops + 1);
        }

        rc = sqlite3_backup_step(bk, nPage);
        (void)sqlite3_backup_remaining(bk);
        (void)sqlite3_backup_pagecount(bk);

        if (remaining > 0 && (*p & 1)) {
          exec_sql(src, "INSERT INTO t(a,b,c) VALUES(456,'mid',x'01');");
        }
        if (remaining > 0) {
          p++;
          remaining--;
        }

        if (rc == SQLITE_DONE) break;
        if (rc != SQLITE_OK && rc != SQLITE_BUSY && rc != SQLITE_LOCKED) break;
        loops++;
      }

      (void)sqlite3_backup_finish(bk);
    }
  }

  exec_sql(src, "ROLLBACK;");
  exec_sql(dst, "ROLLBACK;");

  {
    int pnLog = -1, pnCkpt = -1;
    sqlite3_wal_checkpoint_v2(src, "main", SQLITE_CHECKPOINT_PASSIVE, &pnLog, &pnCkpt);
    sqlite3_wal_checkpoint_v2(src, "aux", SQLITE_CHECKPOINT_FULL, &pnLog, &pnCkpt);
    sqlite3_wal_checkpoint_v2(src, "nonexistent", SQLITE_CHECKPOINT_RESTART, &pnLog, &pnCkpt);
    sqlite3_wal_checkpoint_v2(dst, NULL, SQLITE_CHECKPOINT_TRUNCATE, NULL, NULL);
  }

  sqlite3_close(same);
  sqlite3_close(dst);
  sqlite3_close(src);
  return 0;
}