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
#include <string.h>
#include "sqlite3.h"

static sqlite3_int64 read_i64(const uint8_t *p) {
    sqlite3_int64 v = 0;
    size_t i;
    for (i = 0; i < 8; i++) {
        v = (v << 8) | (sqlite3_int64)p[i];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    int reset1 = 0;
    int reset2 = 0;
    int current = 0;
    int highwater = 0;
    int rc = SQLITE_OK;
    int op = SQLITE_STATUS_MEMORY_USED;
    sqlite3_int64 hard_arg = -1;
    sqlite3_int64 soft_arg = -1;
    sqlite3_int64 old_hard = -1;
    sqlite3_int64 old_soft = -1;
    sqlite3_int64 prev_high = 0;
    sqlite3_int64 used1 = 0;
    sqlite3_int64 used2 = 0;

    if (Size > 0) reset1 = Data[0] & 1;
    if (Size > 1) reset2 = Data[1] & 1;
    if (Size > 2) {
        static const int ops[] = {
            SQLITE_STATUS_MEMORY_USED,
            SQLITE_STATUS_PAGECACHE_USED,
            SQLITE_STATUS_PAGECACHE_OVERFLOW,
            SQLITE_STATUS_SCRATCH_USED,
            SQLITE_STATUS_SCRATCH_OVERFLOW,
            SQLITE_STATUS_MALLOC_SIZE,
            SQLITE_STATUS_PARSER_STACK,
            SQLITE_STATUS_MALLOC_COUNT
        };
        op = ops[Data[2] % (sizeof(ops) / sizeof(ops[0]))];
    }

    if (Size >= 11) {
        hard_arg = read_i64(Data + 3);
        if (hard_arg < 0) hard_arg = -1;
        else hard_arg &= 0x0000FFFFFFFFFFFFLL;
    } else if (Size > 3) {
        hard_arg = (sqlite3_int64)(Data[3] % 4 == 0 ? -1 : (Data[3] % 3 == 0 ? 0 : (sqlite3_int64)(Data[3]) * 1024));
    }

    if (Size >= 19) {
        soft_arg = read_i64(Data + 11);
        if (soft_arg < 0) soft_arg = -1;
        else soft_arg &= 0x0000FFFFFFFFFFFFLL;
    } else if (Size > 11) {
        soft_arg = (sqlite3_int64)(Data[11] % 4 == 0 ? -1 : (Data[11] % 3 == 0 ? 0 : (sqlite3_int64)(Data[11]) * 2048));
    }

    prev_high = sqlite3_memory_highwater(reset1);
    (void)prev_high;

    used1 = sqlite3_memory_used();
    (void)used1;

    rc = sqlite3_status(op, &current, &highwater, reset2);
    (void)rc;
    (void)current;
    (void)highwater;

    used2 = sqlite3_memory_used();
    (void)used2;

    old_hard = sqlite3_hard_heap_limit64(hard_arg);
    (void)old_hard;

    old_soft = sqlite3_soft_heap_limit64(soft_arg);
    (void)old_soft;

    return 0;
}