// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
// sqlite3_strglob at sqlite3.c:107931:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static char *make_cstring(const uint8_t *data, size_t len) {
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
    uint8_t null_flags = 0;
    size_t rem;
    size_t len1 = 0, len2 = 0, len3 = 0, len4 = 0;
    char *buf1 = NULL, *buf2 = NULL, *buf3 = NULL, *buf4 = NULL;
    const char *s1 = NULL, *s2 = NULL;
    const char *g1 = NULL, *x1 = NULL, *g2 = NULL, *x2 = NULL;

    sqlite3_shutdown();

    if (off < Size) mode = Data[off++];
    if (off < Size) null_flags = Data[off++];

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
            (void)sqlite3_config(-1);
            break;
    }

    rem = (off < Size) ? (Size - off) : 0;
    if (rem > 0) len1 = Data[off++] % (rem + 1);

    rem = (off < Size) ? (Size - off) : 0;
    if (rem > 0) len2 = Data[off++] % (rem + 1);

    rem = (off < Size) ? (Size - off) : 0;
    if (rem > 0) len3 = Data[off++] % (rem + 1);

    rem = (off < Size) ? (Size - off) : 0;
    if (rem > 0) len4 = Data[off++] % (rem + 1);

    rem = (off < Size) ? (Size - off) : 0;
    if (len1 > rem) len1 = rem;
    buf1 = make_cstring(Data + off, len1);
    off += len1;

    rem = (off < Size) ? (Size - off) : 0;
    if (len2 > rem) len2 = rem;
    buf2 = make_cstring(Data + off, len2);
    off += len2;

    rem = (off < Size) ? (Size - off) : 0;
    if (len3 > rem) len3 = rem;
    buf3 = make_cstring(Data + off, len3);
    off += len3;

    rem = (off < Size) ? (Size - off) : 0;
    if (len4 > rem) len4 = rem;
    buf4 = make_cstring(Data + off, len4);
    off += len4;

    s1 = (null_flags & 0x01) ? NULL : (buf1 ? buf1 : "");
    s2 = (null_flags & 0x02) ? NULL : (buf2 ? buf2 : "");

    g1 = (null_flags & 0x04) ? "" : (buf2 ? buf2 : "");
    x1 = (null_flags & 0x08) ? "" : (buf3 ? buf3 : "");
    g2 = (null_flags & 0x10) ? "" : (buf3 ? buf3 : "");
    x2 = (null_flags & 0x20) ? "" : (buf4 ? buf4 : "");

    (void)sqlite3_stricmp(s1, s2);
    (void)sqlite3_strglob(g1, x1);
    (void)sqlite3_strglob(g2, x2);

    free(buf1);
    free(buf2);
    free(buf3);
    free(buf4);

    sqlite3_shutdown();
    return 0;
}