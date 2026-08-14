// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3_uint64 read_u64(const uint8_t *Data, size_t Size, size_t *Off) {
    sqlite3_uint64 v = 0;
    size_t i;
    for (i = 0; i < 8 && *Off < Size; ++i, ++(*Off)) {
        v = (v << 8) | Data[*Off];
    }
    return v;
}

static int read_i32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
        v = (v << 8) | Data[*Off];
    }
    return (int)v;
}

int LLVMFuzzerTestOneInput_62(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    void *p1 = NULL;
    void *p2 = NULL;
    void *p3 = NULL;

    sqlite3_initialize();

    sqlite3_free(NULL);
    sqlite3_free_filename(NULL);
    sqlite3_free_table(NULL);

    {
        int n1 = read_i32(Data, Size, &off);
        if (n1 < 0) n1 = -n1;
        n1 %= 4096;
        p1 = sqlite3_malloc(n1);
        if (p1 && off < Size) {
            size_t to_copy = (size_t)n1;
            if (to_copy > Size - off) to_copy = Size - off;
            memcpy(p1, Data + off, to_copy);
            off += to_copy;
        }
    }

    {
        sqlite3_uint64 n2 = read_u64(Data, Size, &off) % 4096;
        p2 = sqlite3_malloc64(n2);
        if (p2 && off < Size) {
            size_t to_copy = (size_t)n2;
            if (to_copy > Size - off) to_copy = Size - off;
            memcpy(p2, Data + off, to_copy);
            off += to_copy;
        }
    }

    {
        sqlite3_uint64 n3 = read_u64(Data, Size, &off) % 8192;
        p1 = sqlite3_realloc64(p1, n3);
        if (p1 && off < Size) {
            size_t to_copy = (size_t)n3;
            if (to_copy > Size - off) to_copy = Size - off;
            memcpy(p1, Data + off, to_copy);
            off += to_copy;
        }
    }

    {
        sqlite3_uint64 n4 = read_u64(Data, Size, &off) % 8192;
        p3 = sqlite3_realloc64(NULL, n4);
        if (p3 && off < Size) {
            size_t to_copy = (size_t)n4;
            if (to_copy > Size - off) to_copy = Size - off;
            memcpy(p3, Data + off, to_copy);
            off += to_copy;
        }
    }

    if (off < Size && (Data[off++] & 1)) {
        sqlite3_free(p2);
        p2 = NULL;
    }

    if (off < Size && (Data[off++] & 1)) {
        p2 = sqlite3_realloc64(p2, read_u64(Data, Size, &off) % 4096);
    }

    sqlite3_free(p1);
    sqlite3_free(p2);
    sqlite3_free(p3);

    sqlite3_free(NULL);
    sqlite3_free_filename(NULL);
    sqlite3_free_table(NULL);

    return 0;
}