#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static int fuzz_exec_callback(void *ctx, int argc, char **argv, char **colnames) {
  const uint8_t *data = (const uint8_t *)ctx;
  int ret = 0;
  if (data && argc >= 0) {
    ret = data[0] & 1;
  }
  (void)argv;
  (void)colnames;
  return ret;
}

static int pick_from_data(const uint8_t *Data, size_t Size, size_t *off, int fallback) {
  if (*off < Size) {
    return (int)Data[(*off)++];
  }
  return fallback;
}

static char *make_nul_terminated_copy(const uint8_t *Data, size_t Size) {
  char *buf = (char *)malloc(Size + 1);
  if (!buf) {
    return NULL;
  }
  if (Size > 0) {
    memcpy(buf, Data, Size);
  }
  buf[Size] = '\0';
  return buf;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  char *errmsg = NULL;
  char *mp1 = NULL;
  char *mp2 = NULL;
  char *sqlbuf = NULL;
  int cur = 0, hi = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlbuf = make_nul_terminated_copy(Data, Size);
  if (!sqlbuf) {
    return 0;
  }

  {
    int cfg_choice = pick_from_data(Data, Size, &off, 0) % 4;
    switch (cfg_choice) {
      case 0:
        sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        break;
      case 1:
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        break;
      case 2:
        sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        break;
      default:
        sqlite3_config(SQLITE_CONFIG_URI, pick_from_data(Data, Size, &off, 0) & 1);
        break;
    }
  }

  {
    const char *name1;
    int name_choice = pick_from_data(Data, Size, &off, 0) % 3;
    if (name_choice == 0) {
      name1 = ":memory:";
    } else if (name_choice == 1) {
      name1 = "./dummy_file";
    } else {
      name1 = "";
    }
    sqlite3_open(name1, &db1);
  }

  if (db1) {
    (void)sqlite3_errmsg(db1);

    {
      const char *sql;
      int cb_mode = pick_from_data(Data, Size, &off, 0) % 3;
      int sql_choice = pick_from_data(Data, Size, &off, 0) % 4;
      if (sql_choice == 0) {
        sql = "CREATE TABLE IF NOT EXISTS t(a,b); INSERT INTO t VALUES(1,'x'); SELECT * FROM t;";
      } else if (sql_choice == 1) {
        sql = "BEGIN; CREATE TABLE IF NOT EXISTS t2(x); INSERT INTO t2 VALUES(random()); COMMIT;";
      } else if (sql_choice == 2) {
        sql = sqlbuf;
      } else {
        sql = "SELECT sqlite_version();";
      }

      sqlite3_exec(db1, sql,
                   cb_mode == 0 ? NULL : fuzz_exec_callback,
                   (void *)Data, &errmsg);
    }

    sqlite3_free(errmsg);
    errmsg = NULL;

    sqlite3_db_status(db1, SQLITE_DBSTATUS_LOOKASIDE_USED, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_LOOKASIDE_HIT, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_CACHE_USED, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_SCHEMA_USED, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_STMT_USED, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_CACHE_HIT, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_CACHE_MISS, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
    sqlite3_db_status(db1, SQLITE_DBSTATUS_CACHE_WRITE, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);

    sqlite3_close(db1);
    db1 = NULL;
  }

  sqlite3_status(SQLITE_STATUS_MEMORY_USED, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
  sqlite3_status(SQLITE_STATUS_PAGECACHE_USED, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
  sqlite3_status(SQLITE_STATUS_PAGECACHE_OVERFLOW, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
  sqlite3_status(SQLITE_STATUS_MALLOC_SIZE, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);
  sqlite3_status(SQLITE_STATUS_MALLOC_COUNT, &cur, &hi, pick_from_data(Data, Size, &off, 0) & 1);

  mp1 = sqlite3_mprintf("%.*s", (int)Size, (const char *)Data);
  sqlite3_free(mp1);
  mp1 = NULL;

  mp2 = sqlite3_mprintf("file:%.*s?mode=memory&cache=shared", (int)Size, (const char *)Data);
  sqlite3_free(mp2);
  mp2 = NULL;

  {
    const char *name2;
    int name_choice = pick_from_data(Data, Size, &off, 0) % 3;
    if (name_choice == 0) {
      name2 = ":memory:";
    } else if (name_choice == 1) {
      name2 = "./dummy_file";
    } else {
      name2 = "";
    }
    sqlite3_open(name2, &db2);
  }

  if (db2) {
    (void)sqlite3_errmsg(db2);

    sqlite3_exec(db2, "CREATE TABLE IF NOT EXISTS x(y);", NULL, NULL, &errmsg);
    sqlite3_free(errmsg);
    errmsg = NULL;

    sqlite3_exec(db2, "INSERT INTO x(y) VALUES(1),(2),(3); SELECT * FROM x;", fuzz_exec_callback, (void *)Data, &errmsg);
    sqlite3_free(errmsg);
    errmsg = NULL;

    {
      const char *sql3;
      int sql_choice = pick_from_data(Data, Size, &off, 0) % 3;
      if (sql_choice == 0) {
        sql3 = sqlbuf;
      } else if (sql_choice == 1) {
        sql3 = "PRAGMA integrity_check;";
      } else {
        sql3 = "SELECT quote(y) FROM x;";
      }
      sqlite3_exec(db2, sql3, fuzz_exec_callback, (void *)Data, &errmsg);
    }

    sqlite3_free(errmsg);
    errmsg = NULL;

    sqlite3_close(db2);
    db2 = NULL;
  }

  free(sqlbuf);
  return 0;
}