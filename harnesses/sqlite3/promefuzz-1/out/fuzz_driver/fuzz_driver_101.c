// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off, uint32_t *Out) {
    if (*Off + 4 > Size) return 0;
    *Out = ((uint32_t)Data[*Off]) |
           ((uint32_t)Data[*Off + 1] << 8) |
           ((uint32_t)Data[*Off + 2] << 16) |
           ((uint32_t)Data[*Off + 3] << 24);
    *Off += 4;
    return 1;
}

static int choose_index(uint32_t v, int col_count) {
    if (col_count <= 0) {
        return (int)(v % 7) - 3;
    }
    switch (v % 5) {
        case 0: return -1;
        case 1: return col_count;
        case 2: return col_count + 1;
        default: return (int)(v % (uint32_t)col_count);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    uint32_t v1 = 0, v2 = 0, v3 = 0, v4 = 0;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
        "CREATE TABLE t("
        "a INTEGER,"
        "b REAL,"
        "c TEXT,"
        "d BLOB,"
        "e TEXT,"
        "f BLOB,"
        "g NULL"
        ");",
        NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db,
        "INSERT INTO t VALUES("
        "123,"
        "3.14159,"
        "'hello',"
        "x'00010203',"
        "'',"
        "x'',"
        "NULL"
        ");",
        NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    if (read_u32(Data, Size, &off, &v1) &&
        read_u32(Data, Size, &off, &v2) &&
        read_u32(Data, Size, &off, &v3) &&
        read_u32(Data, Size, &off, &v4)) {
        sqlite3_stmt *ins = NULL;
        if (sqlite3_prepare_v2(db,
                "INSERT INTO t VALUES(?1, ?2, ?3, ?4, ?5, ?6, NULL);",
                -1, &ins, NULL) == SQLITE_OK && ins) {
            sqlite3_bind_int64(ins, 1, (sqlite3_int64)(int32_t)v1);
            sqlite3_bind_double(ins, 2, (double)(int32_t)v2 / 1000.0);
            if (off <= Size) {
                size_t rem = Size - off;
                size_t n1 = rem / 3;
                size_t n2 = rem / 3;
                size_t n3 = rem - n1 - n2;
                sqlite3_bind_text(ins, 3, (const char *)(Data + off), (int)n1, SQLITE_TRANSIENT);
                sqlite3_bind_blob(ins, 4, (const void *)(Data + off + n1), (int)n2, SQLITE_TRANSIENT);
                sqlite3_bind_text(ins, 5, (const char *)(Data + off + n1 + n2), (int)n3, SQLITE_TRANSIENT);
                sqlite3_bind_blob(ins, 6, (const void *)(Data + off), (int)rem, SQLITE_TRANSIENT);
            } else {
                sqlite3_bind_text(ins, 3, "", 0, SQLITE_STATIC);
                sqlite3_bind_blob(ins, 4, "", 0, SQLITE_STATIC);
                sqlite3_bind_text(ins, 5, "", 0, SQLITE_STATIC);
                sqlite3_bind_blob(ins, 6, "", 0, SQLITE_STATIC);
            }
            sqlite3_step(ins);
            sqlite3_finalize(ins);
        } else if (ins) {
            sqlite3_finalize(ins);
        }
    }

    rc = sqlite3_prepare_v2(
        db,
        "SELECT a, b, c, d, e, f, g, "
        "CAST(a AS TEXT), CAST(b AS BLOB), hex(d), length(c) "
        "FROM t;",
        -1,
        &stmt,
        NULL
    );
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int col_count = sqlite3_column_count(stmt);
        int i;
        int idx1, idx2, idx3, idx4;

        if (!read_u32(Data, Size, &off, &v1)) v1 = 0;
        if (!read_u32(Data, Size, &off, &v2)) v2 = 1;
        if (!read_u32(Data, Size, &off, &v3)) v3 = 2;
        if (!read_u32(Data, Size, &off, &v4)) v4 = 3;

        idx1 = choose_index(v1, col_count);
        idx2 = choose_index(v2, col_count);
        idx3 = choose_index(v3, col_count);
        idx4 = choose_index(v4, col_count);

        (void)sqlite3_column_database_name16(stmt, idx1);
        (void)sqlite3_column_type(stmt, idx1);
        (void)sqlite3_column_text16(stmt, idx1);
        (void)sqlite3_column_bytes16(stmt, idx1);
        (void)sqlite3_column_blob(stmt, idx1);
        (void)sqlite3_column_bytes(stmt, idx1);

        (void)sqlite3_column_blob(stmt, idx2);
        (void)sqlite3_column_bytes(stmt, idx2);
        (void)sqlite3_column_text16(stmt, idx2);
        (void)sqlite3_column_bytes16(stmt, idx2);
        (void)sqlite3_column_type(stmt, idx2);
        (void)sqlite3_column_database_name16(stmt, idx2);

        (void)sqlite3_column_text16(stmt, idx3);
        (void)sqlite3_column_bytes16(stmt, idx3);
        (void)sqlite3_column_blob(stmt, idx3);
        (void)sqlite3_column_bytes(stmt, idx3);
        (void)sqlite3_column_type(stmt, idx3);
        (void)sqlite3_column_database_name16(stmt, idx3);

        for (i = 0; i < col_count; i++) {
            switch ((v4 + (uint32_t)i) % 3) {
                case 0:
                    (void)sqlite3_column_text16(stmt, i);
                    (void)sqlite3_column_bytes16(stmt, i);
                    (void)sqlite3_column_type(stmt, i);
                    break;
                case 1:
                    (void)sqlite3_column_blob(stmt, i);
                    (void)sqlite3_column_bytes(stmt, i);
                    (void)sqlite3_column_type(stmt, i);
                    break;
                default:
                    (void)sqlite3_column_database_name16(stmt, i);
                    (void)sqlite3_column_type(stmt, i);
                    (void)sqlite3_column_text16(stmt, i);
                    (void)sqlite3_column_bytes16(stmt, i);
                    (void)sqlite3_column_blob(stmt, i);
                    (void)sqlite3_column_bytes(stmt, i);
                    break;
            }
        }

        if (off >= Size) {
            break;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}