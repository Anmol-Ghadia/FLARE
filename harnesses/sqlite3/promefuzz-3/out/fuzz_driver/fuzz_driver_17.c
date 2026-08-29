// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0) return 0;
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static int consume_i32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; i++) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static char *make_cstring(const uint8_t **data, size_t *size, size_t max_len) {
    size_t len = *size < max_len ? *size : max_len;
    char *s = (char *)malloc(len + 1);
    if (!s) return NULL;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (*data)[i];
        if (c == 0) c = 'A';
        s[i] = (char)c;
    }
    s[len] = '\0';
    *data += len;
    *size -= len;
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    int rc = sqlite3_initialize();
    if (rc != SQLITE_OK) {
        return 0;
    }

    int use_null_name = consume_u8(&p, &remaining) & 1;
    char *vfs_name = NULL;
    if (!use_null_name) {
        vfs_name = make_cstring(&p, &remaining, 256);
    }

    sqlite3_vfs *vfs = sqlite3_vfs_find(use_null_name ? NULL : vfs_name);
    (void)vfs;

    const char *ver = sqlite3_libversion();
    const char *sid = sqlite3_sourceid();
    (void)ver;
    (void)sid;

    int iterations = 1 + (consume_u8(&p, &remaining) % 32);
    for (int i = 0; i < iterations; i++) {
        int n;
        if (remaining > 0) {
            n = consume_i32(&p, &remaining);
        } else {
            n = i - 2;
        }

        if (consume_u8(&p, &remaining) & 1) {
            n = -n;
        }

        const char *opt = sqlite3_compileoption_get(n);
        (void)opt;
    }

    free(vfs_name);
    return 0;
}