// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFmemset at tif_unix.c:338:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFcalloc at tif_unix.c:317:7 in tiffio.h
// _TIFFmemset at tif_unix.c:338:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmemset at tif_unix.c:338:1 in tiffio.h
// TIFFReadBufferSetup at tif_read.c:1279:1 in tiffio.h
// TIFFReadBufferSetup at tif_read.c:1279:1 in tiffio.h
// _TIFFcalloc at tif_unix.c:317:7 in tiffio.h
// _TIFFmemset at tif_unix.c:338:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFmemset at tif_unix.c:338:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_73(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "rm");
    if (!tif)
        return 0;

    size_t off = 0;

    tmsize_t setup_size = 0;
    if (Size >= 8) {
        setup_size = (tmsize_t)(rd32(Data) & 0xFFFF);
        off = 4;
    } else {
        setup_size = 1;
    }

    if ((off < Size) && (Data[off] & 1)) {
        void *buf = NULL;
        if (setup_size > 0) {
            buf = _TIFFmalloc(setup_size);
            if (buf) {
                int fill = (off + 1 < Size) ? Data[off + 1] : 0xA5;
                _TIFFmemset(buf, fill, setup_size);
            }
        }
        (void)TIFFReadBufferSetup(tif, buf, setup_size);
    } else {
        (void)TIFFReadBufferSetup(tif, NULL, setup_size);
    }

    if (Size - off >= 4) {
        tmsize_t n = (tmsize_t)(rd16(Data + off) & 0x0FFF);
        tmsize_t s = (tmsize_t)((rd16(Data + off + 2) & 0x00FF) + 1);
        void *a = _TIFFcalloc(n, s);
        if (a) {
            _TIFFmemset(a, (off + 4 < Size) ? Data[off + 4] : 0, n * s);
            if (n * s > 0) {
                void *b = _TIFFmalloc(n * s);
                if (b) {
                    _TIFFmemcpy(b, a, n * s);
                    (void)_TIFFmemcmp(a, b, n * s);
                    _TIFFmemset(b, 0x5A, n * s);
                    (void)_TIFFmemcmp(a, b, n * s);
                    _TIFFfree(b);
                }
            }
            _TIFFfree(a);
        }
        off += 4;
    }

    if (Size - off >= 2) {
        uint16_t bps = (uint16_t)((rd16(Data + off) % 16) + 1);
        off += 2;
        if (bps < (sizeof(size_t) * CHAR_BIT)) {
            size_t entries = ((size_t)1) << bps;
            if (entries <= SIZE_MAX / sizeof(uint16_t)) {
                tmsize_t chan_bytes = (tmsize_t)(entries * sizeof(uint16_t));
                uint16_t *r = (uint16_t *)_TIFFmalloc(chan_bytes);
                uint16_t *g = (uint16_t *)_TIFFmalloc(chan_bytes);
                uint16_t *b = (uint16_t *)_TIFFmalloc(chan_bytes);
                if (r && g && b) {
                    size_t i;
                    for (i = 0; i < entries; i++) {
                        uint16_t v = (uint16_t)((i < Size) ? Data[i] * 257U : (uint16_t)i);
                        r[i] = v;
                        g[i] = (uint16_t)(v ^ 0x5555U);
                        b[i] = (uint16_t)(v ^ 0xAAAAU);
                    }
                    (void)_TIFFmemcmp(r, g, chan_bytes < 32 ? chan_bytes : 32);
                    _TIFFmemcpy(g, r, chan_bytes);
                    (void)_TIFFmemcmp(r, g, chan_bytes);
                    _TIFFmemset(b, 0, chan_bytes);
                }
                if (r) _TIFFfree(r);
                if (g) _TIFFfree(g);
                if (b) _TIFFfree(b);
            }
        }
    }

    if (Size > 0) {
        tmsize_t small = (tmsize_t)((Data[0] % 64) + 1);
        void *x = _TIFFmalloc(small);
        void *y = _TIFFcalloc(small, 1);
        if (x && y) {
            _TIFFmemset(x, 0x11, small);
            _TIFFmemcpy(y, x, small);
            (void)_TIFFmemcmp(x, y, small);
        }
        if (x) _TIFFfree(x);
        if (y) _TIFFfree(y);
    }

    TIFFClose(tif);
    return 0;
}