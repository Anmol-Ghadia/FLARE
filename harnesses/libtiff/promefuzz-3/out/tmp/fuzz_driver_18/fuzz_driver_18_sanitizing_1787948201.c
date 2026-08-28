#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static uint32_t read_u32(const uint8_t *data, size_t size, size_t off) {
    uint32_t v = 0;
    if (off < size) v |= (uint32_t)data[off];
    if (off + 1 < size) v |= (uint32_t)data[off + 1] << 8;
    if (off + 2 < size) v |= (uint32_t)data[off + 2] << 16;
    if (off + 3 < size) v |= (uint32_t)data[off + 3] << 24;
    return v;
}

static tmsize_t bounded_size(uint32_t v, tmsize_t maxv) {
    if (maxv <= 0) return 0;
    return (tmsize_t)(v % (uint32_t)maxv);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        void *p0 = _TIFFmalloc(0);
        _TIFFfree(p0);
        p0 = _TIFFcalloc(0, 16);
        _TIFFfree(p0);
        _TIFFfree(NULL);
        return 0;
    }

    uint32_t v0 = read_u32(Data, Size, 0);
    uint32_t v1 = read_u32(Data, Size, 4);
    uint32_t v2 = read_u32(Data, Size, 8);
    uint32_t v3 = read_u32(Data, Size, 12);

    tmsize_t alloc1 = bounded_size(v0, 1 << 16);
    tmsize_t alloc2 = bounded_size(v1, 1 << 16);
    tmsize_t copysz = bounded_size(v2, 1 << 15);
    tmsize_t c_n = bounded_size(v3, 1024);
    tmsize_t c_s = bounded_size(v0 ^ v1, 256);

    void *buf1 = _TIFFmalloc(alloc1);
    void *buf2 = _TIFFcalloc(c_n, c_s);

    if (buf1 && alloc1 > 0) {
        _TIFFmemset(buf1, (int)(v0 & 0xFF), alloc1);
    }
    if (buf2 && c_n > 0 && c_s > 0) {
        tmsize_t total = c_n * c_s;
        _TIFFmemset(buf2, (int)(v1 & 0xFF), total);
    }

    if (buf1 && buf2) {
        tmsize_t maxcopy = alloc1;
        tmsize_t total2 = c_n * c_s;
        if (total2 < maxcopy) maxcopy = total2;
        if (copysz > maxcopy) copysz = maxcopy;
        if (copysz > 0) {
            _TIFFmemcpy(buf1, buf2, copysz);
        }
    }

    (void)TIFFWriteBufferSetup(tif, NULL, (tmsize_t)-1);

    if (buf1) {
        (void)TIFFWriteBufferSetup(tif, buf1, alloc1);
        buf1 = NULL;
    }

    if (buf2) {
        tmsize_t setupsz = c_n * c_s;
        (void)TIFFWriteBufferSetup(tif, buf2, setupsz);
        buf2 = NULL;
    }

    {
        void *tmp = _TIFFmalloc(32);
        if (tmp) {
            _TIFFmemset(tmp, 0xAA, 32);
            (void)TIFFWriteBufferSetup(tif, tmp, 32);
            tmp = NULL;
        }
    }

    (void)TIFFWriteBufferSetup(tif, NULL, 0);
    (void)TIFFWriteBufferSetup(tif, NULL, 1);
    (void)TIFFWriteBufferSetup(tif, NULL, 8192);

    TIFFClose(tif);

    _TIFFfree(buf1);
    _TIFFfree(buf2);
    _TIFFfree(NULL);

    return 0;
}