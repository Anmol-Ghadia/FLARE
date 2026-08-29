// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
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

static int consume_u8(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static uint32_t consume_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  for (int i = 0; i < 4; i++) {
    v = (v << 8) | (uint32_t)consume_u8(p, n);
  }
  return v;
}

static int choose_int(const uint8_t **p, size_t *n, int limit) {
  if (limit <= 0) return 0;
  return (int)(consume_u32(p, n) % (uint32_t)limit);
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static char *make_cstring(const uint8_t *Data, size_t Size) {
  char *s = (char *)malloc(Size + 1);
  if (!s) return NULL;
  if (Size) memcpy(s, Data, Size);
  s[Size] = '\0';
  return s;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
}

static void exercise_stmt(sqlite3 *db, sqlite3_stmt *stmt, const uint8_t **p, size_t *n) {
  if (!stmt) return;

  int param_count = sqlite3_bind_parameter_count(stmt);
  int bind_loops = 1 + choose_int(p, n, 4);

  for (int i = 0; i < bind_loops; i++) {
    const char *name = NULL;
    int selector = choose_int(p, n, 6);
    switch (selector) {
      case 0: name = ":a"; break;
      case 1: name = "@b"; break;
      case 2: name = "$c"; break;
      case 3: name = "?1"; break;
      case 4: name = ":missing"; break;
      default: name = ""; break;
    }

    int idx = sqlite3_bind_parameter_index(stmt, name);
    if (idx == 0 && param_count > 0) {
      idx = 1 + choose_int(p, n, param_count);
    }

    if (idx > 0) {
      int bind_type = choose_int(p, n, 5);
      switch (bind_type) {
        case 0:
          sqlite3_bind_null(stmt, idx);
          break;
        case 1:
          sqlite3_bind_int(stmt, idx, (int)consume_u32(p, n));
          break;
        case 2:
          sqlite3_bind_int64(stmt, idx, (sqlite3_int64)(int32_t)consume_u32(p, n));
          break;
        case 3:
          sqlite3_bind_double(stmt, idx, (double)(int32_t)consume_u32(p, n) / 17.0);
          break;
        default: {
          int len = choose_int(p, n, 32);
          char buf[32];
          for (int j = 0; j < len; j++) buf[j] = (char)consume_u8(p, n);
          sqlite3_bind_text(stmt, idx, buf, len, SQLITE_TRANSIENT);
          break;
        }
      }
    }
  }

  int step_loops = 1 + choose_int(p, n, 4);
  for (int s = 0; s < step_loops; s++) {
    int rc = sqlite3_step(stmt);
    int cols = sqlite3_column_count(stmt);

    for (int i = -2; i < cols + 3; i++) {
      const char *decl = sqlite3_column_decltype(stmt, i);
      const char *dbname = sqlite3_column_database_name(stmt, i);
      (void)decl;
      (void)dbname;
    }

    if (rc == SQLITE_ROW) {
      continue;
    } else {
      break;
    }
  }

  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);

  for (int i = -1; i < sqlite3_column_count(stmt) + 2; i++) {
    const char *decl = sqlite3_column_decltype(stmt, i);
    const char *dbname = sqlite3_column_database_name(stmt, i);
    (void)decl;
    (void)dbname;
  }

  sqlite3_stmt *iter = NULL;
  int guard = 0;
  while ((iter = sqlite3_next_stmt(db, iter)) != NULL && guard++ < 64) {
    sqlite3_bind_parameter_index(iter, ":a");
    if (sqlite3_column_count(iter) > 0) {
      sqlite3_column_decltype(iter, 0);
      sqlite3_column_database_name(iter, 0);
    }
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  write_dummy_file(Data, Size);

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA temp_store=MEMORY;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(c1 VARIANT, c2 TEXT, c3 INTEGER);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x REAL, y BLOB);");
  exec_sql(db, "INSERT INTO t1(c1,c2,c3) VALUES (1,'alpha',10), (2,'beta',20), (NULL,'gamma',30);");
  exec_sql(db, "INSERT INTO t2(x,y) VALUES (3.14, X'0102'), (2.71, X'FF');");

  char *sql = make_cstring(p, n);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }

  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  sqlite3_stmt *stmt3 = NULL;
  const char *tail1 = NULL;
  const char *tail2 = NULL;
  const char *tail3 = NULL;

  sqlite3_prepare(db, sql, -1, &stmt1, &tail1);

  unsigned int flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
  if (consume_u8(&p, &n) & 1) flags |= SQLITE_PREPARE_PERSISTENT;
#endif
#ifdef SQLITE_PREPARE_NORMALIZE
  if (consume_u8(&p, &n) & 1) flags |= SQLITE_PREPARE_NORMALIZE;
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
  if (consume_u8(&p, &n) & 1) flags |= SQLITE_PREPARE_NO_VTAB;
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
  if (consume_u8(&p, &n) & 1) flags |= SQLITE_PREPARE_DONT_LOG;
#endif

  sqlite3_prepare_v3(db, sql, -1, flags, &stmt2, &tail2);

  {
    const char *fixed_sqls[] = {
      "SELECT c1 + 1, c1, c2, c3 FROM t1 WHERE c2 = :a OR c3 = @b;",
      "SELECT main.t1.c1, main.t1.c2 FROM t1;",
      "SELECT x, y, x+1 FROM t2 WHERE x > $c;",
      "SELECT ?1, c1 FROM t1;",
      "SELECT (SELECT c2 FROM t1 LIMIT 1), c3 FROM t1;"
    };
    const char *chosen = fixed_sqls[choose_int(&p, &n, (int)(sizeof(fixed_sqls) / sizeof(fixed_sqls[0])))];
    sqlite3_prepare_v3(db, chosen, -1, flags, &stmt3, &tail3);
  }

  exercise_stmt(db, stmt1, &p, &n);
  exercise_stmt(db, stmt2, &p, &n);
  exercise_stmt(db, stmt3, &p, &n);

  sqlite3_stmt *iter = NULL;
  int guard = 0;
  while ((iter = sqlite3_next_stmt(db, iter)) != NULL && guard++ < 64) {
    sqlite3_bind_parameter_index(iter, ":a");
    sqlite3_bind_parameter_index(iter, "@b");
    sqlite3_bind_parameter_index(iter, "$c");
    sqlite3_bind_parameter_index(iter, "?1");

    int cols = sqlite3_column_count(iter);
    for (int i = -1; i < cols + 2; i++) {
      sqlite3_column_decltype(iter, i);
      sqlite3_column_database_name(iter, i);
    }
  }

  sqlite3_finalize(stmt1);
  sqlite3_finalize(stmt2);
  sqlite3_finalize(stmt3);
  free(sql);
  sqlite3_close(db);
  return 0;
}