// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *off < size; ++i, ++(*off)) {
        v |= ((uint32_t)data[*off]) << (i * 8);
    }
    return v;
}

static int pick_flag_combo(uint32_t sel) {
    static const int combos[] = {
        SQLITE_OPEN_READONLY,
        SQLITE_OPEN_READWRITE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
        SQLITE_OPEN_READONLY | SQLITE_OPEN_URI,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_MEMORY,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXCLUSIVE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_NOMUTEX,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX
    };
    return combos[sel % (sizeof(combos) / sizeof(combos[0]))];
}

static void *make_utf16_terminated(const uint8_t *src, size_t len) {
    size_t even_len = len & ~(size_t)1;
    unsigned char *buf = (unsigned char *)malloc(even_len + 4);
    if (!buf) return NULL;
    if (even_len > 0) {
        memcpy(buf, src, even_len);
    }
    buf[even_len] = 0;
    buf[even_len + 1] = 0;
    buf[even_len + 2] = 0;
    buf[even_len + 3] = 0;
    return buf;
}

static void *make_utf16_from_ascii(const char *src) {
    size_t n = strlen(src);
    unsigned char *buf = (unsigned char *)calloc((n + 2), 2);
    size_t i;
    if (!buf) return NULL;
    for (i = 0; i < n; ++i) {
        buf[i * 2] = (unsigned char)src[i];
        buf[i * 2 + 1] = 0;
    }
    return buf;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint32_t mode, path_len_u32, vfs_len_u32, onoff_sel;
    size_t path_len, vfs_len;
    char *path = NULL;
    char *vfs = NULL;
    sqlite3 *db = NULL;
    int rc;

    if (Size == 0) {
        sqlite3_system_errno(NULL);
        sqlite3_errcode(NULL);
        sqlite3_extended_errcode(NULL);
        return 0;
    }

    mode = read_u32(Data, Size, &off);
    path_len_u32 = read_u32(Data, Size, &off);
    vfs_len_u32 = read_u32(Data, Size, &off);
    onoff_sel = read_u32(Data, Size, &off);

    path_len = (Size > off) ? (path_len_u32 % (Size - off + 1)) : 0;
    path = (char *)malloc(path_len + 1);
    if (!path) return 0;
    if (path_len > 0) memcpy(path, Data + off, path_len);
    path[path_len] = '\0';
    off += path_len;

    vfs_len = (Size > off) ? (vfs_len_u32 % (Size - off + 1)) : 0;
    vfs = (char *)malloc(vfs_len + 1);
    if (!vfs) {
        free(path);
        return 0;
    }
    if (vfs_len > 0) memcpy(vfs, Data + off, vfs_len);
    vfs[vfs_len] = '\0';
    off += vfs_len;

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (off < Size) {
                fwrite(Data + off, 1, Size - off, fp);
            }
            fclose(fp);
        }
    }

    switch (mode % 6) {
        case 0: {
            const char *filename = (path_len == 0) ? "./dummy_file" : path;
            const char *zVfs = (vfs_len == 0) ? NULL : vfs;
            int flags = pick_flag_combo(read_u32(Data, Size, &off));
            rc = sqlite3_open_v2(filename, &db, flags, zVfs);
            (void)rc;
            if (db) {
                sqlite3_extended_result_codes(db, (onoff_sel & 1) ? 1 : 0);
                sqlite3_errcode(db);
                sqlite3_extended_errcode(db);
                sqlite3_system_errno(db);
            } else {
                sqlite3_system_errno(NULL);
                sqlite3_errcode(NULL);
                sqlite3_extended_errcode(NULL);
            }
            break;
        }

        case 1: {
            const char *filename = "./dummy_file";
            int flags = pick_flag_combo(read_u32(Data, Size, &off));
            rc = sqlite3_open_v2(filename, &db, flags, NULL);
            (void)rc;
            if (db) {
                sqlite3_extended_result_codes(db, (int)(read_u32(Data, Size, &off) & 1));
                sqlite3_extended_result_codes(db, (int)(read_u32(Data, Size, &off) & 1));
                sqlite3_errcode(db);
                sqlite3_extended_errcode(db);
                sqlite3_system_errno(db);
            }
            break;
        }

        case 2: {
            void *utf16_name = NULL;
            if (path_len == 0) {
                rc = sqlite3_open16(NULL, &db);
            } else {
                utf16_name = make_utf16_terminated((const uint8_t *)path, path_len);
                if (!utf16_name) {
                    free(path);
                    free(vfs);
                    return 0;
                }
                rc = sqlite3_open16(utf16_name, &db);
                free(utf16_name);
            }
            (void)rc;
            if (db) {
                sqlite3_extended_result_codes(db, (onoff_sel & 1) ? 1 : 0);
                sqlite3_errcode(db);
                sqlite3_extended_errcode(db);
                sqlite3_system_errno(db);
            } else {
                sqlite3_system_errno(NULL);
                sqlite3_errcode(NULL);
                sqlite3_extended_errcode(NULL);
            }
            break;
        }

        case 3: {
            const char *filename = (path_len == 0) ? "" : path;
            int flags = pick_flag_combo(read_u32(Data, Size, &off));
            rc = sqlite3_open_v2(filename, &db, flags, (vfs_len == 0) ? "" : vfs);
            (void)rc;
            if (db) {
                sqlite3_extended_result_codes(db, 1);
                sqlite3_errcode(db);
                sqlite3_extended_errcode(db);
                sqlite3_system_errno(db);
                sqlite3_extended_result_codes(db, 0);
                sqlite3_errcode(db);
                sqlite3_extended_errcode(db);
            }
            break;
        }

        case 4: {
            rc = sqlite3_open_v2(NULL, &db, pick_flag_combo(read_u32(Data, Size, &off)), NULL);
            (void)rc;
            if (db) {
                sqlite3_extended_result_codes(db, (onoff_sel & 1) ? 1 : 0);
                sqlite3_errcode(db);
                sqlite3_extended_errcode(db);
                sqlite3_system_errno(db);
            } else {
                sqlite3_system_errno(NULL);
                sqlite3_errcode(NULL);
                sqlite3_extended_errcode(NULL);
            }
            break;
        }

        case 5:
        default: {
            void *utf16_name;
            if (path_len == 0) {
                utf16_name = make_utf16_from_ascii("./dummy_file");
            } else {
                utf16_name = make_utf16_terminated((const uint8_t *)path, path_len);
            }

            if (utf16_name) {
                rc = sqlite3_open16(utf16_name, &db);
                free(utf16_name);
                (void)rc;
                if (db) {
                    sqlite3_extended_result_codes(db, 1);
                    sqlite3_errcode(db);
                    sqlite3_extended_errcode(db);
                    sqlite3_system_errno(db);
                }
            } else {
                sqlite3_system_errno(NULL);
                sqlite3_errcode(NULL);
                sqlite3_extended_errcode(NULL);
            }
            break;
        }
    }

    if (db) {
        sqlite3_close(db);
    }

    free(path);
    free(vfs);
    return 0;
}