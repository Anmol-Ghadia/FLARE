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

static int read_bool(const uint8_t *Data, size_t Size, size_t *Off) {
    if (*Off >= Size) return 0;
    return (int)(Data[(*Off)++] & 1U);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    (void)sqlite3_shutdown();

    {
        int pre_cfg_choice = (int)(read_u32(Data, Size, &off) % 6U);
        switch (pre_cfg_choice) {
            case 0:
                (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                break;
            case 1:
                (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                break;
            case 2:
                (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                break;
            case 3:
                (void)sqlite3_config(-1);
                break;
            case 4:
                (void)sqlite3_config(0);
                break;
            default:
                (void)sqlite3_config(999999);
                break;
        }
    }

    {
        int shared = read_bool(Data, Size, &off);
        (void)sqlite3_enable_shared_cache(shared);
    }

    {
        int init_first = read_bool(Data, Size, &off);
        if (init_first) {
            (void)sqlite3_initialize();
        } else {
            (void)sqlite3_global_recover();
        }
    }

    {
        uint32_t iterations = (read_u32(Data, Size, &off) % 32U) + 1U;
        uint32_t i;
        for (i = 0; i < iterations; ++i) {
            uint32_t op = read_u32(Data, Size, &off) % 8U;
            switch (op) {
                case 0:
                    (void)sqlite3_initialize();
                    break;
                case 1:
                    (void)sqlite3_shutdown();
                    break;
                case 2:
                    (void)sqlite3_os_end();
                    break;
                case 3:
                    (void)sqlite3_global_recover();
                    break;
                case 4:
                    (void)sqlite3_enable_shared_cache(read_bool(Data, Size, &off));
                    break;
                case 5:
                    (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                    break;
                case 6:
                    (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                    break;
                case 7:
                default: {
                    uint32_t cfg_variant = read_u32(Data, Size, &off) % 5U;
                    switch (cfg_variant) {
                        case 0:
                            (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                            break;
                        case 1:
                            (void)sqlite3_config(-1);
                            break;
                        case 2:
                            (void)sqlite3_config(123456789);
                            break;
                        case 3:
                            (void)sqlite3_config(0);
                            break;
                        default:
                            (void)sqlite3_config((int)(read_u32(Data, Size, &off)));
                            break;
                    }
                    break;
                }
            }
        }
    }

    (void)sqlite3_shutdown();
    (void)sqlite3_os_end();

    return 0;
}