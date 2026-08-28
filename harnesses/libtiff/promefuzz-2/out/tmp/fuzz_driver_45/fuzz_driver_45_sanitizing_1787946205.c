#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "tiffio.h"

static uint64_t read_u64(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    size_t i;
    size_t lim = n < 8 ? n : 8;
    for (i = 0; i < lim; ++i) {
        v = (v << 8) | p[i];
    }
    return v;
}

static tmsize_t bounded_size(uint64_t v, tmsize_t maxv) {
    if (maxv <= 0) return 0;
    return (tmsize_t)(v % (uint64_t)(maxv + 1));
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    uint8_t localbuf[256];
    void *p1 = NULL;
    void *p2 = NULL;
    void *p3 = NULL;
    tmsize_t s1, s2, s3, copy1, copy2, set1, set2;
    uint64_t seed1 = 0, seed2 = 0, seed3 = 0;
    size_t offset = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size >= 8) {
        seed1 = read_u64(Data, 8);
        offset += 8;
    }
    if (Size >= 16) {
        seed2 = read_u64(Data + 8, 8);
        offset += 8;
    }
    if (Size >= 24) {
        seed3 = read_u64(Data + 16, 8);
        offset += 8;
    }

    s1 = bounded_size(seed1, 4096);
    s2 = bounded_size(seed2, 4096);
    s3 = bounded_size(seed3, 4096);

    p1 = _TIFFmalloc(s1);
    p2 = _TIFFcalloc((tmsize_t)((s2 % 64) + 1), (tmsize_t)(s2 / ((s2 % 64) + 1 ? (s2 % 64) + 1 : 1)));
    if (s2 == 0) {
        _TIFFfree(p2);
        p2 = NULL;
    }

    if (p1 != NULL && s1 > 0) {
        int fill = (Size > offset) ? Data[offset] : 0x41;
        _TIFFmemset(p1, fill, s1);
    }

    if (p2 != NULL && s2 > 0) {
        int fill = (Size > offset + 1) ? Data[offset + 1] : 0x5A;
        _TIFFmemset(p2, fill, s2);
    }

    if (p1 != NULL && p2 != NULL) {
        copy1 = s1 < s2 ? s1 : s2;
        if (copy1 > 0) {
            _TIFFmemcpy(p2, p1, copy1);
        }
    }

    p1 = _TIFFrealloc(p1, s3);
    if (p1 != NULL && s3 > 0) {
        set1 = bounded_size(seed1 ^ seed3, s3);
        if (set1 > 0) {
            int fill = (Size > offset + 2) ? Data[offset + 2] : 0x11;
            _TIFFmemset(p1, fill, set1);
        }
    }

    p3 = _TIFFrealloc(NULL, s2);
    if (p3 != NULL && s2 > 0) {
        if (Size > offset) {
            copy2 = (tmsize_t)((Size - offset) < (size_t)s2 ? (Size - offset) : (size_t)s2);
            if (copy2 > 0) {
                _TIFFmemcpy(p3, Data + offset, copy2);
            }
            if (s2 > copy2) {
                _TIFFmemset((uint8_t *)p3 + copy2, 0, s2 - copy2);
            }
        } else {
            _TIFFmemset(p3, 0, s2);
        }
    }

    if (p1 != NULL && p3 != NULL) {
        tmsize_t min13 = s3 < s2 ? s3 : s2;
        if (min13 > 0) {
            _TIFFmemcpy(p1, p3, min13);
        }
    }

    memset(localbuf, 0, sizeof(localbuf));
    if (p1 != NULL && s3 > 0) {
        set2 = s3 < (tmsize_t)sizeof(localbuf) ? s3 : (tmsize_t)sizeof(localbuf);
        if (set2 > 0) {
            _TIFFmemcpy(localbuf, p1, set2);
        }
    }

    _TIFFfree(p3);
    _TIFFfree(p2);
    _TIFFfree(p1);

    _TIFFfree(NULL);
    p2 = _TIFFmalloc(0);
    _TIFFfree(p2);
    p3 = _TIFFcalloc(0, 16);
    _TIFFfree(p3);
    p1 = _TIFFcalloc(16, 0);
    _TIFFfree(p1);

    return 0;
}