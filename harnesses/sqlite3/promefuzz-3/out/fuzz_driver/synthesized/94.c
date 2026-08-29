// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
// sqlite3_vfs_unregister at sqlite3.c:11505:16 in sqlite3.h
// sqlite3_vfs_unregister at sqlite3.c:11505:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
// sqlite3_vfs_register at sqlite3.c:11477:16 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_vfs_unregister at sqlite3.c:11505:16 in sqlite3.h
// sqlite3_vfs_unregister at sqlite3.c:11505:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint8_t consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0) return 0;
    uint8_t v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static uint32_t consume_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | consume_u8(data, size);
    }
    return v;
}

static int consume_int_range(const uint8_t **data, size_t *size, int min, int max) {
    if (max <= min) return min;
    return min + (int)(consume_u32(data, size) % (uint32_t)(max - min + 1));
}

static void make_name_from_data(const uint8_t **data, size_t *size, char *out, size_t out_sz, const char *prefix) {
    size_t i = 0;
    if (out_sz == 0) return;

    while (prefix && *prefix && i + 1 < out_sz) {
        out[i++] = *prefix++;
    }
    if (i + 1 < out_sz) out[i++] = '_';

    while (*size > 0 && i + 1 < out_sz) {
        uint8_t c = consume_u8(data, size);
        c = (uint8_t)('a' + (c % 26));
        out[i++] = (char)c;
        if ((c % 7) == 0) break;
    }

    if (i == 0) out[i++] = 'v';
    out[i] = '\0';
}

typedef struct {
    sqlite3_vfs vfs;
    char name[64];
} FuzzVfs;

static int fuzz_xOpen(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile, int flags, int *pOutFlags) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    if (!base || !base->xOpen) {
        if (pFile) pFile->pMethods = NULL;
        return SQLITE_IOERR;
    }
    return base->xOpen(base, zName, pFile, flags, pOutFlags);
}

static int fuzz_xDelete(sqlite3_vfs *pVfs, const char *zName, int syncDir) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xDelete) ? base->xDelete(base, zName, syncDir) : SQLITE_IOERR;
}

static int fuzz_xAccess(sqlite3_vfs *pVfs, const char *zName, int flags, int *pResOut) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xAccess) ? base->xAccess(base, zName, flags, pResOut) : SQLITE_IOERR;
}

static int fuzz_xFullPathname(sqlite3_vfs *pVfs, const char *zName, int nOut, char *zOut) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xFullPathname) ? base->xFullPathname(base, zName, nOut, zOut) : SQLITE_IOERR;
}

static void *fuzz_xDlOpen(sqlite3_vfs *pVfs, const char *zFilename) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xDlOpen) ? base->xDlOpen(base, zFilename) : NULL;
}

static void fuzz_xDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    if (base && base->xDlError) {
        base->xDlError(base, nByte, zErrMsg);
    } else if (zErrMsg && nByte > 0) {
        zErrMsg[0] = '\0';
    }
}

static void (*fuzz_xDlSym(sqlite3_vfs *pVfs, void *p, const char *zSymbol))(void) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xDlSym) ? base->xDlSym(base, p, zSymbol) : NULL;
}

static void fuzz_xDlClose(sqlite3_vfs *pVfs, void *pHandle) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    if (base && base->xDlClose) base->xDlClose(base, pHandle);
}

static int fuzz_xRandomness(sqlite3_vfs *pVfs, int nByte, char *zOut) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xRandomness) ? base->xRandomness(base, nByte, zOut) : 0;
}

static int fuzz_xSleep(sqlite3_vfs *pVfs, int microseconds) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xSleep) ? base->xSleep(base, microseconds) : 0;
}

static int fuzz_xCurrentTime(sqlite3_vfs *pVfs, double *pTime) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xCurrentTime) ? base->xCurrentTime(base, pTime) : SQLITE_IOERR;
}

static int fuzz_xGetLastError(sqlite3_vfs *pVfs, int a, char *b) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xGetLastError) ? base->xGetLastError(base, a, b) : 0;
}

static int fuzz_xCurrentTimeInt64(sqlite3_vfs *pVfs, sqlite3_int64 *pTime) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xCurrentTimeInt64) ? base->xCurrentTimeInt64(base, pTime) : SQLITE_IOERR;
}

static int fuzz_xSetSystemCall(sqlite3_vfs *pVfs, const char *zName, sqlite3_syscall_ptr pCall) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xSetSystemCall) ? base->xSetSystemCall(base, zName, pCall) : SQLITE_NOTFOUND;
}

static sqlite3_syscall_ptr fuzz_xGetSystemCall(sqlite3_vfs *pVfs, const char *zName) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xGetSystemCall) ? base->xGetSystemCall(base, zName) : NULL;
}

static const char *fuzz_xNextSystemCall(sqlite3_vfs *pVfs, const char *zName) {
    sqlite3_vfs *base = (sqlite3_vfs *)pVfs->pAppData;
    return (base && base->xNextSystemCall) ? base->xNextSystemCall(base, zName) : NULL;
}

static void init_fuzz_vfs(FuzzVfs *dst, sqlite3_vfs *base, const char *name) {
    memset(dst, 0, sizeof(*dst));
    dst->vfs.iVersion = base->iVersion;
    dst->vfs.szOsFile = base->szOsFile;
    dst->vfs.mxPathname = base->mxPathname;
    dst->vfs.zName = dst->name;
    dst->vfs.pAppData = base;

    strncpy(dst->name, name, sizeof(dst->name) - 1);
    dst->name[sizeof(dst->name) - 1] = '\0';

    dst->vfs.xOpen = fuzz_xOpen;
    dst->vfs.xDelete = fuzz_xDelete;
    dst->vfs.xAccess = fuzz_xAccess;
    dst->vfs.xFullPathname = fuzz_xFullPathname;
    dst->vfs.xDlOpen = fuzz_xDlOpen;
    dst->vfs.xDlError = fuzz_xDlError;
    dst->vfs.xDlSym = fuzz_xDlSym;
    dst->vfs.xDlClose = fuzz_xDlClose;
    dst->vfs.xRandomness = fuzz_xRandomness;
    dst->vfs.xSleep = fuzz_xSleep;
    dst->vfs.xCurrentTime = fuzz_xCurrentTime;
    dst->vfs.xGetLastError = fuzz_xGetLastError;

    if (base->iVersion >= 2) {
        dst->vfs.xCurrentTimeInt64 = fuzz_xCurrentTimeInt64;
    }
    if (base->iVersion >= 3) {
        dst->vfs.xSetSystemCall = fuzz_xSetSystemCall;
        dst->vfs.xGetSystemCall = fuzz_xGetSystemCall;
        dst->vfs.xNextSystemCall = fuzz_xNextSystemCall;
    }
}

int LLVMFuzzerTestOneInput_94(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (sqlite3_initialize() != SQLITE_OK) {
        return 0;
    }

    sqlite3_vfs *default_vfs = sqlite3_vfs_find(NULL);
    if (!default_vfs || !default_vfs->zName || default_vfs->zName[0] == '\0') {
        return 0;
    }

    char name1[64];
    char name2[64];
    make_name_from_data(&p, &remaining, name1, sizeof(name1), "fuzzvfs1");
    make_name_from_data(&p, &remaining, name2, sizeof(name2), "fuzzvfs2");
    if (strcmp(name1, name2) == 0) {
        size_t len = strlen(name2);
        if (len + 1 < sizeof(name2)) {
            name2[len] = 'x';
            name2[len + 1] = '\0';
        }
    }

    FuzzVfs vfs1, vfs2;
    init_fuzz_vfs(&vfs1, default_vfs, name1);
    init_fuzz_vfs(&vfs2, default_vfs, name2);

    int registered1 = 0;
    int registered2 = 0;

    int steps = consume_int_range(&p, &remaining, 4, 32);
    for (int i = 0; i < steps; i++) {
        int op = consume_int_range(&p, &remaining, 0, 11);
        switch (op) {
            case 0:
                (void)sqlite3_initialize();
                break;

            case 1: {
                int n = consume_int_range(&p, &remaining, -16, 512);
                if (consume_u8(&p, &remaining) & 1) {
                    sqlite3_randomness(n, NULL);
                } else {
                    unsigned char buf[512];
                    memset(buf, 0, sizeof(buf));
                    sqlite3_randomness(n, buf);
                }
                break;
            }

            case 2: {
                int rc = sqlite3_vfs_register(&vfs1.vfs, consume_u8(&p, &remaining) & 1);
                if (rc == SQLITE_OK) registered1 = 1;
                break;
            }

            case 3: {
                int rc = sqlite3_vfs_register(&vfs2.vfs, consume_u8(&p, &remaining) & 1);
                if (rc == SQLITE_OK) registered2 = 1;
                break;
            }

            case 4:
                (void)sqlite3_vfs_unregister(&vfs1.vfs);
                registered1 = 0;
                break;

            case 5:
                (void)sqlite3_vfs_unregister(&vfs2.vfs);
                registered2 = 0;
                break;

            case 6: {
                sqlite3_vfs *v = sqlite3_vfs_find(NULL);
                if (v && v->zName) (void)sqlite3_vfs_find(v->zName);
                break;
            }

            case 7: {
                char query[64];
                make_name_from_data(&p, &remaining, query, sizeof(query), "query");
                (void)sqlite3_vfs_find(query);
                break;
            }

            case 8:
                (void)sqlite3_vfs_find(vfs1.name);
                (void)sqlite3_vfs_find(vfs2.name);
                break;

            case 9: {
                int n = consume_int_range(&p, &remaining, 0, 256);
                unsigned char *dyn = n > 0 ? (unsigned char *)malloc((size_t)n) : NULL;
                if (dyn) {
                    memset(dyn, 0, (size_t)n);
                    sqlite3_randomness(n, dyn);
                    free(dyn);
                } else {
                    sqlite3_randomness(n, NULL);
                }
                break;
            }

            case 10:
                (void)sqlite3_vfs_register(&vfs1.vfs, 1);
                registered1 = 1;
                break;

            case 11:
                (void)sqlite3_vfs_register(&vfs2.vfs, 1);
                registered2 = 1;
                break;

            default:
                break;
        }

        if ((consume_u8(&p, &remaining) & 7) == 0) {
            unsigned char smallbuf[32];
            int n = consume_int_range(&p, &remaining, 0, (int)sizeof(smallbuf));
            memset(smallbuf, 0, sizeof(smallbuf));
            sqlite3_randomness(n, smallbuf);
        }
    }

    if (registered1) (void)sqlite3_vfs_unregister(&vfs1.vfs);
    if (registered2) (void)sqlite3_vfs_unregister(&vfs2.vfs);

    return 0;
}