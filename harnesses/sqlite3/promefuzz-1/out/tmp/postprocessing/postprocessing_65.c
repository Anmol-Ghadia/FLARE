#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; i++) {
        v |= ((uint32_t)(*p)[i]) << (8 * i);
    }
    *p += take;
    *n -= take;
    return v;
}

static int read_i32(const uint8_t **p, size_t *n) {
    return (int)read_u32(p, n);
}

static sqlite3_int64 read_i64(const uint8_t **p, size_t *n) {
    sqlite3_int64 v = 0;
    size_t take = *n < 8 ? *n : 8;
    for (size_t i = 0; i < take; i++) {
        v |= ((sqlite3_int64)(*p)[i]) << (8 * i);
    }
    *p += take;
    *n -= take;
    return v;
}

static void alarm_cb(void *arg, sqlite3_int64 used, int n) {
    volatile unsigned char *p = (volatile unsigned char *)arg;
    if (p) {
        *p ^= (unsigned char)((used ^ (sqlite3_int64)n) & 0xFF);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    volatile unsigned char alarm_state = 0;

    int valid_ops[] = {
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

    size_t valid_ops_count = sizeof(valid_ops) / sizeof(valid_ops[0]);

    (void)sqlite3_memory_used();
    (void)sqlite3_memory_highwater(0);
    (void)sqlite3_memory_highwater(1);

    (void)sqlite3_memory_alarm(alarm_cb, (void *)&alarm_state, (sqlite3_int64)0);
    (void)sqlite3_memory_alarm(NULL, NULL, read_i64(&p, &n));

    for (int iter = 0; iter < 32 && n > 0; iter++) {
        uint32_t action = read_u32(&p, &n) % 8;

        switch (action) {
            case 0: {
                int current = 0;
                int highwater = 0;
                int op;
                if (valid_ops_count > 0 && (read_u32(&p, &n) & 1U)) {
                    op = valid_ops[read_u32(&p, &n) % valid_ops_count];
                } else {
                    op = read_i32(&p, &n);
                }
                int resetFlag = read_i32(&p, &n);

                int *pc = (read_u32(&p, &n) & 1U) ? &current : NULL;
                int *ph = (read_u32(&p, &n) & 1U) ? &highwater : NULL;

                (void)sqlite3_status(op, pc, ph, resetFlag);
                break;
            }

            case 1: {
                sqlite3_int64 current64 = 0;
                sqlite3_int64 highwater64 = 0;
                int op;
                if (valid_ops_count > 0 && (read_u32(&p, &n) & 1U)) {
                    op = valid_ops[read_u32(&p, &n) % valid_ops_count];
                } else {
                    op = read_i32(&p, &n);
                }
                int resetFlag = read_i32(&p, &n);

                sqlite3_int64 *pc = (read_u32(&p, &n) & 1U) ? &current64 : NULL;
                sqlite3_int64 *ph = (read_u32(&p, &n) & 1U) ? &highwater64 : NULL;

                (void)sqlite3_status64(op, pc, ph, resetFlag);
                break;
            }

            case 2: {
                (void)sqlite3_memory_used();
                break;
            }

            case 3: {
                int resetFlag = read_i32(&p, &n);
                (void)sqlite3_memory_highwater(resetFlag);
                break;
            }

            case 4: {
                int amount = read_i32(&p, &n);
                (void)sqlite3_release_memory(amount);
                break;
            }

            case 5: {
                sqlite3_int64 threshold = read_i64(&p, &n);
                if (read_u32(&p, &n) & 1U) {
                    (void)sqlite3_memory_alarm(alarm_cb, (void *)&alarm_state, threshold);
                } else {
                    (void)sqlite3_memory_alarm(NULL, NULL, threshold);
                }
                break;
            }

            case 6: {
                int current = 0, highwater = 0;
                sqlite3_int64 current64 = 0, highwater64 = 0;
                int op = valid_ops_count > 0 ? valid_ops[read_u32(&p, &n) % valid_ops_count] : 0;
                int resetFlag = read_i32(&p, &n);

                (void)sqlite3_status(op, &current, &highwater, resetFlag);
                (void)sqlite3_status64(op, &current64, &highwater64, resetFlag);
                (void)sqlite3_memory_used();
                (void)sqlite3_memory_highwater(resetFlag & 1);
                break;
            }

            case 7: {
                int amt1 = read_i32(&p, &n);
                int amt2 = read_i32(&p, &n);
                (void)sqlite3_release_memory(amt1);
                (void)sqlite3_release_memory(amt2);
                (void)sqlite3_memory_used();
                break;
            }
        }
    }

    (void)sqlite3_memory_alarm(NULL, NULL, 0);
    return 0;
}