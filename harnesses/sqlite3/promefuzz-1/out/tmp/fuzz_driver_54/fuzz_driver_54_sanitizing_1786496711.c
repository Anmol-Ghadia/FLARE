#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void udf_result_fuzzer(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    const unsigned char *data = NULL;
    int bytes = 0;
    int mode = 0;
    int len_choice = 0;
    int n = 0;
    void (*xDel)(void*) = SQLITE_TRANSIENT;

    if (argc > 0 && argv && argv[0]) {
        data = sqlite3_value_blob(argv[0]);
        bytes = sqlite3_value_bytes(argv[0]);
    }
    if (argc > 1 && argv && argv[1]) {
        mode = sqlite3_value_int(argv[1]);
    }
    if (argc > 2 && argv && argv[2]) {
        len_choice = sqlite3_value_int(argv[2]);
    }
    if (!data) {
        data = (const unsigned char *)"";
        bytes = 0;
    }

    switch ((unsigned)len_choice % 6) {
        case 0:
            n = bytes;
            break;
        case 1:
            n = 0;
            break;
        case 2:
            n = bytes;
            break;
        case 3:
            n = bytes > 0 ? bytes / 2 : 0;
            break;
        case 4:
            n = bytes > 0 ? bytes - 1 : 0;
            break;
        default:
            n = bytes + 4;
            break;
    }

    switch ((unsigned)mode % 12) {
        case 0:
            sqlite3_result_text(ctx, (const char *)data, n, xDel);
            break;
        case 1:
            sqlite3_result_text(ctx, (const char *)data, bytes, SQLITE_STATIC);
            break;
        case 2:
            sqlite3_result_text16le(ctx, (const void *)data, n, xDel);
            break;
        case 3:
            sqlite3_result_text16le(ctx, (const void *)data, bytes, SQLITE_STATIC);
            break;
        case 4:
            sqlite3_result_text16be(ctx, (const void *)data, n, xDel);
            break;
        case 5:
            sqlite3_result_text16be(ctx, (const void *)data, bytes, SQLITE_STATIC);
            break;
        case 6:
            sqlite3_result_text16(ctx, (const void *)data, n, xDel);
            break;
        case 7:
            sqlite3_result_text16(ctx, (const void *)data, bytes, SQLITE_STATIC);
            break;
        case 8:
            sqlite3_result_error(ctx, (const char *)data, n);
            break;
        case 9:
            sqlite3_result_error(ctx, (const char *)data, bytes);
            break;
        case 10:
            sqlite3_result_blob(ctx, (const void *)data, bytes, xDel);
            break;
        default:
            sqlite3_result_blob(ctx, (const void *)data, bytes, SQLITE_STATIC);
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;
    int mode = 0;
    int len_choice = 0;

    if (Size >= 1) mode = Data[0];
    if (Size >= 2) len_choice = Data[1];

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_create_function(db, "fuzz_result", 3, SQLITE_UTF8, NULL,
                            udf_result_fuzzer, NULL, NULL);

    rc = sqlite3_prepare_v2(
        db,
        "SELECT fuzz_result(?1, ?2, ?3);",
        -1,
        &stmt,
        NULL
    );
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, mode);
    sqlite3_bind_int(stmt, 3, len_choice);

    (void)sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, mode ^ 0x55);
    sqlite3_bind_int(stmt, 3, len_choice ^ 0xAA);
    (void)sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            fwrite(Data, 1, Size, fp);
            fclose(fp);
        }

        sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, &errmsg);
        if (errmsg) {
            sqlite3_free(errmsg);
            errmsg = NULL;
        }
        sqlite3_exec(db,
                     "CREATE TEMP VIEW v AS SELECT fuzz_result(readfile('./dummy_file'), 1, 2);",
                     NULL, NULL, &errmsg);
        if (errmsg) {
            sqlite3_free(errmsg);
            errmsg = NULL;
        }
    }

    sqlite3_close(db);
    return 0;
}