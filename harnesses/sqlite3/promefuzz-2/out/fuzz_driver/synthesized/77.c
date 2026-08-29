// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_reset_auto_extension at sqlite3.c:114881:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_reset_auto_extension at sqlite3.c:114881:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_auto_extension at sqlite3.c:114809:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_reset_auto_extension at sqlite3.c:114881:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "sqlite3.h"

static int fuzz_autoext_ok(sqlite3 *db, const char **pzErrMsg, const struct sqlite3_api_routines *pThunk) {
    (void)db;
    (void)pzErrMsg;
    (void)pThunk;
    return SQLITE_OK;
}

static int fuzz_autoext_err(sqlite3 *db, const char **pzErrMsg, const struct sqlite3_api_routines *pThunk) {
    (void)db;
    (void)pThunk;
    if (pzErrMsg) {
        *pzErrMsg = sqlite3_mprintf("fuzz_autoext_err");
    }
    return SQLITE_ERROR;
}

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off, uint32_t *Out) {
    if (*Off + 4 > Size) return 0;
    *Out = ((uint32_t)Data[*Off]) |
           ((uint32_t)Data[*Off + 1] << 8) |
           ((uint32_t)Data[*Off + 2] << 16) |
           ((uint32_t)Data[*Off + 3] << 24);
    *Off += 4;
    return 1;
}

int LLVMFuzzerTestOneInput_77(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    int is_initialized = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3_shutdown();
    if (sqlite3_initialize() == SQLITE_OK) {
        is_initialized = 1;
    }
    sqlite3_reset_auto_extension();

    for (int i = 0; i < 32 && off < Size; i++) {
        uint8_t op = Data[off++];

        switch (op % 12) {
            case 0: {
                if (sqlite3_initialize() == SQLITE_OK) {
                    is_initialized = 1;
                }
                break;
            }
            case 1: {
                if (is_initialized) {
                    sqlite3_shutdown();
                    is_initialized = 0;
                }
                break;
            }
            case 2: {
                if (!is_initialized && sqlite3_initialize() == SQLITE_OK) {
                    is_initialized = 1;
                }
                if (is_initialized) {
                    void (*fn)(void) = (op & 1)
                        ? (void (*)(void))fuzz_autoext_ok
                        : (void (*)(void))fuzz_autoext_err;
                    (void)sqlite3_auto_extension(fn);
                }
                break;
            }
            case 3: {
                if (!is_initialized && sqlite3_initialize() == SQLITE_OK) {
                    is_initialized = 1;
                }
                if (is_initialized) {
                    void (*fn)(void) = (op & 1)
                        ? (void (*)(void))fuzz_autoext_ok
                        : (void (*)(void))fuzz_autoext_err;
                    (void)sqlite3_cancel_auto_extension(fn);
                }
                break;
            }
            case 4: {
                if (!is_initialized && sqlite3_initialize() == SQLITE_OK) {
                    is_initialized = 1;
                }
                if (is_initialized) {
                    sqlite3_reset_auto_extension();
                }
                break;
            }
            case 5: {
                int onoff = (off < Size) ? (Data[off++] & 1) : 0;
                if (!is_initialized) {
                    (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, onoff);
                }
                break;
            }
            case 6: {
                int onoff = (off < Size) ? (Data[off++] & 1) : 0;
                if (!is_initialized) {
                    (void)sqlite3_config(SQLITE_CONFIG_URI, onoff);
                }
                break;
            }
            case 7: {
                int onoff = (off < Size) ? (Data[off++] & 1) : 0;
                if (!is_initialized) {
                    (void)sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, onoff);
                }
                break;
            }
            case 8: {
                if (!is_initialized) {
                    uint32_t n = 0;
                    if (read_u32(Data, Size, &off, &n)) {
                        (void)sqlite3_config(SQLITE_CONFIG_SORTERREF_SIZE, (int)(n % 65536));
                    }
                }
                break;
            }
            case 9: {
                if (!is_initialized) {
                    uint32_t n = 0;
                    if (read_u32(Data, Size, &off, &n)) {
                        (void)sqlite3_config(SQLITE_CONFIG_LOOKASIDE, 0, (int)(n % 4096), (int)(n % 64));
                    }
                }
                break;
            }
            case 10: {
                sqlite3 *db = NULL;
                int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
                if (op & 1) flags |= SQLITE_OPEN_URI;
                if (!is_initialized && sqlite3_initialize() == SQLITE_OK) {
                    is_initialized = 1;
                }
                (void)sqlite3_open_v2("./dummy_file", &db, flags, NULL);
                if (db) {
                    sqlite3_close(db);
                }
                break;
            }
            case 11: {
                if (!is_initialized) {
                    uint32_t badopt = 0;
                    if (read_u32(Data, Size, &off, &badopt)) {
                        (void)sqlite3_config((int)(badopt | 0x40000000));
                    }
                }
                break;
            }
        }
    }

    if (is_initialized) {
        sqlite3_reset_auto_extension();
        sqlite3_shutdown();
    }
    return 0;
}