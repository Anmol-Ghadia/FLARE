// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void udf_result_fuzzer(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    const unsigned char *data = (const unsigned char *)"";
    int bytes = 0;
    int mode = 0;
    int len_choice = 0;
    int n_text = 0;
    int n_blob = 0;

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

    switch ((unsigned)len_choice % 5) {
        case 0:
            n_text = bytes;
            break;
        case 1:
            n_text = bytes > 0 ? bytes / 2 : 0;
            break;
        case 2:
            n_text = bytes > 1 ? bytes - 1 : bytes;
            break;
        case 3:
            n_text = 0;
            break;
        default:
            n_text = bytes;
            break;
    }

    n_blob = bytes;

    switch ((unsigned)mode % 12) {
        case 0:
            sqlite3_result_text(ctx, (const char *)data, n_text, SQLITE_TRANSIENT);
            break;
        case 1:
            sqlite3_result_text(ctx, (const char *)data, n_text, SQLITE_STATIC);
            break;
        case 2:
            sqlite3_result_text16le(ctx, (const void *)data, n_text, SQLITE_TRANSIENT);
            break;
        case 3:
            sqlite3_result_text16le(ctx, (const void *)data, n_text, SQLITE_STATIC);
            break;
        case 4:
            sqlite3_result_text16be(ctx, (const void *)data, n_text, SQLITE_TRANSIENT);
            break;
        case 5:
            sqlite3_result_text16be(ctx, (const void *)data, n_text, SQLITE_STATIC);
            break;
        case 6:
            sqlite3_result_text16(ctx, (const void *)data, n_text, SQLITE_TRANSIENT);
            break;
        case 7:
            sqlite3_result_text16(ctx, (const void *)data, n_text, SQLITE_STATIC);
            break;
        case 8:
            sqlite3_result_error(ctx, (const char *)data, n_text);
            break;
        case 9:
            sqlite3_result_error(ctx, (const char *)data, bytes);
            break;
        case 10:
            sqlite3_result_blob(ctx, (const void *)data, n_blob, SQLITE_TRANSIENT);
            break;
        default:
            sqlite3_result_blob(ctx, (const void *)data, n_blob, SQLITE_STATIC);
            break;
    }
}

int LLVMFuzzerTestOneInput_54(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp = NULL;
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

    (void)sqlite3_create_function(db, "fuzz_result", 3, SQLITE_UTF8, NULL,
                                  udf_result_fuzzer, NULL, NULL);

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_prepare_v2(db, "SELECT fuzz_result(?1, ?2, ?3);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt != NULL) {
        (void)sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
        (void)sqlite3_bind_int(stmt, 2, mode);
        (void)sqlite3_bind_int(stmt, 3, len_choice);
        (void)sqlite3_step(stmt);

        (void)sqlite3_reset(stmt);
        (void)sqlite3_clear_bindings(stmt);

        (void)sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
        (void)sqlite3_bind_int(stmt, 2, mode ^ 0x55);
        (void)sqlite3_bind_int(stmt, 3, len_choice ^ 0xAA);
        (void)sqlite3_step(stmt);

        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_close(db);
    return 0;
}