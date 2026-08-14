#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; i++) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size) {
    return (int)read_u32(data, size);
}

static unsigned int read_uint(const uint8_t **data, size_t *size) {
    return (unsigned int)read_u32(data, size);
}

static char *read_string(const uint8_t **data, size_t *size, size_t max_len) {
    if (*size == 0) {
        char *s = (char *)malloc(1);
        if (s) s[0] = '\0';
        return s;
    }

    size_t len = read_u32(data, size);
    if (len > *size) len = *size;
    if (len > max_len) len = max_len;

    char *s = (char *)malloc(len + 1);
    if (!s) return NULL;

    memcpy(s, *data, len);
    s[len] = '\0';
    *data += len;
    *size -= len;
    return s;
}

static int safe_keyword_len(const char *s, int n) {
    size_t slen;
    if (!s) return 0;
    slen = strlen(s);
    if (n < 0) return 0;
    if ((size_t)n > slen) return (int)slen;
    return n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    char *s1 = read_string(&p, &remaining, 4096);
    char *s2 = read_string(&p, &remaining, 4096);
    char *s3 = read_string(&p, &remaining, 4096);
    char *s4 = read_string(&p, &remaining, 4096);

    if (!s1 || !s2 || !s3 || !s4) {
        free(s1);
        free(s2);
        free(s3);
        free(s4);
        return 0;
    }

    int n1 = read_int(&p, &remaining);
    int n2 = read_int(&p, &remaining);
    unsigned int esc = read_uint(&p, &remaining);

    const char *maybe_null1 = (remaining > 0 && (p[0] & 1)) ? NULL : s1;
    const char *maybe_null2 = (remaining > 0 && (p[0] & 2)) ? NULL : s2;
    const char *maybe_null3 = (remaining > 0 && (p[0] & 4)) ? NULL : s3;
    const char *maybe_null4 = (remaining > 0 && (p[0] & 8)) ? NULL : s4;

    (void)sqlite3_strglob(s1, s2);
    (void)sqlite3_strglob(s2, s1);
    (void)sqlite3_strglob(s3, s4);

    (void)sqlite3_strlike(s1, s2, esc);
    (void)sqlite3_strlike(s2, s1, 0);
    (void)sqlite3_strlike(s3, s4, esc & 0xFFu);

    (void)sqlite3_stricmp(maybe_null1, maybe_null2);
    (void)sqlite3_stricmp(maybe_null2, maybe_null1);
    (void)sqlite3_stricmp(maybe_null3, maybe_null4);

    (void)sqlite3_strnicmp(maybe_null1, maybe_null2, n1);
    (void)sqlite3_strnicmp(maybe_null2, maybe_null1, n2);
    (void)sqlite3_strnicmp(maybe_null3, maybe_null4, n1 ^ n2);

    (void)sqlite3_keyword_check(s1, safe_keyword_len(s1, n1));
    (void)sqlite3_keyword_check(s2, safe_keyword_len(s2, n2));
    (void)sqlite3_keyword_check(s3, (int)strlen(s3));
    (void)sqlite3_keyword_check(s4, (int)strlen(s4));

    {
        const char *kw = NULL;
        int kwlen = 0;
        int idx1 = read_int(&p, &remaining);
        int idx2 = read_int(&p, &remaining);

        if (sqlite3_keyword_name(idx1, &kw, &kwlen) == SQLITE_OK && kw && kwlen >= 0) {
            (void)sqlite3_stricmp(kw, s1);
            (void)sqlite3_strnicmp(kw, s2, kwlen);
            (void)sqlite3_strglob(s3, kw);
            (void)sqlite3_strlike(s4, kw, esc & 0xFFu);
            (void)sqlite3_keyword_check(kw, kwlen);
        }

        kw = NULL;
        kwlen = 0;
        if (sqlite3_keyword_name(idx2, &kw, &kwlen) == SQLITE_OK && kw && kwlen >= 0) {
            (void)sqlite3_stricmp(s1, kw);
            (void)sqlite3_strnicmp(s2, kw, n1);
            (void)sqlite3_strglob(kw, s3);
            (void)sqlite3_strlike(kw, s4, 0);
            (void)sqlite3_keyword_check(kw, kwlen);
        }
    }

    free(s1);
    free(s2);
    free(s3);
    free(s4);
    return 0;
}