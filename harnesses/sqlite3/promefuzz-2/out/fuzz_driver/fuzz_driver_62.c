// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_sourceid at sqlite3.c:222713:24 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int read_int32(const uint8_t *Data, size_t Size, size_t *Offset) {
    int v = 0;
    size_t i;
    for (i = 0; i < 4 && *Offset < Size; ++i, ++(*Offset)) {
        v = (v << 8) | Data[*Offset];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    int rc;
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_initialize();
    (void)rc;

    (void)sqlite3_libversion_number();
    (void)sqlite3_threadsafe();

    {
        const char *sid = sqlite3_sourceid();
        if (sid != NULL) {
            volatile size_t sid_len = strlen(sid);
            (void)sid_len;
        }
    }

    (void)sqlite3_compileoption_used(NULL);

    if (Size == 0) {
        (void)sqlite3_compileoption_get(-1);
        (void)sqlite3_compileoption_get(0);
        (void)sqlite3_compileoption_get(1);
        return 0;
    }

    for (i = 0; i < 16; ++i) {
        int idx = read_int32(Data, Size, &off);
        const char *opt = sqlite3_compileoption_get(idx);
        if (opt != NULL) {
            (void)sqlite3_compileoption_used(opt);

            if (strncmp(opt, "SQLITE_", 7) == 0) {
                (void)sqlite3_compileoption_used(opt + 7);
            }

            {
                volatile size_t opt_len = strlen(opt);
                (void)opt_len;
            }
        } else {
            (void)sqlite3_compileoption_used("");
        }

        if (off >= Size) {
            break;
        }
    }

    while (off < Size) {
        size_t remaining = Size - off;
        size_t len = Data[off++] % 64;
        char buf[65];

        if (len > remaining - (off <= Size ? 0 : 0)) {
            len = remaining < 64 ? remaining : 64;
        }
        if (len > Size - off) {
            len = Size - off;
        }

        memcpy(buf, Data + off, len);
        buf[len] = '\0';
        off += len;

        (void)sqlite3_compileoption_used(buf);

        if (len >= 7 && strncmp(buf, "SQLITE_", 7) == 0) {
            (void)sqlite3_compileoption_used(buf + 7);
        }

        if (len > 0) {
            int idx = (unsigned char)buf[0];
            (void)sqlite3_compileoption_get(idx);
            (void)sqlite3_compileoption_get(-idx);
        }

        (void)sqlite3_libversion_number();
        (void)sqlite3_threadsafe();
        (void)sqlite3_sourceid();
        (void)sqlite3_initialize();
    }

    (void)sqlite3_compileoption_get(-1);
    (void)sqlite3_compileoption_get(0);
    (void)sqlite3_compileoption_get(1024 * 1024);

    return 0;
}