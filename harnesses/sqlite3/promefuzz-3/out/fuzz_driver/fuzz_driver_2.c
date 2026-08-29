// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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

static int consume_u8(const uint8_t **data, size_t *size){
  if(*size == 0) return 0;
  int v = **data;
  (*data)++;
  (*size)--;
  return v;
}

static int consume_bool(const uint8_t **data, size_t *size){
  return consume_u8(data, size) & 1;
}

static int consume_int(const uint8_t **data, size_t *size){
  int v = 0;
  size_t n = *size < 4 ? *size : 4;
  for(size_t i = 0; i < n; i++){
    v = (v << 8) | **data;
    (*data)++;
    (*size)--;
  }
  return v;
}

static char *make_cstring(const uint8_t *data, size_t size){
  char *s = (char *)malloc(size + 1);
  if(!s) return NULL;
  memcpy(s, data, size);
  s[size] = '\0';
  return s;
}

static void write_dummy_file(const uint8_t *data, size_t size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(!fp) return;
  if(size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t remaining = Size;

  write_dummy_file(Data, Size);

  (void)sqlite3_shutdown();
  {
    int cfg_choice = consume_u8(&p, &remaining) % 4;
    switch(cfg_choice){
      case 0:
        (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        break;
      case 1:
        (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        break;
      case 2:
        (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        break;
      default:
        (void)sqlite3_config(SQLITE_CONFIG_URI, consume_bool(&p, &remaining));
        break;
    }
  }

  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  char *errmsg = NULL;
  int cur = 0, hiwtr = 0;

  const char *open_name1;
  switch(consume_u8(&p, &remaining) % 4){
    case 0: open_name1 = ":memory:"; break;
    case 1: open_name1 = ""; break;
    case 2: open_name1 = "./dummy_file"; break;
    default: open_name1 = "file:dummy_file?mode=memory&cache=shared"; break;
  }

  (void)sqlite3_open(open_name1, &db1);
  (void)sqlite3_errmsg(db1);

  char *sql1 = make_cstring(p, remaining);
  if(!sql1) sql1 = make_cstring((const uint8_t *)"", 0);
  (void)sqlite3_exec(db1, sql1 ? sql1 : "", NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
  errmsg = NULL;
  free(sql1);

  {
    static const int db_ops[10] = {
      SQLITE_DBSTATUS_LOOKASIDE_USED,
      SQLITE_DBSTATUS_LOOKASIDE_HIT,
      SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE,
      SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL,
      SQLITE_DBSTATUS_CACHE_USED,
      SQLITE_DBSTATUS_SCHEMA_USED,
      SQLITE_DBSTATUS_STMT_USED,
      SQLITE_DBSTATUS_CACHE_HIT,
      SQLITE_DBSTATUS_CACHE_MISS,
      SQLITE_DBSTATUS_CACHE_WRITE
    };

    (void)sqlite3_db_status(db1, db_ops[0], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[1], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[2], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[3], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[4], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[5], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[6], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[7], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[8], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_db_status(db1, db_ops[9], &cur, &hiwtr, consume_bool(&p, &remaining));
  }

  (void)sqlite3_close(db1);
  db1 = NULL;

  {
    static const int status_ops[5] = {
      SQLITE_STATUS_MEMORY_USED,
      SQLITE_STATUS_PAGECACHE_USED,
      SQLITE_STATUS_PAGECACHE_OVERFLOW,
      SQLITE_STATUS_MALLOC_SIZE,
      SQLITE_STATUS_PARSER_STACK
    };
    (void)sqlite3_status(status_ops[0], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_status(status_ops[1], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_status(status_ops[2], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_status(status_ops[3], &cur, &hiwtr, consume_bool(&p, &remaining));
    (void)sqlite3_status(status_ops[4], &cur, &hiwtr, consume_bool(&p, &remaining));
  }

  {
    int n = consume_int(&p, &remaining);
    char *mp1 = sqlite3_mprintf("%.*s", n & 0xFF, (const char *)p);
    sqlite3_free(mp1);
  }

  {
    int n = consume_int(&p, &remaining);
    char *mp2 = sqlite3_mprintf("file:%.*s?mode=%s&cache=%s",
                                n & 0x3F,
                                (const char *)p,
                                (consume_bool(&p, &remaining) ? "memory" : "ro"),
                                (consume_bool(&p, &remaining) ? "shared" : "private"));
    sqlite3_free(mp2);
  }

  const char *open_name2;
  switch(consume_u8(&p, &remaining) % 4){
    case 0: open_name2 = ":memory:"; break;
    case 1: open_name2 = "./dummy_file"; break;
    case 2: open_name2 = "file:./dummy_file?mode=rwc"; break;
    default: open_name2 = ""; break;
  }

  (void)sqlite3_open(open_name2, &db2);
  (void)sqlite3_errmsg(db2);

  size_t part1 = remaining / 3;
  size_t part2 = (remaining - part1) / 2;
  size_t part3 = remaining - part1 - part2;

  char *sql2 = make_cstring(p, part1);
  char *sql3 = make_cstring(p + part1, part2);
  char *sql4 = make_cstring(p + part1 + part2, part3);

  if(!sql2) sql2 = make_cstring((const uint8_t *)"", 0);
  if(!sql3) sql3 = make_cstring((const uint8_t *)"", 0);
  if(!sql4) sql4 = make_cstring((const uint8_t *)"", 0);

  (void)sqlite3_exec(db2, sql2 ? sql2 : "", NULL, NULL, &errmsg);
  (void)sqlite3_exec(db2, sql3 ? sql3 : "", NULL, NULL, &errmsg);
  (void)sqlite3_exec(db2, sql4 ? sql4 : "", NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
  errmsg = NULL;

  free(sql2);
  free(sql3);
  free(sql4);

  (void)sqlite3_close(db2);
  return 0;
}