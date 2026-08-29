// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_os_end at sqlite3.c:29823:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_global_recover at sqlite3.c:155846:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_global_recover at sqlite3.c:155846:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*Off < Size) {
            v |= Data[*Off];
            (*Off)++;
        }
    }
    return v;
}

static int read_int(const uint8_t *Data, size_t Size, size_t *Off) {
    return (int)read_u32(Data, Size, Off);
}

int LLVMFuzzerTestOneInput_52(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    FILE *fp;
    int i, steps;
    int initialized = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    steps = (Size > 0) ? (int)(Data[off++] % 32) + 1 : 8;

    for (i = 0; i < steps; ++i) {
        int op = (off < Size) ? (Data[off++] % 8) : (i % 8);
        int rc;

        switch (op) {
            case 0: {
                int enable = read_int(Data, Size, &off);
                rc = sqlite3_enable_shared_cache(enable);
                (void)rc;
                break;
            }

            case 1: {
                rc = sqlite3_global_recover();
                (void)rc;
                break;
            }

            case 2: {
                int cfg = read_int(Data, Size, &off);
                int a1 = read_int(Data, Size, &off);
                int a2 = read_int(Data, Size, &off);
                sqlite3_int64 a3 = (sqlite3_int64)read_u32(Data, Size, &off);

                switch ((unsigned)cfg % 10U) {
                    case 0:
                        rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                        break;
                    case 1:
                        rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                        break;
                    case 2:
                        rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                        break;
                    case 3:
                        rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, a1 & 1);
                        break;
                    case 4:
                        rc = sqlite3_config(SQLITE_CONFIG_SMALL_MALLOC, a1 & 1);
                        break;
                    case 5:
                        rc = sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, a1 & 1);
                        break;
                    case 6:
                        rc = sqlite3_config(SQLITE_CONFIG_SORTERREF_SIZE, a3);
                        break;
                    case 7:
                        rc = sqlite3_config(SQLITE_CONFIG_STMTJRNL_SPILL, a1);
                        break;
                    case 8:
                        rc = sqlite3_config(SQLITE_CONFIG_URI, a1 & 1);
                        break;
                    default:
                        rc = sqlite3_config(-1, a1, a2);
                        break;
                }
                (void)rc;
                break;
            }

            case 3: {
                rc = sqlite3_initialize();
                if (rc == SQLITE_OK) {
                    initialized = 1;
                }
                break;
            }

            case 4: {
                rc = sqlite3_os_end();
                (void)rc;
                break;
            }

            case 5: {
                rc = sqlite3_shutdown();
                if (rc == SQLITE_OK) {
                    initialized = 0;
                }
                break;
            }

            case 6: {
                rc = sqlite3_shutdown();
                (void)rc;
                rc = sqlite3_initialize();
                if (rc == SQLITE_OK) {
                    initialized = 1;
                }
                break;
            }

            case 7: {
                int enable = (off < Size) ? (Data[off++] & 1) : 0;
                rc = sqlite3_enable_shared_cache(enable);
                (void)rc;
                rc = sqlite3_initialize();
                if (rc == SQLITE_OK) {
                    initialized = 1;
                }
                rc = sqlite3_global_recover();
                (void)rc;
                break;
            }

            default:
                break;
        }
    }

    if (initialized) {
        sqlite3_shutdown();
    } else {
        sqlite3_shutdown();
    }

    return 0;
}