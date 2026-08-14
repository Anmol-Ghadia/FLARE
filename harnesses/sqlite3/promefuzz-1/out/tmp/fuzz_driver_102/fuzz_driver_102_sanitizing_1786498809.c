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

static void fuzz_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int mode = 0;
  const unsigned char *ctrl = NULL;
  int ctrl_len = 0;
  int argi = 0;

  if (argc > 0 && argv[0]) {
    ctrl = sqlite3_value_text(argv[0]);
    ctrl_len = sqlite3_value_bytes(argv[0]);
    if (ctrl_len > 0 && ctrl) {
      mode = ctrl[0];
    }
    argi = 1;
  }

  (void)sqlite3_vtab_nochange(ctx);

  switch (mode % 8) {
    case 0: {
      if (argi < argc && argv[argi]) {
        sqlite3_result_value(ctx, argv[argi]);
      } else {
        sqlite3_result_text(ctx, "fallback", -1, SQLITE_STATIC);
      }
      break;
    }
    case 1: {
      if (argi < argc && argv[argi]) {
        const unsigned char *txt = sqlite3_value_text(argv[argi]);
        int n = sqlite3_value_bytes(argv[argi]);
        if (txt) {
          sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
        }
      } else {
        sqlite3_result_text(ctx, "x", -1, SQLITE_STATIC);
      }
      break;
    }
    case 2: {
      if (argi < argc && argv[argi]) {
        const void *txt16 = sqlite3_value_text16(argv[argi]);
        int n16 = sqlite3_value_bytes16(argv[argi]);
        if (txt16) {
          sqlite3_result_text16(ctx, txt16, n16, SQLITE_TRANSIENT);
        } else {
          static const unsigned short empty16[] = {0};
          sqlite3_result_text16(ctx, empty16, 0, SQLITE_STATIC);
        }
      } else {
        static const unsigned short sample16[] = {'o','k',0};
        sqlite3_result_text16(ctx, sample16, -1, SQLITE_STATIC);
      }
      break;
    }
    case 3: {
      if (argi < argc && argv[argi]) {
        const unsigned char *blob = sqlite3_value_blob(argv[argi]);
        int n = sqlite3_value_bytes(argv[argi]);
        if (blob && n >= 0) {
          sqlite3_result_blob(ctx, blob, n, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_blob(ctx, "", 0, SQLITE_STATIC);
        }
      } else {
        sqlite3_result_blob(ctx, "A", 1, SQLITE_STATIC);
      }
      break;
    }
    case 4: {
      if (argi < argc && argv[argi]) {
        const unsigned char *txt = sqlite3_value_text(argv[argi]);
        int n = sqlite3_value_bytes(argv[argi]);
        if (txt && n >= 0) {
          sqlite3_result_error(ctx, (const char *)txt, n);
        } else {
          sqlite3_result_error(ctx, "err", -1);
        }
      } else {
        sqlite3_result_error(ctx, "default error", -1);
      }
      break;
    }
    case 5: {
      if (argi + 1 < argc && argv[argi] && argv[argi + 1]) {
        sqlite3_result_value(ctx, argv[argi]);
        {
          const unsigned char *txt = sqlite3_value_text(argv[argi + 1]);
          int n = sqlite3_value_bytes(argv[argi + 1]);
          if (txt) {
            sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
          }
        }
      } else {
        sqlite3_result_text(ctx, "multi", -1, SQLITE_STATIC);
      }
      break;
    }
    case 6: {
      if (argi < argc && argv[argi]) {
        const void *txt16 = sqlite3_value_text16(argv[argi]);
        int n16 = sqlite3_value_bytes16(argv[argi]);
        if (txt16) {
          sqlite3_result_text16(ctx, txt16, n16 >= 0 ? n16 : 0, SQLITE_TRANSIENT);
        }
      }
      if (argi + 1 < argc && argv[argi + 1]) {
        const unsigned char *blob = sqlite3_value_blob(argv[argi + 1]);
        int n = sqlite3_value_bytes(argv[argi + 1]);
        if (blob && n >= 0) {
          sqlite3_result_blob(ctx, blob, n, SQLITE_TRANSIENT);
        }
      }
      break;
    }
    default: {
      if (argi < argc && argv[argi]) {
        sqlite3_result_value(ctx, argv[argi]);
      } else {
        sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
      }
      break;
    }
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  size_t split1, split2;
  char *ctrl = NULL;
  char *arg1 = NULL;
  char *arg2 = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 3, SQLITE_UTF8, NULL, fuzz_func, NULL, NULL);

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c);"
               "INSERT INTO t VALUES('one', x'000102', 'three');"
               "INSERT INTO t VALUES(NULL, 'text', x'414243');",
               NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  split1 = Size / 3;
  split2 = (Size * 2) / 3;

  ctrl = (char *)malloc(split1 + 1);
  arg1 = (char *)malloc((split2 - split1) + 1);
  arg2 = (char *)malloc((Size - split2) + 1);

  if (!ctrl || !arg1 || !arg2) {
    free(ctrl);
    free(arg1);
    free(arg2);
    sqlite3_close(db);
    return 0;
  }

  memcpy(ctrl, Data, split1);
  ctrl[split1] = '\0';
  memcpy(arg1, Data + split1, split2 - split1);
  arg1[split2 - split1] = '\0';
  memcpy(arg2, Data + split2, Size - split2);
  arg2[Size - split2] = '\0';

  rc = sqlite3_prepare_v2(
      db,
      "SELECT fuzzfunc(?1, ?2, ?3);"
      "SELECT fuzzfunc(a,b,c) FROM t;"
      "SELECT fuzzfunc(?3, ?1, ?2) FROM t WHERE rowid>=1;",
      -1, &stmt, NULL);

  if (rc == SQLITE_OK && stmt) {
    sqlite3_bind_text(stmt, 1, ctrl, (int)split1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, arg1, (int)(split2 - split1), SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 3, arg2, (int)(Size - split2), SQLITE_TRANSIENT);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }

  if (stmt) sqlite3_finalize(stmt);

  rc = sqlite3_prepare_v2(
      db,
      "SELECT fuzzfunc(readfile('./dummy_file'), ?1, ?2);",
      -1, &stmt, NULL);

  if (rc == SQLITE_OK && stmt) {
    sqlite3_bind_text(stmt, 1, ctrl, (int)split1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, arg1, (int)(split2 - split1), SQLITE_TRANSIENT);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }

  if (stmt) sqlite3_finalize(stmt);

  free(ctrl);
  free(arg1);
  free(arg2);
  sqlite3_close(db);
  return 0;
}