#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

static const char *make_cstring(const uint8_t *data, size_t size) {
    char *s = (char *)malloc(size + 1);
    if (!s) return NULL;
    if (size) memcpy(s, data, size);
    s[size] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

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

    int idx1 = read_int(&p, &n);
    int idx2 = read_int(&p, &n);
    int idx3 = read_int(&p, &n);

    {
        const char *opt = sqlite3_compileoption_get(idx1);
        if (opt) {
            volatile size_t opt_len = strlen(opt);
            (void)opt_len;
            (void)sqlite3_compileoption_used(opt);
        }
    }

    {
        const char *opt = sqlite3_compileoption_get(idx2);
        if (opt) {
            volatile size_t opt_len = strlen(opt);
            (void)opt_len;
            (void)sqlite3_compileoption_used(opt);
        }
    }

    {
        const char *opt = sqlite3_compileoption_get(idx3);
        if (opt) {
            volatile size_t opt_len = strlen(opt);
            (void)opt_len;
            (void)sqlite3_compileoption_used(opt);
        }
    }

    {
        const char *user_opt = make_cstring(p, n);
        if (user_opt) {
            (void)sqlite3_compileoption_used(user_opt);
            free((void *)user_opt);
        } else {
            (void)sqlite3_compileoption_used(NULL);
        }
    }

    (void)sqlite3_compileoption_get(-1);
    (void)sqlite3_compileoption_used(NULL);

    {
        int op = read_int(&p, &n);
        int a1 = read_int(&p, &n);
        int a2 = read_int(&p, &n);
        sqlite3_uint64 u1 = (sqlite3_uint64)read_u32(&p, &n);
        int rc;

        switch ((unsigned)op % 8U) {
            case 0:
                rc = sqlite3_test_control(op);
                (void)rc;
                break;
            case 1:
                rc = sqlite3_test_control(op, a1);
                (void)rc;
                break;
            case 2:
                rc = sqlite3_test_control(op, a1, a2);
                (void)rc;
                break;
            case 3:
                rc = sqlite3_test_control(op, (sqlite3_uint64)u1);
                (void)rc;
                break;
            case 4:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
                (void)rc;
                break;
            case 5:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SEED, a1, (const char *)Data);
                (void)rc;
                break;
            case 6:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
                (void)rc;
                break;
            case 7:
            default:
                rc = sqlite3_test_control(SQLITE_TESTCTRL_FAULT_INSTALL, 0);
                (void)rc;
                break;
        }
    }

    return 0;
}