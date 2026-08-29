// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_deserialize at sqlite3.c:36973:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
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

static int fuzz_callback(void *ctx, int argc, char **argv, char **colnames) {
    (void)argv;
    (void)colnames;
    if (ctx == NULL) return 0;
    const uint8_t *p = (const uint8_t *)ctx;
    if (argc >= 0 && (p[0] & 1)) {
        return 1;
    }
    return 0;
}

int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    void *mem = NULL;
    unsigned char *buf = NULL;
    char *errmsg = NULL;
    int rc;

    rc = sqlite3_initialize();
    if (rc != SQLITE_OK) {
        return 0;
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0) {
                fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (db == NULL) {
        return 0;
    }

    {
        sqlite3_uint64 alloc_size = 0;
        if (Size >= 8) {
            alloc_size =
                ((sqlite3_uint64)Data[0] << 56) ^
                ((sqlite3_uint64)Data[1] << 48) ^
                ((sqlite3_uint64)Data[2] << 40) ^
                ((sqlite3_uint64)Data[3] << 32) ^
                ((sqlite3_uint64)Data[4] << 24) ^
                ((sqlite3_uint64)Data[5] << 16) ^
                ((sqlite3_uint64)Data[6] << 8)  ^
                ((sqlite3_uint64)Data[7]);
        } else {
            alloc_size = (sqlite3_uint64)Size;
        }

        alloc_size %= 65536;
        if (alloc_size == 0) {
            alloc_size = Size ? (sqlite3_uint64)Size : 1;
        }

        mem = sqlite3_malloc64(alloc_size);
        if (mem != NULL) {
            memset(mem, 0, (size_t)alloc_size);
        }
    }

    {
        sqlite3_int64 szDb = (sqlite3_int64)Size;
        sqlite3_int64 szBuf = szDb;
        unsigned flags = 0;

        if (Size > 0) {
            flags |= (Data[0] & 1) ? SQLITE_DESERIALIZE_FREEONCLOSE : 0;
            flags |= (Data[0] & 2) ? SQLITE_DESERIALIZE_RESIZEABLE : 0;
            flags |= (Data[0] & 4) ? SQLITE_DESERIALIZE_READONLY : 0;
        }

        if (szBuf == 0) {
            szBuf = 1;
        }

        if (flags & SQLITE_DESERIALIZE_FREEONCLOSE) {
            buf = (unsigned char *)sqlite3_malloc64((sqlite3_uint64)szBuf);
            if (buf != NULL) {
                if (Size > 0) {
                    memcpy(buf, Data, (size_t)szDb);
                } else {
                    buf[0] = 0;
                }
            }
        } else {
            buf = (unsigned char *)Data;
        }

        if (buf != NULL) {
            sqlite3_deserialize(db, "main", buf, szDb, szBuf, flags);
            if (!(flags & SQLITE_DESERIALIZE_FREEONCLOSE)) {
                buf = NULL;
            }
        }
    }

    {
        int op = SQLITE_FCNTL_DATA_VERSION;
        char dbname_storage[5] = {0};
        const char *zDbName = NULL;
        sqlite3_int64 arg64 = 0;
        void *pArg = &arg64;
        sqlite3_file *file_ptr = NULL;
        sqlite3_vfs *vfs_ptr = NULL;

        if (Size > 1) {
            switch (Data[1] % 6) {
                case 0:
                    zDbName = NULL;
                    break;
                case 1:
                    zDbName = "main";
                    break;
                case 2:
                    zDbName = "temp";
                    break;
                case 3:
                    zDbName = "invalid";
                    break;
                default:
                    memcpy(dbname_storage, "main", 5);
                    if (Size > 2) {
                        dbname_storage[0] = (char)Data[2];
                        dbname_storage[4] = '\0';
                    }
                    zDbName = dbname_storage;
                    break;
            }
        }

        if (Size > 3) {
            switch (Data[3] % 5) {
                case 0:
                    op = SQLITE_FCNTL_DATA_VERSION;
                    pArg = &arg64;
                    break;
                case 1:
                    op = SQLITE_FCNTL_FILE_POINTER;
                    pArg = &file_ptr;
                    break;
                case 2:
                    op = SQLITE_FCNTL_VFS_POINTER;
                    pArg = &vfs_ptr;
                    break;
                case 3:
                    op = SQLITE_FCNTL_JOURNAL_POINTER;
                    pArg = &file_ptr;
                    break;
                default:
                    op = (int)(Data[3]) | ((Size > 4 ? (int)Data[4] << 8 : 0));
                    pArg = &arg64;
                    break;
            }
        }

        sqlite3_file_control(db, zDbName, op, pArg);
    }

    {
        const char *sql = "";
        int use_callback = 0;

        if (Size > 0) {
            size_t sql_len = Size;
            char *sql_buf = (char *)sqlite3_malloc64((sqlite3_uint64)sql_len + 1);
            if (sql_buf != NULL) {
                memcpy(sql_buf, Data, sql_len);
                sql_buf[sql_len] = '\0';
                sql = sql_buf;
                use_callback = (Data[0] & 8) ? 1 : 0;
                sqlite3_exec(db, sql, use_callback ? fuzz_callback : NULL,
                             use_callback ? (void *)Data : NULL, &errmsg);
                sqlite3_free(sql_buf);
            } else {
                sqlite3_exec(db, sql, NULL, NULL, &errmsg);
            }
        } else {
            sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        }
    }

    if (errmsg != NULL) {
        sqlite3_free(errmsg);
    }

    if (mem != NULL) {
        sqlite3_free(mem);
    }

    if (db != NULL) {
        sqlite3_close(db);
    }

    return 0;
}