// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_str *s1 = NULL;
    sqlite3_str *s2 = NULL;
    char *out1 = NULL;
    char *out2 = NULL;
    int rc;
    char filename[256];
    size_t n;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Data != NULL && Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size == 0) {
        rc = sqlite3_open(":memory:", &db);
    } else {
        n = Size < sizeof(filename) - 1 ? Size : sizeof(filename) - 1;
        memcpy(filename, Data, n);
        filename[n] = '\0';

        for (size_t i = 0; i < n; i++) {
            if (filename[i] == '\0' || filename[i] == '\n' || filename[i] == '\r') {
                filename[i] = '_';
            }
        }

        if ((Data[0] & 0x03) == 0) {
            rc = sqlite3_open(":memory:", &db);
        } else if ((Data[0] & 0x03) == 1) {
            rc = sqlite3_open("", &db);
        } else if ((Data[0] & 0x03) == 2) {
            rc = sqlite3_open("./dummy_file", &db);
        } else {
            rc = sqlite3_open(filename, &db);
        }
    }

    (void)rc;

    s1 = sqlite3_str_new(db);
    s2 = sqlite3_str_new(db);

    out1 = sqlite3_str_finish(s1);
    out2 = sqlite3_str_finish(s2);

    sqlite3_close(db);
    sqlite3_free(out1);
    sqlite3_free(out2);

    return 0;
}