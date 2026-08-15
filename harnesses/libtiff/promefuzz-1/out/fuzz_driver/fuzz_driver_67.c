// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFcalloc at tif_unix.c:317:7 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFrealloc at tif_unix.c:332:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFcalloc at tif_unix.c:317:7 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFrealloc at tif_unix.c:332:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFrealloc at tif_unix.c:332:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tiffio.h"

static tmsize_t clamp_size(size_t v, tmsize_t maxv) {
    if ((uint64_t)v > (uint64_t)maxv)
        return maxv;
    return (tmsize_t)v;
}

static uint32_t read_u32(const uint8_t *p, size_t n) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && i < n; i++) {
        v = (v << 8) | p[i];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    void *p1 = NULL;
    void *p2 = NULL;
    void *p3 = NULL;
    void *tmp = NULL;
    tmsize_t s1, s2, s3, copy_len, cmp_len;
    size_t off = 0;
    int cmp_result;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (Size == 0) {
        p1 = _TIFFmalloc(0);
        _TIFFfree(p1);
        p2 = _TIFFcalloc(0, 1);
        _TIFFfree(p2);
        p3 = _TIFFrealloc(NULL, 0);
        _TIFFfree(p3);
        return 0;
    }

    s1 = clamp_size(Size, 1 << 20);
    p1 = _TIFFmalloc(s1);
    if (p1 != NULL && s1 > 0) {
        _TIFFmemcpy(p1, Data, s1);
    }

    if (Size >= 4)
        off = read_u32(Data, Size) % Size;
    else
        off = 0;

    s2 = clamp_size(Size - off, 1 << 20);
    p2 = _TIFFcalloc(s2 == 0 ? 0 : 1, s2);
    if (p2 != NULL && s2 > 0) {
        _TIFFmemcpy(p2, Data + off, s2);
    }

    cmp_len = (s1 < s2) ? s1 : s2;
    if (p1 != NULL && p2 != NULL && cmp_len > 0) {
        cmp_result = _TIFFmemcmp(p1, p2, cmp_len);
        if (cmp_result == 0 && s1 > 0) {
            ((unsigned char *)p1)[0] ^= 0xFF;
            (void)_TIFFmemcmp(p1, p2, cmp_len);
        }
    }

    if (Size >= 8)
        s3 = clamp_size(read_u32(Data + 4, Size - 4), 1 << 20);
    else
        s3 = clamp_size(Size / 2, 1 << 20);

    if (s3 == 0) {
        _TIFFfree(p1);
        p1 = NULL;
    } else {
        tmp = _TIFFrealloc(p1, s3);
        if (tmp != NULL) {
            p1 = tmp;
            if (Size > 0) {
                copy_len = s3 < (tmsize_t)Size ? s3 : (tmsize_t)Size;
                _TIFFmemcpy(p1, Data, copy_len);
            }
        }
    }

    tmp = (s2 == 0) ? NULL : _TIFFrealloc(NULL, s2);
    if (tmp != NULL) {
        p3 = tmp;
        if (p2 != NULL && s2 > 0) {
            _TIFFmemcpy(p3, p2, s2);
            (void)_TIFFmemcmp(p3, p2, s2);
        }
    }

    if (p1 != NULL && p3 != NULL) {
        copy_len = s3 < s2 ? s3 : s2;
        if (copy_len > 0) {
            _TIFFmemcpy(p3, p1, copy_len);
            (void)_TIFFmemcmp(p1, p3, copy_len);
        }
    }

    _TIFFfree(p3);
    _TIFFfree(p2);
    _TIFFfree(p1);
    return 0;
}