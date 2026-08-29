// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_system_errno at sqlite3.c:154868:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *Off < Size; i++, (*Off)++) {
        v |= ((uint32_t)Data[*Off]) << (i * 8);
    }
    return v;
}

static int pick_flag(uint32_t x) {
    int flags = 0;
    if (x & 1U) flags |= SQLITE_OPEN_READONLY;
    if (x & 2U) flags |= SQLITE_OPEN_READWRITE;
    if (x & 4U) flags |= SQLITE_OPEN_CREATE;
    if (x & 8U) flags |= SQLITE_OPEN_URI;
    if (x & 16U) flags |= SQLITE_OPEN_MEMORY;
    if (x & 32U) flags |= SQLITE_OPEN_NOMUTEX;
    if (x & 64U) flags |= SQLITE_OPEN_FULLMUTEX;
    if (x & 128U) flags |= SQLITE_OPEN_SHAREDCACHE;
    if (x & 256U) flags |= SQLITE_OPEN_PRIVATECACHE;
#ifdef SQLITE_OPEN_EXRESCODE
    if (x & 512U) flags |= SQLITE_OPEN_EXRESCODE;
#endif
    if ((flags & (SQLITE_OPEN_READONLY | SQLITE_OPEN_READWRITE)) == 0) {
        flags |= SQLITE_OPEN_READWRITE;
    }
    return flags;
}

static const char *pick_dbname(uint32_t x) {
    switch (x % 6U) {
        case 0: return "main";
        case 1: return "temp";
        case 2: return NULL;
        case 3: return "aux";
        case 4: return "";
        default: return "nonexistent";
    }
}

static const char *pick_vfs(uint32_t x) {
    switch (x % 4U) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "unix";
        default: return "no_such_vfs";
    }
}

static int pick_fcntl_op(uint32_t x) {
    static const int ops[] = {
#ifdef SQLITE_FCNTL_LOCKSTATE
        SQLITE_FCNTL_LOCKSTATE,
#endif
#ifdef SQLITE_FCNTL_SIZE_HINT
        SQLITE_FCNTL_SIZE_HINT,
#endif
#ifdef SQLITE_FCNTL_CHUNK_SIZE
        SQLITE_FCNTL_CHUNK_SIZE,
#endif
#ifdef SQLITE_FCNTL_FILE_POINTER
        SQLITE_FCNTL_FILE_POINTER,
#endif
#ifdef SQLITE_FCNTL_JOURNAL_POINTER
        SQLITE_FCNTL_JOURNAL_POINTER,
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        SQLITE_FCNTL_VFS_POINTER,
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
        SQLITE_FCNTL_DATA_VERSION,
#endif
#ifdef SQLITE_FCNTL_PERSIST_WAL
        SQLITE_FCNTL_PERSIST_WAL,
#endif
#ifdef SQLITE_FCNTL_POWERSAFE_OVERWRITE
        SQLITE_FCNTL_POWERSAFE_OVERWRITE,
#endif
#ifdef SQLITE_FCNTL_PRAGMA
        SQLITE_FCNTL_PRAGMA,
#endif
        -1,
        0,
        1,
        999999
    };
    return ops[x % (sizeof(ops) / sizeof(ops[0]))];
}

static void do_file_control(sqlite3 *db, const char *zDbName, int op, uint32_t seed1, uint32_t seed2) {
    sqlite3_file *file_ptr = NULL;
    sqlite3_vfs *vfs_ptr = NULL;
    int data_version = 0;
    sqlite3_int64 size_hint = (sqlite3_int64)(seed1 ^ seed2);
    int int_arg = (int)(seed2 & 0x7fffffffU);
    void *arg = &int_arg;

    if (db == NULL) {
        return;
    }

    switch (op) {
#ifdef SQLITE_FCNTL_FILE_POINTER
        case SQLITE_FCNTL_FILE_POINTER:
            arg = &file_ptr;
            break;
#endif
#ifdef SQLITE_FCNTL_JOURNAL_POINTER
        case SQLITE_FCNTL_JOURNAL_POINTER:
            arg = &file_ptr;
            break;
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        case SQLITE_FCNTL_VFS_POINTER:
            arg = &vfs_ptr;
            break;
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
        case SQLITE_FCNTL_DATA_VERSION:
            arg = &data_version;
            break;
#endif
#ifdef SQLITE_FCNTL_SIZE_HINT
        case SQLITE_FCNTL_SIZE_HINT:
            arg = &size_hint;
            break;
#endif
#ifdef SQLITE_FCNTL_CHUNK_SIZE
        case SQLITE_FCNTL_CHUNK_SIZE:
            arg = &int_arg;
            break;
#endif
#ifdef SQLITE_FCNTL_PERSIST_WAL
        case SQLITE_FCNTL_PERSIST_WAL:
            arg = &int_arg;
            break;
#endif
#ifdef SQLITE_FCNTL_POWERSAFE_OVERWRITE
        case SQLITE_FCNTL_POWERSAFE_OVERWRITE:
            arg = &int_arg;
            break;
#endif
        default:
            arg = &int_arg;
            break;
    }

    (void)sqlite3_file_control(db, zDbName, op, arg);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    sqlite3 *db1 = NULL;
    sqlite3 *db2 = NULL;
    int rc1, rc2;
    char fname[256];
    unsigned char utf16name[512];
    size_t name_len, i;
    uint32_t selector1, selector2, selector3;
    int flags;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    selector1 = rd32(Data, Size, &off);
    selector2 = rd32(Data, Size, &off);
    selector3 = rd32(Data, Size, &off);

    memset(fname, 0, sizeof(fname));
    memcpy(fname, "./dummy_file", sizeof("./dummy_file"));

    name_len = Size - off;
    if (name_len > 0) {
        size_t prefix = strlen("./dummy_file");
        size_t copy_len = name_len;
        if (prefix + 1 + copy_len >= sizeof(fname)) {
            copy_len = sizeof(fname) - prefix - 2;
        }
        fname[prefix] = '_';
        memcpy(fname + prefix + 1, Data + off, copy_len);
        for (i = 0; i < copy_len; i++) {
            unsigned char c = (unsigned char)fname[prefix + 1 + i];
            if (c == '\0' || c == '/' || c == '\\' || c == ':' || c == '\n' || c == '\r') {
                fname[prefix + 1 + i] = (char)('A' + (c % 26));
            }
        }
        fname[prefix + 1 + copy_len] = '\0';
    }

    memset(utf16name, 0, sizeof(utf16name));
    {
        size_t utf8len = strlen(fname);
        size_t maxchars = (sizeof(utf16name) / 2) - 1;
        if (utf8len > maxchars) utf8len = maxchars;
        for (i = 0; i < utf8len; i++) {
            utf16name[i * 2] = (unsigned char)fname[i];
            utf16name[i * 2 + 1] = 0;
        }
    }

    flags = pick_flag(selector1);
    rc1 = sqlite3_open_v2(fname, &db1, flags, pick_vfs(selector2));
    (void)rc1;
    if (db1 != NULL) {
        (void)sqlite3_errcode(db1);
        (void)sqlite3_extended_errcode(db1);
        (void)sqlite3_system_errno(db1);

        do_file_control(db1, pick_dbname(selector1), pick_fcntl_op(selector3), selector1, selector2);
        do_file_control(db1, "main", -1, selector2, selector3);
        do_file_control(db1, "temp", 0, selector3, selector1);
        do_file_control(db1, "aux", 999999, selector1 ^ selector3, selector2 ^ selector3);

        (void)sqlite3_errcode(db1);
        (void)sqlite3_extended_errcode(db1);
        (void)sqlite3_system_errno(db1);
    } else {
        (void)sqlite3_system_errno(db1);
        (void)sqlite3_errcode(db1);
        (void)sqlite3_extended_errcode(db1);
    }

    rc2 = sqlite3_open16(utf16name, &db2);
    (void)rc2;
    if (db2 != NULL) {
#ifdef SQLITE_FCNTL_FILE_POINTER
        do_file_control(db2, NULL, SQLITE_FCNTL_FILE_POINTER, selector1, selector2);
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        do_file_control(db2, "main", SQLITE_FCNTL_VFS_POINTER, selector2, selector3);
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
        do_file_control(db2, "main", SQLITE_FCNTL_DATA_VERSION, selector3, selector1);
#endif
        do_file_control(db2, pick_dbname(selector2), pick_fcntl_op(selector1), selector2, selector3);

        (void)sqlite3_errcode(db2);
        (void)sqlite3_extended_errcode(db2);
        (void)sqlite3_system_errno(db2);
    } else {
        (void)sqlite3_system_errno(db2);
        (void)sqlite3_errcode(db2);
        (void)sqlite3_extended_errcode(db2);
    }

    if (db2 != NULL) {
        sqlite3_close(db2);
    }
    if (db1 != NULL) {
        sqlite3_close(db1);
    }

    (void)sqlite3_system_errno(NULL);
    return 0;
}