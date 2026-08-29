// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_status64 at sqlite3.c:9561:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_memory_highwater at sqlite3.c:15644:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_soft_heap_limit64 at sqlite3.c:15531:26 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static uint64_t read_u64(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        v <<= 8;
        if (*Offset < Size) {
            v |= Data[*Offset];
            (*Offset)++;
        }
    }
    return v;
}

static int read_i32(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*Offset < Size) {
            v |= Data[*Offset];
            (*Offset)++;
        }
    }
    return (int)v;
}

static sqlite3_int64 read_i64(const uint8_t *Data, size_t Size, size_t *Offset) {
    return (sqlite3_int64)read_u64(Data, Size, Offset);
}

static int choose_status_op(int selector) {
    static const int ops[] = {
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
    const int n = (int)(sizeof(ops) / sizeof(ops[0]));
    if (n == 0) {
        return selector;
    }
    if (selector < 0) {
        selector = -selector;
    }
    return ops[selector % n];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    sqlite3_int64 cur_mem;
    sqlite3_int64 high_mem;
    sqlite3_int64 prev_soft;
    sqlite3_int64 prev_hard;
    sqlite3_int64 current_soft;
    sqlite3_int64 current_hard;
    int cur32 = 0;
    int high32 = 0;
    sqlite3_int64 cur64 = 0;
    sqlite3_int64 high64 = 0;
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    (void)sqlite3_initialize();

    cur_mem = sqlite3_memory_used();
    high_mem = sqlite3_memory_highwater(0);

    prev_soft = sqlite3_soft_heap_limit64(-1);
    prev_hard = sqlite3_hard_heap_limit64(-1);

    for (i = 0; i < 8; ++i) {
        int action = (off < Size) ? (Data[off++] % 8) : i;
        int reset = (off < Size) ? (Data[off++] & 1) : 0;

        switch (action) {
            case 0: {
                sqlite3_int64 n = read_i64(Data, Size, &off);
                (void)sqlite3_soft_heap_limit64(n);
                break;
            }
            case 1: {
                sqlite3_int64 n = read_i64(Data, Size, &off);
                if (n < 0) {
                    n = -1;
                } else {
                    n &= 0x7fffffff;
                }
                (void)sqlite3_hard_heap_limit64(n);
                break;
            }
            case 2:
                (void)sqlite3_memory_used();
                break;
            case 3:
                (void)sqlite3_memory_highwater(reset);
                break;
            case 4: {
                int op = choose_status_op(read_i32(Data, Size, &off));
                (void)sqlite3_status(op, &cur32, &high32, reset);
                break;
            }
            case 5: {
                int op = choose_status_op(read_i32(Data, Size, &off));
                (void)sqlite3_status64(op, &cur64, &high64, reset);
                break;
            }
            case 6: {
                int invalid_op = read_i32(Data, Size, &off);
                (void)sqlite3_status(invalid_op, &cur32, &high32, reset);
                (void)sqlite3_status64(invalid_op, &cur64, &high64, reset);
                break;
            }
            case 7: {
                (void)sqlite3_soft_heap_limit64(-1);
                (void)sqlite3_hard_heap_limit64(-1);
                (void)sqlite3_memory_used();
                (void)sqlite3_memory_highwater(reset);
                break;
            }
            default:
                break;
        }
    }

    current_soft = sqlite3_soft_heap_limit64(-1);
    current_hard = sqlite3_hard_heap_limit64(-1);
    (void)current_soft;
    (void)current_hard;

    (void)sqlite3_status(choose_status_op(0), &cur32, &high32, 0);
    (void)sqlite3_status64(choose_status_op(1), &cur64, &high64, 0);

    (void)sqlite3_memory_used();
    (void)sqlite3_memory_highwater(1);

    if (prev_hard >= 0) {
        (void)sqlite3_hard_heap_limit64(prev_hard);
    } else {
        (void)sqlite3_hard_heap_limit64(-1);
    }
    (void)sqlite3_soft_heap_limit64(prev_soft);

    return 0;
}