// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static void udf_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  const unsigned char *txt = NULL;
  const void *blob = NULL;
  int txt_len = -1;
  int blob_len = 0;
  int iv = 0;
  int mode = 0;

  if(argc > 0 && argv[0]){
    mode = sqlite3_value_int(argv[0]);
  }
  if(argc > 1 && argv[1]){
    txt = sqlite3_value_text(argv[1]);
    txt_len = sqlite3_value_bytes(argv[1]);
  }
  if(argc > 2 && argv[2]){
    blob = sqlite3_value_blob(argv[2]);
    blob_len = sqlite3_value_bytes(argv[2]);
  }
  if(argc > 3 && argv[3]){
    iv = sqlite3_value_int(argv[3]);
  }

  switch((unsigned)mode % 8u){
    case 0:
      sqlite3_result_int(ctx, iv);
      break;
    case 1:
      sqlite3_result_text(ctx, (const char *)txt, txt_len, SQLITE_TRANSIENT);
      break;
    case 2:
      sqlite3_result_text(ctx, (const char *)txt, -1, SQLITE_TRANSIENT);
      break;
    case 3:
      sqlite3_result_blob(ctx, blob, blob_len, SQLITE_TRANSIENT);
      break;
    case 4:
      sqlite3_result_blob(ctx, blob, 0, SQLITE_TRANSIENT);
      break;
    case 5:
      sqlite3_result_zeroblob(ctx, iv < 0 ? -(iv % 1024) : (iv % 1024));
      break;
    case 6:
      (void)sqlite3_result_zeroblob64(ctx, (sqlite3_uint64)(unsigned int)iv);
      break;
    default:
      sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
      break;
  }
}

static void udf_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  unsigned char *state;
  int n = 0;
  int mode = 0;

  if(argc > 0 && argv[0]){
    mode = sqlite3_value_int(argv[0]);
  }
  if(argc > 1 && argv[1]){
    n = sqlite3_value_int(argv[1]);
  }

  state = (unsigned char *)sqlite3_aggregate_context(ctx, n);
  if(state){
    state[0] ^= (unsigned char)mode;
    if(n > 1){
      state[1] ^= (unsigned char)(argc & 0xff);
    }
  }else{
    (void)sqlite3_aggregate_context(ctx, 0);
  }

  switch((unsigned)mode % 5u){
    case 0:
      sqlite3_result_int(ctx, mode);
      break;
    case 1:
      sqlite3_result_zeroblob(ctx, n);
      break;
    case 2:
      (void)sqlite3_result_zeroblob64(ctx, (sqlite3_uint64)(unsigned int)n);
      break;
    case 3:
      if(argc > 2 && argv[2]){
        const unsigned char *t = sqlite3_value_text(argv[2]);
        int tl = sqlite3_value_bytes(argv[2]);
        sqlite3_result_text(ctx, (const char *)t, tl, SQLITE_TRANSIENT);
      }
      break;
    default:
      if(argc > 3 && argv[3]){
        const void *b = sqlite3_value_blob(argv[3]);
        int bl = sqlite3_value_bytes(argv[3]);
        sqlite3_result_blob(ctx, b, bl, SQLITE_TRANSIENT);
      }
      break;
  }
}

static void udf_agg_final(sqlite3_context *ctx){
  unsigned char *state = (unsigned char *)sqlite3_aggregate_context(ctx, 0);
  if(state){
    sqlite3_result_int(ctx, (int)state[0]);
  }else{
    sqlite3_result_zeroblob(ctx, 0);
  }
}

int LLVMFuzzerTestOneInput_75(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  size_t split1, split2, split3;
  const char *sql =
    "SELECT fuzz_scalar(?1, ?2, ?3, ?4);"
    "SELECT fuzz_agg(?1, ?4, ?2, ?3) FROM (SELECT 1 UNION ALL SELECT 2 UNION ALL SELECT 3);";

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  if(Size > 0){
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      (void)fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  (void)sqlite3_exec(db, "CREATE TABLE t(x); INSERT INTO t VALUES(1);", NULL, NULL, &errmsg);
  if(errmsg){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  (void)sqlite3_create_function(db, "fuzz_scalar", 4, SQLITE_UTF8, NULL, udf_scalar, NULL, NULL);
  (void)sqlite3_create_function(db, "fuzz_agg", -1, SQLITE_UTF8, NULL, NULL, udf_agg_step, udf_agg_final);

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if(rc == SQLITE_OK && stmt){
    split1 = Size / 4;
    split2 = Size / 2;
    split3 = (Size * 3) / 4;

    (void)sqlite3_bind_int(stmt, 1, Size ? (int)Data[0] : 0);

    if(split2 > split1){
      (void)sqlite3_bind_text(stmt, 2, (const char *)(Data + split1),
                              (int)(split2 - split1), SQLITE_TRANSIENT);
    }else{
      (void)sqlite3_bind_text(stmt, 2, "", 0, SQLITE_STATIC);
    }

    if(split3 > split2){
      (void)sqlite3_bind_blob(stmt, 3, Data + split2,
                              (int)(split3 - split2), SQLITE_TRANSIENT);
    }else{
      (void)sqlite3_bind_blob(stmt, 3, "", 0, SQLITE_STATIC);
    }

    if(Size > split3){
      int v = 0;
      size_t rem = Size - split3;
      size_t cpy = rem > sizeof(v) ? sizeof(v) : rem;
      memcpy(&v, Data + split3, cpy);
      (void)sqlite3_bind_int(stmt, 4, v);
    }else{
      (void)sqlite3_bind_int(stmt, 4, 0);
    }

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
    }

    while(rc == SQLITE_DONE){
      rc = sqlite3_next_stmt(db, NULL) ? SQLITE_DONE : SQLITE_DONE;
      break;
    }
  }

  if(stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}