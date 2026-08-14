// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_os_end at sqlite3.c:29823:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_thread_cleanup at sqlite3.c:155922:17 in sqlite3.h
// sqlite3_os_end at sqlite3.c:29823:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **Data, size_t *Size) {
    uint32_t v = 0;
    size_t n = *Size < 4 ? *Size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*Data)[i];
    }
    *Data += n;
    *Size -= n;
    return v;
}

static int read_bool(const uint8_t **Data, size_t *Size) {
    if (*Size == 0) return 0;
    int v = (**Data) & 1;
    (*Data)++;
    (*Size)--;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    int steps = 1;
    if (remaining > 0) {
        steps = (int)(read_u32(&p, &remaining) % 32U) + 1;
    }

    for (int i = 0; i < steps; ++i) {
        int op = 0;
        if (remaining > 0) {
            op = (int)(read_u32(&p, &remaining) % 10U);
        }

        switch (op) {
            case 0:
                (void)sqlite3_shutdown();
                break;

            case 1:
                (void)sqlite3_initialize();
                break;

            case 2: {
                int onoff = read_bool(&p, &remaining);
                (void)sqlite3_enable_shared_cache(onoff);
                break;
            }

            case 3:
                sqlite3_thread_cleanup();
                break;

            case 4:
                (void)sqlite3_os_end();
                break;

            case 5:
                (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                break;

            case 6:
                (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                break;

            case 7:
                (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                break;

            case 8: {
                int bogus = (int)(read_u32(&p, &remaining) | 0x40000000U);
                (void)sqlite3_config(bogus);
                break;
            }

            case 9: {
                if (read_bool(&p, &remaining)) {
                    (void)sqlite3_shutdown();
                    (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                    (void)sqlite3_initialize();
                } else {
                    (void)sqlite3_initialize();
                    (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                    (void)sqlite3_shutdown();
                }
                break;
            }

            default:
                break;
        }
    }

    sqlite3_thread_cleanup();
    (void)sqlite3_shutdown();
    (void)sqlite3_os_end();

    return 0;
}