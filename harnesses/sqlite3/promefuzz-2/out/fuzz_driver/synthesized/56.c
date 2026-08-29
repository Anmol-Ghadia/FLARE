// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_os_init at sqlite3.c:29704:16 in sqlite3.h
// sqlite3_os_end at sqlite3.c:29823:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_complete16 at sqlite3.c:152128:16 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_complete16 at sqlite3.c:152128:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_os_init at sqlite3.c:29704:16 in sqlite3.h
// sqlite3_os_end at sqlite3.c:29823:16 in sqlite3.h
// sqlite3_complete16 at sqlite3.c:152128:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *Offset < Size; ++i, ++(*Offset)) {
        v |= ((uint32_t)Data[*Offset]) << (i * 8);
    }
    return v;
}

static int read_int_range(const uint8_t *Data, size_t Size, size_t *Offset, int minv, int maxv) {
    uint32_t span;
    uint32_t v;
    if (minv >= maxv) return minv;
    span = (uint32_t)(maxv - minv + 1);
    v = read_u32(Data, Size, Offset);
    return minv + (int)(v % span);
}

static void *make_utf16_native_nul_terminated(const uint8_t *Src, size_t SrcAvail, size_t LenBytes) {
    size_t copy_len = LenBytes;
    unsigned char *buf;

    if (copy_len > SrcAvail) copy_len = SrcAvail;
    if (copy_len & 1) copy_len--;

    /* Allocate 4 trailing zero bytes so sqlite3_complete16() can safely
       inspect z[nByte] and z[nByte+1] even when nByte reaches the limit. */
    buf = (unsigned char *)malloc(copy_len + 4);
    if (!buf) return NULL;

    if (copy_len > 0) {
        memcpy(buf, Src, copy_len);
    }
    buf[copy_len] = 0;
    buf[copy_len + 1] = 0;
    buf[copy_len + 2] = 0;
    buf[copy_len + 3] = 0;
    return buf;
}

int LLVMFuzzerTestOneInput_56(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    int i;
    int rounds;
    int rc;
    void *utf16_sql = NULL;
    unsigned char *randbuf = NULL;

    if (!Data) return 0;

    rounds = read_int_range(Data, Size, &offset, 1, 16);

    for (i = 0; i < rounds; ++i) {
        int action = read_int_range(Data, Size, &offset, 0, 9);

        switch (action) {
            case 0:
                (void)sqlite3_initialize();
                break;

            case 1:
                (void)sqlite3_shutdown();
                break;

            case 2:
                (void)sqlite3_os_init();
                break;

            case 3:
                (void)sqlite3_os_end();
                break;

            case 4:
            {
                size_t remaining = (offset < Size) ? (Size - offset) : 0;
                size_t use_len = remaining;
                if (remaining > 0) {
                    size_t limit = (size_t)read_int_range(Data, Size, &offset, 0, (int)remaining);
                    if (limit < use_len) use_len = limit;
                }
                if (use_len > ((offset < Size) ? (Size - offset) : 0)) {
                    use_len = (offset < Size) ? (Size - offset) : 0;
                }
                if (use_len & 1) use_len--;

                utf16_sql = make_utf16_native_nul_terminated(
                    Data + offset,
                    (offset < Size) ? (Size - offset) : 0,
                    use_len
                );
                if (utf16_sql) {
                    rc = sqlite3_complete16(utf16_sql);
                    (void)rc;
                    free(utf16_sql);
                    utf16_sql = NULL;
                }
                if (offset + use_len <= Size) {
                    offset += use_len;
                } else {
                    offset = Size;
                }
                break;
            }

            case 5:
            {
                int n = read_int_range(Data, Size, &offset, -16, 4096);
                if (n > 0) {
                    randbuf = (unsigned char *)malloc((size_t)n);
                    if (randbuf) {
                        sqlite3_randomness(n, randbuf);
                        free(randbuf);
                        randbuf = NULL;
                    } else {
                        sqlite3_randomness(n, NULL);
                    }
                } else {
                    sqlite3_randomness(n, NULL);
                }
                break;
            }

            case 6:
            {
                int n = read_int_range(Data, Size, &offset, 0, 4096);
                randbuf = (unsigned char *)malloc((size_t)n);
                if (n == 0) {
                    sqlite3_randomness(0, randbuf);
                } else if (randbuf) {
                    sqlite3_randomness(n, randbuf);
                } else {
                    sqlite3_randomness(n, NULL);
                }
                free(randbuf);
                randbuf = NULL;
                break;
            }

            case 7:
            {
                (void)sqlite3_initialize();
                (void)sqlite3_os_init();
                (void)sqlite3_os_end();
                (void)sqlite3_shutdown();
                break;
            }

            case 8:
            {
                size_t remaining = (offset < Size) ? (Size - offset) : 0;
                size_t use_len = remaining > 64 ? 64 : remaining;
                if (use_len & 1) use_len--;

                utf16_sql = make_utf16_native_nul_terminated(
                    Data + offset,
                    remaining,
                    use_len
                );
                if (utf16_sql) {
                    (void)sqlite3_initialize();
                    rc = sqlite3_complete16(utf16_sql);
                    (void)rc;
                    sqlite3_randomness((int)(use_len > 0 ? use_len : 1), utf16_sql);
                    ((unsigned char *)utf16_sql)[use_len] = 0;
                    ((unsigned char *)utf16_sql)[use_len + 1] = 0;
                    ((unsigned char *)utf16_sql)[use_len + 2] = 0;
                    ((unsigned char *)utf16_sql)[use_len + 3] = 0;
                    rc = sqlite3_complete16(utf16_sql);
                    (void)rc;
                    free(utf16_sql);
                    utf16_sql = NULL;
                }
                if (offset + use_len <= Size) {
                    offset += use_len;
                } else {
                    offset = Size;
                }
                break;
            }

            case 9:
            default:
                (void)sqlite3_initialize();
                (void)sqlite3_initialize();
                (void)sqlite3_shutdown();
                (void)sqlite3_shutdown();
                break;
        }
    }

    (void)sqlite3_shutdown();
    return 0;
}