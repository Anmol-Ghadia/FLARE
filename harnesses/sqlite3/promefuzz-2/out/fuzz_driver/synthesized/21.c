// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static char *make_string(const uint8_t *data, size_t len) {
    char *s = (char *)malloc(len + 1);
    if (!s) return NULL;
    if (len > 0) {
        memcpy(s, data, len);
    }
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput_21(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint8_t mode = 0;
    char *s1 = NULL;
    char *s2 = NULL;
    char *p1 = NULL;
    char *p2 = NULL;

    if (Size > 0) {
        mode = Data[off++];
    }

    sqlite3_shutdown();

    switch (mode % 6) {
        case 0:
            (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
            break;
        case 1:
            (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
            break;
        case 2:
            (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
            break;
        case 3:
            (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (int)(mode & 1));
            break;
        case 4:
            (void)sqlite3_config(SQLITE_CONFIG_URI, (int)(mode & 1));
            break;
        default:
            (void)sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, (int)(mode & 1));
            break;
    }

    {
        size_t rem = (off <= Size) ? (Size - off) : 0;
        size_t l1 = 0, l2 = 0, l3 = 0, l4 = 0;

        if (rem > 0) {
            l1 = Data[off++] % (rem);
            rem = Size - off;
            if (l1 > rem) l1 = rem;
        }
        s1 = make_string(Data + off, l1);
        off += l1;
        rem = (off <= Size) ? (Size - off) : 0;

        if (rem > 0) {
            l2 = Data[off++] % (rem);
            rem = Size - off;
            if (l2 > rem) l2 = rem;
        }
        s2 = make_string(Data + off, l2);
        off += l2;
        rem = (off <= Size) ? (Size - off) : 0;

        if (rem > 0) {
            l3 = Data[off++] % (rem);
            rem = Size - off;
            if (l3 > rem) l3 = rem;
        }
        p1 = make_string(Data + off, l3);
        off += l3;
        rem = (off <= Size) ? (Size - off) : 0;

        if (rem > 0) {
            l4 = Data[off++] % (rem);
            rem = Size - off;
            if (l4 > rem) l4 = rem;
        }
        p2 = make_string(Data + off, l4);
    }

    {
        const char *a = (mode & 0x10) ? NULL : s1;
        const char *b = (mode & 0x20) ? NULL : s2;
        const char *g1 = p1 ? p1 : "";
        const char *g2 = p2 ? p2 : "";
        const char *x1 = a ? a : "";
        const char *x2 = b ? b : "";

        (void)sqlite3_stricmp(a, b);
        (void)sqlite3_strglob(g1, x1);
        (void)sqlite3_strglob(g2, x2);
    }

    free(s1);
    free(s2);
    free(p1);
    free(p2);

    sqlite3_shutdown();
    return 0;
}