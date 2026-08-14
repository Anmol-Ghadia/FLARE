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

static void trace_cb(void *pArg, const char *zSql) {
    (void)pArg;
    (void)zSql;
}

static int exec_cb(void *ctx, int argc, char **argv, char **colNames) {
    const uint8_t *data = (const uint8_t *)ctx;
    int decision = 0;

    if (data && argc >= 0) {
        decision = data[0] & 1;
    }

    (void)argv;
    (void)colNames;
    return decision;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    char *mprintf_buf = NULL;
    char *errmsg = NULL;
    char *sql_buf = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);

    if (db != NULL) {
        if (Size > 0 && (Data[0] & 1)) {
            sqlite3_trace(db, trace_cb, (void *)Data);
        } else {
            sqlite3_trace(db, NULL, NULL);
        }

        if (Size > 0) {
            size_t sql_len = Size;
            sql_buf = (char *)malloc(sql_len + 1);
            if (sql_buf != NULL) {
                memcpy(sql_buf, Data, sql_len);
                sql_buf[sql_len] = '\0';
            }

            mprintf_buf = sqlite3_mprintf("%s", sql_buf ? sql_buf : "");
        } else {
            mprintf_buf = sqlite3_mprintf("%s", "");
        }

        if (mprintf_buf != NULL) {
            sqlite3_exec(
                db,
                mprintf_buf,
                (Size > 1 && (Data[1] & 1)) ? exec_cb : NULL,
                (void *)Data,
                &errmsg
            );

            if (Size > 2) {
                sqlite3_exec(
                    db,
                    (Data[2] & 1) ? "" : " ",
                    NULL,
                    NULL,
                    &errmsg
                );
            }
        }

        if (errmsg != NULL) {
            sqlite3_free(errmsg);
            errmsg = NULL;
        }

        sqlite3_free(mprintf_buf);
        mprintf_buf = NULL;

        sqlite3_close(db);
        db = NULL;
    }

    free(sql_buf);
    return 0;
}