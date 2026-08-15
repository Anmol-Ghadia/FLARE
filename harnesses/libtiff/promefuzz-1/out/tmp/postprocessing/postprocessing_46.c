#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "tiffio.h"

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *off < size; i++, (*off)++) {
        v = (v << 8) | data[*off];
    }
    return v;
}

static tmsize_t bounded_tmsize(uint64_t v, tmsize_t maxv) {
    if (maxv <= 0) return 0;
    return (tmsize_t)(v % (uint64_t)(maxv + 1));
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    FILE *fp;
    void *p1 = NULL, *p2 = NULL, *p3 = NULL, *tmp = NULL;
    tmsize_t s1, s2, s3, copy1, copy2, set1, set2;
    int fill1, fill2;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    s1 = bounded_tmsize(read_u64(Data, Size, &off), 4096);
    s2 = bounded_tmsize(read_u64(Data, Size, &off), 4096);
    s3 = bounded_tmsize(read_u64(Data, Size, &off), 8192);
    copy1 = bounded_tmsize(read_u64(Data, Size, &off), 4096);
    copy2 = bounded_tmsize(read_u64(Data, Size, &off), 4096);
    set1 = bounded_tmsize(read_u64(Data, Size, &off), 4096);
    set2 = bounded_tmsize(read_u64(Data, Size, &off), 4096);
    fill1 = (off < Size) ? Data[off++] : 0x41;
    fill2 = (off < Size) ? Data[off++] : 0x5a;

    p1 = _TIFFmalloc(s1);
    if (p1 != NULL && s1 > 0) {
        _TIFFmemset(p1, fill1, (set1 <= s1) ? set1 : s1);
    }

    p2 = _TIFFcalloc(s2, 1);
    if (p2 != NULL && s2 > 0) {
        _TIFFmemset(p2, fill2, (set2 <= s2) ? set2 : s2);
    }

    if (p1 != NULL && p2 != NULL && s1 > 0 && s2 > 0) {
        tmsize_t n = copy1;
        if (n > s1) n = s1;
        if (n > s2) n = s2;
        _TIFFmemcpy(p1, p2, n);
    }

    tmp = _TIFFrealloc(p1, s3);
    if (tmp != NULL) {
        p1 = tmp;
        if (s3 > 0) {
            _TIFFmemset(p1, fill1 ^ fill2, (s3 < 64) ? s3 : 64);
        }
    } else if (s3 == 0) {
        p1 = NULL;
    }

    p3 = _TIFFrealloc(NULL, s2);
    if (p3 != NULL && p2 != NULL && s2 > 0) {
        tmsize_t n = copy2;
        if (n > s2) n = s2;
        _TIFFmemcpy(p3, p2, n);
    }

    tmp = _TIFFrealloc(p2, s1);
    if (tmp != NULL) {
        p2 = tmp;
        if (p1 != NULL && s1 > 0 && s3 > 0) {
            tmsize_t n = s1 < s3 ? s1 : s3;
            _TIFFmemcpy(p2, p1, n);
        }
    } else if (s1 == 0) {
        p2 = NULL;
    }

    _TIFFfree(p3);
    _TIFFfree(p2);
    _TIFFfree(p1);
    _TIFFfree(NULL);

    return 0;
}