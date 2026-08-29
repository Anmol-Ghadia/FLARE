// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_vfs_find at sqlite3.c:11431:25 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
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

static int consume_int(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < sizeof(int) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&v, *data, n);
        *data += n;
        *size -= n;
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    int rc = sqlite3_initialize();
    if (rc != SQLITE_OK) {
        return 0;
    }

    char vfs_name_buf[256];
    const char *zVfsName = NULL;

    if (remaining > 0) {
        size_t name_len = remaining < sizeof(vfs_name_buf) - 1 ? remaining : sizeof(vfs_name_buf) - 1;
        memcpy(vfs_name_buf, p, name_len);
        vfs_name_buf[name_len] = '\0';

        if ((p[0] & 1) != 0) {
            zVfsName = vfs_name_buf;
        } else {
            zVfsName = NULL;
        }
    }

    sqlite3_vfs *vfs1 = sqlite3_vfs_find(zVfsName);
    sqlite3_vfs *vfs2 = sqlite3_vfs_find(NULL);
    if (vfs1 != NULL && vfs1->zName != NULL) {
        (void)sqlite3_vfs_find(vfs1->zName);
    }
    if (vfs2 != NULL && vfs2->zName != NULL) {
        (void)sqlite3_vfs_find(vfs2->zName);
    }

    const char *ver = sqlite3_libversion();
    const char *sid = sqlite3_sourceid();
    (void)ver;
    (void)sid;

    int idx1 = consume_int(&p, &remaining);
    int idx2 = consume_int(&p, &remaining);
    int idx3 = consume_int(&p, &remaining);

    (void)sqlite3_compileoption_get(idx1);
    (void)sqlite3_compileoption_get(idx2);
    (void)sqlite3_compileoption_get(idx3);
    (void)sqlite3_compileoption_get(-1);
    (void)sqlite3_compileoption_get(0);

    return 0;
}