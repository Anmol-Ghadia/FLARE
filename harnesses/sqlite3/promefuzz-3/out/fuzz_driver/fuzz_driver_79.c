// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_soft_heap_limit at sqlite3.c:15556:17 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_soft_heap_limit at sqlite3.c:15556:17 in sqlite3.h
// sqlite3_release_memory at sqlite3.c:15459:16 in sqlite3.h
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
    for (i = 0; i < 4; i++) {
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    int rc;
    int current = 0;
    int highwater = 0;
    int resetFlag;
    int op;
    int n;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_initialize();
    (void)rc;

    if (Size == 0) {
        (void)sqlite3_status(SQLITE_STATUS_MEMORY_USED, &current, &highwater, 0);
        (void)sqlite3_soft_heap_limit(0);
        (void)sqlite3_release_memory(0);
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
        (void)sqlite3_shutdown();
        return 0;
    }

    for (int i = 0; i < 16 && off < Size; i++) {
        uint8_t choice = Data[off++];

        switch (choice % 8) {
            case 0:
                (void)sqlite3_initialize();
                break;

            case 1:
                (void)sqlite3_shutdown();
                (void)sqlite3_initialize();
                break;

            case 2:
                n = read_int(Data, Size, &off);
                sqlite3_soft_heap_limit(n);
                break;

            case 3:
                n = read_int(Data, Size, &off);
                (void)sqlite3_release_memory(n);
                break;

            case 4: {
                static const int status_ops[] = {
#ifdef SQLITE_STATUS_MEMORY_USED
                    SQLITE_STATUS_MEMORY_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_USED
                    SQLITE_STATUS_PAGECACHE_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_OVERFLOW
                    SQLITE_STATUS_PAGECACHE_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_SCRATCH_USED
                    SQLITE_STATUS_SCRATCH_USED,
#endif
#ifdef SQLITE_STATUS_SCRATCH_OVERFLOW
                    SQLITE_STATUS_SCRATCH_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_MALLOC_SIZE
                    SQLITE_STATUS_MALLOC_SIZE,
#endif
#ifdef SQLITE_STATUS_PARSER_STACK
                    SQLITE_STATUS_PARSER_STACK,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_SIZE
                    SQLITE_STATUS_PAGECACHE_SIZE,
#endif
#ifdef SQLITE_STATUS_SCRATCH_SIZE
                    SQLITE_STATUS_SCRATCH_SIZE,
#endif
#ifdef SQLITE_STATUS_MALLOC_COUNT
                    SQLITE_STATUS_MALLOC_COUNT,
#endif
                };
                size_t num_ops = sizeof(status_ops) / sizeof(status_ops[0]);
                if (num_ops > 0) {
                    op = status_ops[choice % num_ops];
                    resetFlag = (int)(choice & 1);
                    (void)sqlite3_status(op, &current, &highwater, resetFlag);
                }
                break;
            }

            case 5:
                resetFlag = (int)(choice & 1);
#ifdef SQLITE_STATUS_MEMORY_USED
                (void)sqlite3_status(SQLITE_STATUS_MEMORY_USED, &current, &highwater, resetFlag);
#endif
                break;

            case 6:
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
                (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
                (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESET
                (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
#endif
                break;

            case 7:
                op = read_int(Data, Size, &off);
                switch ((unsigned)op % 6U) {
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
                    case 0:
                        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
                        break;
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
                    case 1:
                        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
                        break;
#endif
#ifdef SQLITE_TESTCTRL_PRNG_RESET
                    case 2:
                        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
                        break;
#endif
#ifdef SQLITE_TESTCTRL_FAULT_INSTALL
                    case 3:
                        (void)sqlite3_test_control(SQLITE_TESTCTRL_FAULT_INSTALL, 0, 0);
                        break;
#endif
#ifdef SQLITE_TESTCTRL_BENIGN_MALLOC_HOOKS
                    case 4:
                        (void)sqlite3_test_control(SQLITE_TESTCTRL_BENIGN_MALLOC_HOOKS, 0, 0);
                        break;
#endif
                    default:
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
                        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
                        break;
                }
                break;
        }
    }

    (void)sqlite3_release_memory(0);
    (void)sqlite3_shutdown();
    return 0;
}