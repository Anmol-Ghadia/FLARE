#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4; i++){
    v <<= 8;
    if(*Off < Size){
      v |= Data[*Off];
      (*Off)++;
    }
  }
  return v;
}

static int fuzz_xOpen(sqlite3_vfs *vfs, const char *zName, sqlite3_file *file, int flags, int *pOutFlags){
  (void)vfs;
  (void)zName;
  (void)flags;
  if(file) file->pMethods = NULL;
  if(pOutFlags) *pOutFlags = 0;
  return SQLITE_CANTOPEN;
}

static int fuzz_xDelete(sqlite3_vfs *vfs, const char *zName, int syncDir){
  (void)vfs;
  (void)zName;
  (void)syncDir;
  return SQLITE_OK;
}

static int fuzz_xAccess(sqlite3_vfs *vfs, const char *zName, int flags, int *pResOut){
  (void)vfs;
  (void)zName;
  (void)flags;
  if(pResOut) *pResOut = 0;
  return SQLITE_OK;
}

static int fuzz_xFullPathname(sqlite3_vfs *vfs, const char *zName, int nOut, char *zOut){
  (void)vfs;
  const char *src = zName ? zName : "./dummy_file";
  size_t len = strlen(src);
  if(nOut <= 0) return SQLITE_CANTOPEN;
  if((int)len >= nOut) return SQLITE_CANTOPEN;
  memcpy(zOut, src, len + 1);
  return SQLITE_OK;
}

static void *fuzz_xDlOpen(sqlite3_vfs *vfs, const char *zFilename){
  (void)vfs;
  (void)zFilename;
  return NULL;
}

static void fuzz_xDlError(sqlite3_vfs *vfs, int nByte, char *zErrMsg){
  (void)vfs;
  if(nByte > 0 && zErrMsg){
    zErrMsg[0] = '\0';
  }
}

static void (*fuzz_xDlSym(sqlite3_vfs *vfs, void *p, const char *zSymbol))(void){
  (void)vfs;
  (void)p;
  (void)zSymbol;
  return NULL;
}

static void fuzz_xDlClose(sqlite3_vfs *vfs, void *p){
  (void)vfs;
  (void)p;
}

static int fuzz_xRandomness(sqlite3_vfs *vfs, int nByte, char *zOut){
  (void)vfs;
  if(zOut && nByte > 0){
    memset(zOut, 0, (size_t)nByte);
  }
  return nByte > 0 ? nByte : 0;
}

static int fuzz_xSleep(sqlite3_vfs *vfs, int microseconds){
  (void)vfs;
  return microseconds;
}

static int fuzz_xCurrentTime(sqlite3_vfs *vfs, double *pTime){
  (void)vfs;
  if(pTime) *pTime = 0.0;
  return SQLITE_OK;
}

static int fuzz_xGetLastError(sqlite3_vfs *vfs, int a, char *b){
  (void)vfs;
  (void)a;
  if(b) b[0] = '\0';
  return 0;
}

static int fuzz_xCurrentTimeInt64(sqlite3_vfs *vfs, sqlite3_int64 *p){
  (void)vfs;
  if(p) *p = 0;
  return SQLITE_OK;
}

static int fuzz_xSetSystemCall(sqlite3_vfs *vfs, const char *zName, sqlite3_syscall_ptr pFunc){
  (void)vfs;
  (void)zName;
  (void)pFunc;
  return SQLITE_OK;
}

static sqlite3_syscall_ptr fuzz_xGetSystemCall(sqlite3_vfs *vfs, const char *zName){
  (void)vfs;
  (void)zName;
  return NULL;
}

static const char *fuzz_xNextSystemCall(sqlite3_vfs *vfs, const char *zName){
  (void)vfs;
  (void)zName;
  return NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t off = 0;
  int rc;
  FILE *fp;
  char namebuf[64];
  sqlite3_vfs custom_vfs;
  uint32_t selector, makeDfltRaw, sleepRaw, versionRaw;

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_os_init();
  (void)rc;

  memset(&custom_vfs, 0, sizeof(custom_vfs));
  selector = read_u32(Data, Size, &off);
  makeDfltRaw = read_u32(Data, Size, &off);
  sleepRaw = read_u32(Data, Size, &off);
  versionRaw = read_u32(Data, Size, &off);

  snprintf(namebuf, sizeof(namebuf), "fuzzvfs_%08x", selector);

  custom_vfs.iVersion = (int)((versionRaw % 3U) + 1U);
  custom_vfs.szOsFile = (int)sizeof(sqlite3_file);
  custom_vfs.mxPathname = 256;
  custom_vfs.zName = namebuf;
  custom_vfs.pAppData = NULL;
  custom_vfs.xOpen = fuzz_xOpen;
  custom_vfs.xDelete = fuzz_xDelete;
  custom_vfs.xAccess = fuzz_xAccess;
  custom_vfs.xFullPathname = fuzz_xFullPathname;
  custom_vfs.xDlOpen = fuzz_xDlOpen;
  custom_vfs.xDlError = fuzz_xDlError;
  custom_vfs.xDlSym = fuzz_xDlSym;
  custom_vfs.xDlClose = fuzz_xDlClose;
  custom_vfs.xRandomness = fuzz_xRandomness;
  custom_vfs.xSleep = fuzz_xSleep;
  custom_vfs.xCurrentTime = fuzz_xCurrentTime;
  custom_vfs.xGetLastError = fuzz_xGetLastError;
  if(custom_vfs.iVersion >= 2){
    custom_vfs.xCurrentTimeInt64 = fuzz_xCurrentTimeInt64;
  }
  if(custom_vfs.iVersion >= 3){
    custom_vfs.xSetSystemCall = fuzz_xSetSystemCall;
    custom_vfs.xGetSystemCall = fuzz_xGetSystemCall;
    custom_vfs.xNextSystemCall = fuzz_xNextSystemCall;
  }

  switch(selector % 8U){
    case 0:
      (void)sqlite3_vfs_register(&custom_vfs, (int)(makeDfltRaw & 1U));
      (void)sqlite3_sleep((int)(sleepRaw % 5U));
      (void)sqlite3_vfs_unregister(&custom_vfs);
      break;
    case 1:
      (void)sqlite3_sleep((int)(sleepRaw % 10U));
      (void)sqlite3_vfs_register(&custom_vfs, (int)(makeDfltRaw & 1U));
      (void)sqlite3_vfs_unregister(&custom_vfs);
      break;
    case 2:
      (void)sqlite3_vfs_register(&custom_vfs, 0);
      (void)sqlite3_vfs_register(&custom_vfs, 1);
      (void)sqlite3_vfs_unregister(&custom_vfs);
      break;
    case 3:
      (void)sqlite3_vfs_unregister(&custom_vfs);
      (void)sqlite3_vfs_register(&custom_vfs, (int)(makeDfltRaw & 1U));
      (void)sqlite3_vfs_unregister(&custom_vfs);
      break;
    case 4:
      (void)sqlite3_shutdown();
      (void)sqlite3_os_init();
      (void)sqlite3_vfs_register(&custom_vfs, (int)(makeDfltRaw & 1U));
      (void)sqlite3_sleep((int)(sleepRaw % 3U));
      (void)sqlite3_vfs_unregister(&custom_vfs);
      break;
    case 5:
      (void)sqlite3_os_end();
      (void)sqlite3_os_init();
      (void)sqlite3_sleep((int)(sleepRaw % 2U));
      break;
    case 6:
      (void)sqlite3_vfs_register(&custom_vfs, 1);
      (void)sqlite3_shutdown();
      (void)sqlite3_os_init();
      (void)sqlite3_vfs_unregister(&custom_vfs);
      break;
    default:
      (void)sqlite3_sleep((int)(sleepRaw % 20U));
      (void)sqlite3_shutdown();
      (void)sqlite3_os_init();
      break;
  }

  (void)sqlite3_vfs_unregister(&custom_vfs);
  (void)sqlite3_os_end();
  (void)sqlite3_shutdown();
  return 0;
}