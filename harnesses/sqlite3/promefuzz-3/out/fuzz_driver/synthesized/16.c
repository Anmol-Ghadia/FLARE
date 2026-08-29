// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
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
#include "sqlite3.h"

int LLVMFuzzerTestOneInput_16(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_str *s1 = NULL;
    sqlite3_str *s2 = NULL;
    char *out1 = NULL;
    char *out2 = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Data != NULL && Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    (void)rc;

    s1 = sqlite3_str_new(db);
    s2 = sqlite3_str_new(db);

    out1 = sqlite3_str_finish(s1);
    out2 = sqlite3_str_finish(s2);

    rc = sqlite3_close(db);
    (void)rc;

    sqlite3_free(out1);
    sqlite3_free(out2);

    return 0;
}