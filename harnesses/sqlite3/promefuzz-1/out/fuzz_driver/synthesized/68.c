// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; i++) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static int read_int(const uint8_t **p, size_t *n) {
    return (int)read_u32(p, n);
}

static char *make_cstring(const uint8_t *data, size_t size) {
    char *s = (char *)malloc(size + 1);
    if (!s) return NULL;
    if (size) memcpy(s, data, size);
    s[size] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput_68(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    char *user_opt = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    (void)sqlite3_libversion_number();

    {
        const char *sid = sqlite3_sourceid();
        if (sid) {
            volatile size_t sid_len = strlen(sid);
            (void)sid_len;
        }
    }

    (void)sqlite3_threadsafe();

    {
        int idx1 = read_int(&p, &n);
        int idx2 = read_int(&p, &n);
        int idx3 = read_int(&p, &n);

        const char *opt;

        opt = sqlite3_compileoption_get(idx1);
        if (opt) {
            volatile size_t opt_len = strlen(opt);
            (void)opt_len;
            (void)sqlite3_compileoption_used(opt);
        }

        opt = sqlite3_compileoption_get(idx2);
        if (opt) {
            volatile size_t opt_len = strlen(opt);
            (void)opt_len;
            (void)sqlite3_compileoption_used(opt);
        }

        opt = sqlite3_compileoption_get(idx3);
        if (opt) {
            volatile size_t opt_len = strlen(opt);
            (void)opt_len;
            (void)sqlite3_compileoption_used(opt);
        }
    }

    user_opt = make_cstring(p, n);
    if (user_opt) {
        (void)sqlite3_compileoption_used(user_opt);
    }
    (void)sqlite3_compileoption_used(NULL);
    (void)sqlite3_compileoption_get(-1);

    /*
     * Restrict sqlite3_test_control() to opcodes with safe, fixed signatures.
     * Do not use SQLITE_TESTCTRL_ASSERT: it expects a non-zero int and asserts
     * internally on zero, which can intentionally abort the process.
     */
    {
        int selector = read_int(&p, &n);
        int a1 = read_int(&p, &n);
        int rc = 0;

        switch ((unsigned)selector % 3U) {
            case 0:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
                break;
            case 1:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
                break;
            case 2:
            default:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, a1);
                break;
        }
        (void)rc;
    }

    free(user_opt);
    return 0;
}