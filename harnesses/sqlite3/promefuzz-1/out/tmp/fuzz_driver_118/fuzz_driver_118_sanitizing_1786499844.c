#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; i++) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static int read_bool(const uint8_t **p, size_t *n) {
    if (*n == 0) return 0;
    int v = (**p) & 1;
    (*p)++;
    (*n)--;
    return v;
}

static int pick_flags(uint32_t selector) {
    static const int candidates[10] = {
        SQLITE_OPEN_READONLY,
        SQLITE_OPEN_READWRITE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE,
        SQLITE_OPEN_READONLY | SQLITE_OPEN_URI
    };
    return candidates[selector % 10];
}

static void make_cstring(const uint8_t **p, size_t *n, char *out, size_t out_sz) {
    size_t i = 0;
    if (out_sz == 0) return;
    while (*n > 0 && i + 1 < out_sz) {
        uint8_t c = **p;
        (*p)++;
        (*n)--;
        if (c == 0) break;
        out[i++] = (char)c;
    }
    out[i] = '\0';
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (data && size) {
        fwrite(data, 1, size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    sqlite3 *db = NULL;
    sqlite3_vfs *default_vfs = NULL;
    sqlite3_vfs *found_vfs = NULL;
    char vfs_name_buf[256];
    const char *vfs_name = NULL;
    int do_shutdown_early;
    int register_default;
    int open_flags;

    write_dummy_file(Data, Size);

    do_shutdown_early = read_bool(&p, &n);
    register_default = read_bool(&p, &n);
    open_flags = pick_flags(read_u32(&p, &n));
    make_cstring(&p, &n, vfs_name_buf, sizeof(vfs_name_buf));

    if (vfs_name_buf[0] != '\0') {
        vfs_name = vfs_name_buf;
    }

    (void)sqlite3_initialize();

    default_vfs = sqlite3_vfs_find(NULL);
    found_vfs = sqlite3_vfs_find(vfs_name);

    if (default_vfs) {
        (void)sqlite3_vfs_register(default_vfs, register_default);
        if (default_vfs->zName) {
            (void)sqlite3_vfs_find(default_vfs->zName);
        }
    }

    if (found_vfs) {
        (void)sqlite3_vfs_register(found_vfs, read_bool(&p, &n));
    }

    if (do_shutdown_early) {
        (void)sqlite3_shutdown();
        (void)sqlite3_initialize();
        default_vfs = sqlite3_vfs_find(NULL);
        found_vfs = sqlite3_vfs_find(vfs_name);
    }

    {
        const char *filename = "./dummy_file";
        const char *use_vfs = NULL;

        if (read_bool(&p, &n)) {
            use_vfs = vfs_name;
        } else if (found_vfs && found_vfs->zName && read_bool(&p, &n)) {
            use_vfs = found_vfs->zName;
        } else if (default_vfs && default_vfs->zName && read_bool(&p, &n)) {
            use_vfs = default_vfs->zName;
        }

        (void)sqlite3_open_v2(filename, &db, open_flags, use_vfs);
    }

    if (db) {
        sqlite3_close(db);
        db = NULL;
    }

    if (found_vfs) {
        (void)sqlite3_vfs_unregister(found_vfs);
        (void)sqlite3_vfs_register(found_vfs, 0);
    }

    if (default_vfs) {
        (void)sqlite3_vfs_unregister(default_vfs);
        (void)sqlite3_vfs_register(default_vfs, 1);
    }

    (void)sqlite3_shutdown();
    (void)sqlite3_initialize();
    (void)sqlite3_vfs_find(NULL);
    (void)sqlite3_shutdown();

    return 0;
}