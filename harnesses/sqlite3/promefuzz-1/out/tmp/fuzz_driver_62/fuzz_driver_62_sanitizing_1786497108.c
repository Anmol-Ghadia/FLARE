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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    void *p1 = NULL;
    void *p2 = NULL;
    void *p3 = NULL;
    char **table = NULL;
    int i;

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

    {
        int rows = 0, cols = 0;
        if (off < Size) rows = Data[off++] % 4;
        if (off < Size) cols = Data[off++] % 4;

        {
            int total = (rows + 1) * cols;
            table = (char **)sqlite3_malloc64((sqlite3_uint64)total * sizeof(char *));
            if (table) {
                for (i = 0; i < total; ++i) {
                    sqlite3_uint64 cell_len = 0;
                    size_t to_copy;
                    if (off < Size) cell_len = (sqlite3_uint64)(Data[off++] % 32);
                    table[i] = (char *)sqlite3_malloc64(cell_len + 1);
                    if (!table[i]) {
                        int j;
                        for (j = 0; j < i; ++j) {
                            sqlite3_free(table[j]);
                        }
                        sqlite3_free(table);
                        table = NULL;
                        break;
                    }
                    to_copy = (size_t)cell_len;
                    if (to_copy > Size - off) to_copy = Size - off;
                    if (to_copy > 0) memcpy(table[i], Data + off, to_copy);
                    if (cell_len > to_copy) memset(table[i] + to_copy, 'B', (size_t)cell_len - to_copy);
                    table[i][cell_len] = '\0';
                    off += to_copy;
                }
            }
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
    sqlite3_free_table(table);

    sqlite3_free(NULL);
    sqlite3_free_filename(NULL);
    sqlite3_free_table(NULL);

    return 0;
}