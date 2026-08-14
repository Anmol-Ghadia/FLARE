// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int32_t read_i32(const uint8_t **data, size_t *size) {
    return (int32_t)read_u32(data, size);
}

static sqlite3_int64 read_i64(const uint8_t **data, size_t *size) {
    sqlite3_int64 v = 0;
    size_t n = *size < 8 ? *size : 8;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int choose_index(const uint8_t **data, size_t *size, int max_param) {
    if (max_param <= 0) return 1;
    uint32_t v = read_u32(data, size);
    return (int)(v % (uint32_t)max_param) + 1;
}

static const char *choose_text(const uint8_t **data, size_t *size, char *buf, size_t buf_sz) {
    if (buf_sz == 0) return "";
    size_t n = *size;
    if (n >= buf_sz) n = buf_sz - 1;
    if (n > 0) {
        memcpy(buf, *data, n);
        *data += n;
        *size -= n;
    }
    buf[n] = '\0';
    return buf;
}

int LLVMFuzzerTestOneInput_34(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp = NULL;
    int rc;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    {
        const char *sql =
            "SELECT "
            "?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, "
            "?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20, "
            "?21, ?22, ?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30, "
            "?31, ?32, ?33, ?34";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            sqlite3_close(db);
            return 0;
        }
    }

    {
        const uint8_t *p = Data;
        size_t remaining = Size;
        int max_param = sqlite3_bind_parameter_count(stmt);
        char text1[256], text2[256], text3[256], text4[256], text5[256];
        char text6[256], text7[256], text8[256], text9[256];

        sqlite3_bind_int64(stmt, choose_index(&p, &remaining, max_param), read_i64(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text1, sizeof(text1)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int64(stmt, choose_index(&p, &remaining, max_param), read_i64(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text2, sizeof(text2)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int64(stmt, choose_index(&p, &remaining, max_param), read_i64(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text3, sizeof(text3)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text4, sizeof(text4)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text5, sizeof(text5)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text6, sizeof(text6)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text7, sizeof(text7)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text8, sizeof(text8)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        (void)sqlite3_libversion_number();
        sqlite3_bind_int(stmt, choose_index(&p, &remaining, max_param), read_i32(&p, &remaining));
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text9, sizeof(text9)), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, choose_index(&p, &remaining, max_param), choose_text(&p, &remaining, text1, sizeof(text1)), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        (void)rc;
        sqlite3_reset(stmt);

        if (Size > 0) {
            sqlite3_clear_bindings(stmt);
            p = Data;
            remaining = Size;

            sqlite3_bind_int64(stmt, 1, read_i64(&p, &remaining));
            sqlite3_bind_int(stmt, 2, read_i32(&p, &remaining));
            sqlite3_bind_text(stmt, 3, choose_text(&p, &remaining, text1, sizeof(text1)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 4, read_i32(&p, &remaining));
            sqlite3_bind_int64(stmt, 5, read_i64(&p, &remaining));
            sqlite3_bind_text(stmt, 6, choose_text(&p, &remaining, text2, sizeof(text2)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 7, read_i32(&p, &remaining));
            sqlite3_bind_int64(stmt, 8, read_i64(&p, &remaining));
            sqlite3_bind_text(stmt, 9, choose_text(&p, &remaining, text3, sizeof(text3)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 10, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 11, read_i32(&p, &remaining));
            sqlite3_bind_text(stmt, 12, choose_text(&p, &remaining, text4, sizeof(text4)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 13, choose_text(&p, &remaining, text5, sizeof(text5)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 14, choose_text(&p, &remaining, text6, sizeof(text6)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 15, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 16, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 17, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 18, read_i32(&p, &remaining));
            sqlite3_bind_text(stmt, 19, choose_text(&p, &remaining, text7, sizeof(text7)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 20, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 21, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 22, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 23, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 24, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 25, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 26, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 27, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 28, read_i32(&p, &remaining));
            sqlite3_bind_int(stmt, 29, read_i32(&p, &remaining));
            sqlite3_bind_text(stmt, 30, choose_text(&p, &remaining, text8, sizeof(text8)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 31, read_i32(&p, &remaining));
            (void)sqlite3_libversion_number();
            sqlite3_bind_int(stmt, 32, read_i32(&p, &remaining));
            sqlite3_bind_text(stmt, 33, choose_text(&p, &remaining, text9, sizeof(text9)), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 34, choose_text(&p, &remaining, text1, sizeof(text1)), -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            (void)rc;
            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}