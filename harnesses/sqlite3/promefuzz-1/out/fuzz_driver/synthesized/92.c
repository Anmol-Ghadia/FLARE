// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_load_extension at sqlite3.c:114732:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_load_extension at sqlite3.c:114732:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_reset_auto_extension at sqlite3.c:114881:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static size_t take_bytes(const uint8_t **data, size_t *size, char *out, size_t max_out) {
  if (max_out == 0) return 0;
  size_t want = *size ? ((*data)[0] % max_out) : 0;
  if (*size) {
    (*data)++;
    (*size)--;
  }
  if (want > *size) want = *size;
  memcpy(out, *data, want);
  out[want] = '\0';
  *data += want;
  *size -= want;
  return want;
}

static void scalar_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int sum = argc;
  for (int i = 0; i < argc; i++) {
    sum += sqlite3_value_bytes(argv[i]);
  }
  sqlite3_result_int(ctx, sum);
}

static void agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, sizeof(int));
  if (!acc) return;
  *acc += argc;
  for (int i = 0; i < argc; i++) {
    *acc += sqlite3_value_type(argv[i]);
  }
}

static void agg_final(sqlite3_context *ctx) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int(ctx, acc ? *acc : 0);
}

static int db_has_loadext_support(sqlite3 *db) {
  sqlite3_vfs *vfs = NULL;
  if (!db) return 0;
  if (sqlite3_file_control(db, "main", SQLITE_FCNTL_VFS_POINTER, &vfs) != SQLITE_OK) {
    return 0;
  }
  if (!vfs) return 0;
  return vfs->xDlOpen != NULL && vfs->xDlSym != NULL && vfs->xDlClose != NULL;
}

int LLVMFuzzerTestOneInput_92(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  (void)sqlite3_initialize();
  sqlite3_reset_auto_extension();

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  char vfs_name[64];
  char func_name[300];
  char proc_name[128];
  char keep1[64], keep2[64], keep3[64];

  take_bytes(&p, &remaining, vfs_name, sizeof(vfs_name) - 1);
  take_bytes(&p, &remaining, func_name, sizeof(func_name) - 1);
  take_bytes(&p, &remaining, proc_name, sizeof(proc_name) - 1);
  take_bytes(&p, &remaining, keep1, sizeof(keep1) - 1);
  take_bytes(&p, &remaining, keep2, sizeof(keep2) - 1);
  take_bytes(&p, &remaining, keep3, sizeof(keep3) - 1);

  const char *filename = (remaining && (p[0] & 1)) ? ":memory:" : "./dummy_file";
  if (remaining) {
    p++;
    remaining--;
  }

  {
    int flag_options[] = {
      SQLITE_OPEN_READONLY,
      SQLITE_OPEN_READWRITE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX
    };
    int flags = flag_options[read_u32(&p, &remaining) % (sizeof(flag_options) / sizeof(flag_options[0]))];
    const char *zVfs = NULL;

    if (vfs_name[0] != '\0' && (read_u32(&p, &remaining) & 1)) {
      zVfs = vfs_name;
    }

    (void)sqlite3_open_v2(filename, &db, flags, zVfs);
  }

  if (db) {
    int enc_options[] = {
      SQLITE_UTF8,
      SQLITE_UTF16LE,
      SQLITE_UTF16BE,
      SQLITE_UTF16,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC,
      SQLITE_UTF8 | SQLITE_DIRECTONLY,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
    };
    int eTextRep = enc_options[read_u32(&p, &remaining) % (sizeof(enc_options) / sizeof(enc_options[0]))];
    int nArg = (int)(read_u32(&p, &remaining) % 129) - 1;
    int mode = (int)(read_u32(&p, &remaining) % 3);

    if (mode == 0) {
      (void)sqlite3_create_function(db, func_name[0] ? func_name : "f", nArg, eTextRep,
                                    NULL, scalar_func, NULL, NULL);
    } else if (mode == 1) {
      (void)sqlite3_create_function(db, func_name[0] ? func_name : "f", nArg, eTextRep,
                                    NULL, NULL, agg_step, agg_final);
    } else {
      (void)sqlite3_create_function(db, func_name[0] ? func_name : "f", nArg, eTextRep,
                                    NULL, NULL, NULL, NULL);
    }

    {
      const char *keep_list1[] = { keep1[0] ? keep1 : "fts5", NULL };
      const char *keep_list2[] = { keep1[0] ? keep1 : "fts5", keep2[0] ? keep2 : "rtree", NULL };
      const char *keep_list3[] = { keep1[0] ? keep1 : "fts5", keep2[0] ? keep2 : "rtree", keep3[0] ? keep3 : "json_each", NULL };

      switch (read_u32(&p, &remaining) % 4) {
        case 0:
          (void)sqlite3_drop_modules(db, NULL);
          break;
        case 1:
          (void)sqlite3_drop_modules(db, keep_list1);
          break;
        case 2:
          (void)sqlite3_drop_modules(db, keep_list2);
          break;
        default:
          (void)sqlite3_drop_modules(db, keep_list3);
          break;
      }
    }

    if (db_has_loadext_support(db)) {
      char *err = NULL;
      const char *zProc = NULL;

      (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL);

      if (read_u32(&p, &remaining) & 1) {
        zProc = proc_name[0] ? proc_name : NULL;
      }

      (void)sqlite3_load_extension(db, "./dummy_file", zProc, &err);
      if (err) {
        sqlite3_free(err);
        err = NULL;
      }

      (void)sqlite3_load_extension(db, "nonexistent_extension", zProc, &err);
      if (err) {
        sqlite3_free(err);
        err = NULL;
      }
    }

    sqlite3_close(db);
  }

  return 0;
}