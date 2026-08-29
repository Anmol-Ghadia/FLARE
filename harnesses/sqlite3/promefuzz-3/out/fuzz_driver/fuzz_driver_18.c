// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sqlite3.h>

static sqlite3_int64 read_i64_le(const uint8_t *p, size_t n) {
    sqlite3_int64 v = 0;
    size_t i;
    size_t limit = n < 8 ? n : 8;
    for (i = 0; i < limit; ++i) {
        v |= ((sqlite3_int64)p[i]) << (8 * i);
    }
    if (limit == 8 && (p[7] & 0x80)) {
        v = (sqlite3_int64)(int64_t)v;
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    int reset1 = 0;
    int reset2 = 0;
    int current = 0;
    int highwater = 0;
    int rc;
    int op;
    sqlite3_int64 hard_arg;
    sqlite3_int64 soft_arg;
    sqlite3_int64 prev_hard;
    sqlite3_int64 prev_soft;
    sqlite3_int64 mhw1;
    sqlite3_int64 mu1;
    sqlite3_int64 mu2;

    if (Data == NULL) {
        return 0;
    }

    reset1 = (Size > 0) ? (Data[0] & 1) : 0;
    reset2 = (Size > 1) ? (Data[1] & 1) : 0;

    {
        static const int ops[] = {
            SQLITE_STATUS_MEMORY_USED,
            SQLITE_STATUS_PAGECACHE_USED,
            SQLITE_STATUS_PAGECACHE_OVERFLOW,
            SQLITE_STATUS_SCRATCH_USED,
            SQLITE_STATUS_SCRATCH_OVERFLOW,
            SQLITE_STATUS_MALLOC_SIZE,
            SQLITE_STATUS_PARSER_STACK,
            SQLITE_STATUS_PAGECACHE_SIZE,
            SQLITE_STATUS_SCRATCH_SIZE,
            SQLITE_STATUS_MALLOC_COUNT
        };
        size_t idx = 0;
        if (Size > 2) {
            idx = (size_t)Data[2] % (sizeof(ops) / sizeof(ops[0]));
        }
        op = ops[idx];
    }

    hard_arg = (Size > 3) ? read_i64_le(Data + 3, Size - 3) : -1;
    soft_arg = (Size > 11) ? read_i64_le(Data + 11, Size - 11) : -1;

    if (Size > 19) {
        switch (Data[19] % 4) {
            case 0:
                hard_arg = -1;
                break;
            case 1:
                hard_arg = 0;
                break;
            case 2:
                hard_arg = hard_arg < 0 ? -hard_arg : hard_arg;
                break;
            default:
                break;
        }
    }

    if (Size > 20) {
        switch (Data[20] % 4) {
            case 0:
                soft_arg = -1;
                break;
            case 1:
                soft_arg = 0;
                break;
            case 2:
                soft_arg = soft_arg < 0 ? -soft_arg : soft_arg;
                break;
            default:
                break;
        }
    }

    mhw1 = sqlite3_memory_highwater(reset1);
    (void)mhw1;

    mu1 = sqlite3_memory_used();
    (void)mu1;

    rc = sqlite3_status(op, &current, &highwater, reset2);
    (void)rc;
    (void)current;
    (void)highwater;

    mu2 = sqlite3_memory_used();
    (void)mu2;

    prev_hard = sqlite3_hard_heap_limit64(hard_arg);
    (void)prev_hard;

    prev_soft = sqlite3_soft_heap_limit64(soft_arg);
    (void)prev_soft;

    return 0;
}