// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
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

static void aux_destructor(void *p){
  sqlite3_free(p);
}

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int mode = 0;
  int idx = 0;
  void *ud = 0;
  void *aux = 0;
  double d = 0.0;
  unsigned char const *txt = 0;
  int err = SQLITE_ERROR;
  void *copy = 0;

  (void)argc;
  (void)argv;

  ud = sqlite3_user_data(ctx);
  if(ud){
    mode = (*(const unsigned char *)ud) % 6;
  }

  if(argc > 0){
    idx = sqlite3_value_int(argv[0]);
    if(idx < 0) idx = -idx;
    idx %= 4;
  }

  aux = sqlite3_get_auxdata(ctx, idx);

  if(argc > 1){
    txt = sqlite3_value_text(argv[1]);
    if(txt){
      size_t n = strlen((const char *)txt);
      copy = sqlite3_malloc64(n + 1);
      if(copy){
        memcpy(copy, txt, n + 1);
        sqlite3_set_auxdata(ctx, idx, copy, aux_destructor);
      }else{
        sqlite3_result_error_nomem(ctx);
        return;
      }
    }
  }else if(!aux){
    copy = sqlite3_malloc64(1);
    if(copy){
      ((char *)copy)[0] = 0;
      sqlite3_set_auxdata(ctx, idx, copy, aux_destructor);
    }else{
      sqlite3_result_error_nomem(ctx);
      return;
    }
  }

  switch(mode){
    case 0:
      if(aux){
        d = (double)(uintptr_t)aux;
      }else if(argc > 2){
        d = sqlite3_value_double(argv[2]);
      }
      sqlite3_result_double(ctx, d);
      break;
    case 1:
      sqlite3_result_null(ctx);
      break;
    case 2:
      if(argc > 2){
        err = sqlite3_value_int(argv[2]);
      }
      sqlite3_result_error_code(ctx, err);
      break;
    case 3:
      sqlite3_result_error_nomem(ctx);
      break;
    case 4:
      sqlite3_result_double(ctx, aux ? 1.0 : 0.0);
      break;
    default:
      if(argc > 2){
        d = sqlite3_value_double(argv[2]);
      }
      sqlite3_result_double(ctx, d * 0.5);
      break;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  unsigned char *ud = NULL;
  const char *sql =
    "WITH RECURSIVE c(x) AS ("
    "  SELECT 1 UNION ALL SELECT x+1 FROM c WHERE x<8"
    ") "
    "SELECT fuzz_aux(?1, ?2, ?3) FROM c;";

  if(sqlite3_open(":memory:", &db) != SQLITE_OK){
    if(db) sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_exec(db, "CREATE TABLE t(a,b);", NULL, NULL, &errmsg);
  if(rc != SQLITE_OK){
    sqlite3_free(errmsg);
  }

  if(Size > 0){
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  ud = (unsigned char *)sqlite3_malloc64(1);
  if(ud){
    *ud = (unsigned char)(Size ? Data[0] : 0);
  }

  rc = sqlite3_create_function_v2(
    db, "fuzz_aux", 3, SQLITE_UTF8, ud,
    fuzz_sql_func, NULL, NULL, sqlite3_free
  );
  if(rc != SQLITE_OK){
    sqlite3_free(ud);
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if(rc == SQLITE_OK && stmt){
    int i;
    int idxv = 0;
    double dv = 0.0;
    char *textbuf = NULL;
    size_t textlen = 0;

    if(Size >= 4){
      idxv = (int)(
        ((unsigned int)Data[0] << 24) ^
        ((unsigned int)Data[1] << 16) ^
        ((unsigned int)Data[2] << 8) ^
        ((unsigned int)Data[3])
      );
    }else if(Size > 0){
      idxv = (int)Data[0];
    }

    if(Size >= 12){
      uint64_t bits = 0;
      memcpy(&bits, Data + 4, sizeof(bits));
      memcpy(&dv, &bits, sizeof(dv));
    }else{
      dv = (double)Size;
    }

    if(Size > 12){
      textlen = Size - 12;
      textbuf = (char *)malloc(textlen + 1);
      if(textbuf){
        memcpy(textbuf, Data + 12, textlen);
        textbuf[textlen] = '\0';
      }
    }

    sqlite3_bind_int(stmt, 1, idxv);
    if(textbuf){
      sqlite3_bind_text(stmt, 2, textbuf, (int)textlen, SQLITE_TRANSIENT);
    }else{
      sqlite3_bind_null(stmt, 2);
    }
    sqlite3_bind_double(stmt, 3, dv);

    for(i = 0; i < 3; i++){
      while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
      }
      sqlite3_reset(stmt);

      if(Size > (size_t)i){
        sqlite3_bind_int(stmt, 1, idxv + (int)(signed char)Data[i]);
      }

      if(i == 1){
        sqlite3_bind_null(stmt, 2);
      }else if(textbuf){
        sqlite3_bind_text(stmt, 2, textbuf, (int)textlen, SQLITE_TRANSIENT);
      }

      if(Size > (size_t)(i + 4)){
        sqlite3_bind_double(stmt, 3, dv + (double)(signed char)Data[i + 4]);
      }
    }

    free(textbuf);
  }

  if(stmt){
    sqlite3_finalize(stmt);
  }
  sqlite3_close(db);
  return 0;
}