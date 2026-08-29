// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_auto_extension at sqlite3.c:114809:16 in sqlite3.h
// sqlite3_auto_extension at sqlite3.c:114809:16 in sqlite3.h
// sqlite3_auto_extension at sqlite3.c:114809:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int dummy_autoext(sqlite3 *db, const char **pzErrMsg,
                         const struct sqlite3_api_routines *pThunk) {
  (void)db;
  (void)pzErrMsg;
  (void)pThunk;
  return SQLITE_OK;
}

static int dummy_autoext_fail(sqlite3 *db, const char **pzErrMsg,
                              const struct sqlite3_api_routines *pThunk) {
  (void)db;
  (void)pThunk;
  if (pzErrMsg) {
    *pzErrMsg = 0;
  }
  return SQLITE_ERROR;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;
  int small_int;
  int onoff;
  sqlite3_int64 i64v;
  int mode;
  int hdrsz = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlite3_shutdown();

  mode = (int)(read_u32(&p, &n) % 10);
  small_int = (int)(read_u32(&p, &n) & 0x7fffffff);
  onoff = (int)(read_u32(&p, &n) & 1);
  i64v = (sqlite3_int64)read_u32(&p, &n);

  switch (mode) {
    case 0:
      rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 1:
      rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 2:
      rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 3:
      rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, onoff);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 4:
      rc = sqlite3_config(SQLITE_CONFIG_URI, onoff);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 5:
      rc = sqlite3_initialize();
      (void)rc;
      rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, onoff);
      (void)rc;
      break;
    case 6:
      rc = sqlite3_config(SQLITE_CONFIG_PCACHE_HDRSZ, &hdrsz);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 7:
      rc = sqlite3_config(SQLITE_CONFIG_LOOKASIDE, 0, small_int & 0xff,
                          (small_int >> 8) & 0xff);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    case 8:
      rc = sqlite3_initialize();
      (void)rc;
      rc = sqlite3_shutdown();
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
    default:
      rc = sqlite3_config(0x7fffffff, small_int);
      (void)rc;
      rc = sqlite3_initialize();
      (void)rc;
      break;
  }

  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_ISINIT);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_FAULT_INSTALL, 0);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_BENIGN_MALLOC_HOOKS, 0, 0);
  (void)sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE,
                             (unsigned int)(i64v & 0xffffffffu));

  (void)sqlite3_auto_extension((void (*)(void))dummy_autoext);
  (void)sqlite3_auto_extension((void (*)(void))dummy_autoext_fail);
  (void)sqlite3_auto_extension((void (*)(void))dummy_autoext);
  (void)sqlite3_cancel_auto_extension((void (*)(void))dummy_autoext_fail);
  (void)sqlite3_cancel_auto_extension((void (*)(void))dummy_autoext);
  (void)sqlite3_cancel_auto_extension((void (*)(void))dummy_autoext);

  sqlite3_shutdown();
  remove("./dummy_file");
  return 0;
}