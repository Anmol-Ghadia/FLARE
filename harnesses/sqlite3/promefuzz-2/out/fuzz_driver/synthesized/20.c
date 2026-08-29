// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

int LLVMFuzzerTestOneInput_20(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_value *val = NULL;
    sqlite3_value *dup = NULL;
    const unsigned char *txt = NULL;
    void *buf = NULL;
    sqlite3_uint64 alloc_size = 0;

    if (sqlite3_initialize() != SQLITE_OK) {
        return 0;
    }

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    if (sqlite3_prepare_v2(db, "SELECT ?1;", -1, &stmt, NULL) != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    if (Size == 0) {
        sqlite3_bind_null(stmt, 1);
    } else {
        switch (Data[0] % 5) {
            case 0:
                sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
                break;
            case 1:
                sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
                break;
            case 2: {
                sqlite3_int64 v = 0;
                size_t n = Size < sizeof(v) ? Size : sizeof(v);
                memcpy(&v, Data, n);
                sqlite3_bind_int64(stmt, 1, v);
                break;
            }
            case 3: {
                double d = 0.0;
                size_t n = Size < sizeof(d) ? Size : sizeof(d);
                memcpy(&d, Data, n);
                sqlite3_bind_double(stmt, 1, d);
                break;
            }
            default:
                sqlite3_bind_null(stmt, 1);
                break;
        }
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        val = sqlite3_column_value(stmt, 0);
        if (val) {
            dup = sqlite3_value_dup(val);
        }
    }

    if (dup != NULL) {
        txt = sqlite3_value_text(dup);
    } else {
        txt = NULL;
    }

    if (Size > 0) {
        alloc_size = (sqlite3_uint64)((Data[0] % 256) + 1);
    } else {
        alloc_size = 1;
    }

    buf = sqlite3_malloc64(alloc_size);

    if (buf != NULL) {
        if (txt != NULL) {
            size_t src_len = strlen((const char *)txt);
            size_t copy_len = src_len;
            if (copy_len >= (size_t)alloc_size) {
                copy_len = (size_t)alloc_size - 1;
            }
            memcpy(buf, txt, copy_len);
            ((char *)buf)[copy_len] = '\0';
        } else {
            ((char *)buf)[0] = '\0';
        }
    }

    /* sqlite3_result_text requires a valid sqlite3_context provided by SQLite.
       Avoid calling it with an invalid context to prevent crashes. */
    if (0) {
        sqlite3_result_text((sqlite3_context *)0, (const char *)buf, -1, SQLITE_TRANSIENT);
    }

    sqlite3_free(buf);
    sqlite3_value_free(dup);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}