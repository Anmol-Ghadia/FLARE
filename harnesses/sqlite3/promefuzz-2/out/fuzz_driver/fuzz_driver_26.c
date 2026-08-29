// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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
#include "sqlite3.h"

static void trace_cb(void *ctx, const char *sql) {
  (void)ctx;
  (void)sql;
}

static int exec_cb(void *ctx, int argc, char **argv, char **colnames) {
  volatile int *state = (volatile int *)ctx;
  int i;
  if (state) {
    *state ^= argc;
    for (i = 0; i < argc; ++i) {
      if (argv && argv[i]) *state ^= (unsigned char)argv[i][0];
      if (colnames && colnames[i]) *state ^= (unsigned char)colnames[i][0];
    }
  }
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *formatted = NULL;
  char *errmsg = NULL;
  int rc;
  volatile int cb_state = 0;
  FILE *fp;
  char *sqlbuf;
  size_t sql_len;
  int use_callback;
  int use_trace;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);

  if (db == NULL) {
    return 0;
  }

  use_trace = (Size > 0) ? (Data[0] & 1) : 0;
  sqlite3_trace(db, use_trace ? trace_cb : NULL, (void *)&cb_state);

  if (Size > 2) {
    size_t frag_len = Size - 2;
    sqlbuf = (char *)malloc(frag_len + 1);
    if (!sqlbuf) {
      sqlite3_close(db);
      return 0;
    }
    memcpy(sqlbuf, Data + 2, frag_len);
    sqlbuf[frag_len] = '\0';
  } else {
    sqlbuf = (char *)malloc(1);
    if (!sqlbuf) {
      sqlite3_close(db);
      return 0;
    }
    sqlbuf[0] = '\0';
  }

  formatted = sqlite3_mprintf("%s", sqlbuf);
  free(sqlbuf);

  if (formatted) {
    use_callback = (Size > 1) ? (Data[1] & 1) : 0;
    rc = sqlite3_exec(db, formatted, use_callback ? exec_cb : NULL,
                      use_callback ? (void *)&cb_state : NULL, &errmsg);
    (void)rc;
  }

  sqlite3_free(errmsg);
  sqlite3_free(formatted);
  sqlite3_close(db);
  return 0;
}