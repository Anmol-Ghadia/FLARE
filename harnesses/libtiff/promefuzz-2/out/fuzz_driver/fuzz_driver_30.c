// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteTile at tif_write.c:364:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteRawTile at tif_write.c:503:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadTile at tif_read.c:900:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tiffio.h"

static uint16_t rd16(const uint8_t *p, size_t n) {
    if (n < 2) return 0;
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n) {
    if (n < 4) {
        uint32_t v = 0;
        for (size_t i = 0; i < n; i++) v |= ((uint32_t)p[i]) << (8 * i);
        return v;
    }
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void write_file(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (!f) return;
    if (Size > 0) fwrite(Data, 1, Size, f);
    fclose(f);
}

static void setup_tiled_writer(TIFF *tif, const uint8_t *Data, size_t Size) {
    uint32_t width = (Size > 0) ? (uint32_t)(Data[0] % 64 + 1) : 16;
    uint32_t height = (Size > 1) ? (uint32_t)(Data[1] % 64 + 1) : 16;
    uint32_t tilew = (Size > 2) ? (uint32_t)(1U << ((Data[2] % 4) + 4)) : 16;   /* 16,32,64,128 */
    uint32_t tileh = (Size > 3) ? (uint32_t)(1U << ((Data[3] % 4) + 4)) : 16;
    uint16_t spp = (Size > 4) ? (uint16_t)(Data[4] % 4 + 1) : 1;
    uint16_t bps = (Size > 5 && (Data[5] & 1)) ? 16 : 8;
    uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (Size > 6 && (Data[6] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
    TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, height > 0 ? height : 1);
}

static void setup_strip_writer(TIFF *tif, const uint8_t *Data, size_t Size) {
    uint32_t width = (Size > 7) ? (uint32_t)(Data[7] % 64 + 1) : 16;
    uint32_t height = (Size > 8) ? (uint32_t)(Data[8] % 64 + 1) : 16;
    uint16_t spp = (Size > 9) ? (uint16_t)(Data[9] % 4 + 1) : 1;
    uint16_t bps = (Size > 10 && (Data[10] & 1)) ? 16 : 8;
    uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (Size > 11 && (Data[11] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = (Size > 12) ? (uint32_t)(Data[12] % (height ? height : 1) + 1) : height;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip ? rowsperstrip : 1);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_file(Data, Size);

    {
        TIFF *tifw = TIFFOpen("./dummy_file", "w");
        if (tifw) {
            setup_tiled_writer(tifw, Data, Size);

            tmsize_t tilesize = TIFFTileSize(tifw);
            if (tilesize <= 0) tilesize = (Size > 0) ? (tmsize_t)Size : 1;

            void *tilebuf = NULL;
            if (tilesize > 0 && tilesize < (tmsize_t)(1 << 20)) {
                tilebuf = _TIFFmalloc((tmsize_t)tilesize);
            }
            if (tilebuf) {
                memset(tilebuf, 0, (size_t)tilesize);
                if (Size > 0) {
                    size_t copy = (Size < (size_t)tilesize) ? Size : (size_t)tilesize;
                    memcpy(tilebuf, Data, copy);
                }

                uint32_t x = (Size > 16) ? rd32(Data + 13, Size - 13) : 0;
                uint32_t y = (Size > 20) ? rd32(Data + 17, Size - 17) : 0;
                uint32_t z = 0;
                uint16_t s = (Size > 21) ? (uint16_t)(Data[21] % 4) : 0;
                uint32_t tile = (Size > 25) ? rd32(Data + 22, Size - 22) : 0;
                tmsize_t cc = (Size > 29) ? (tmsize_t)(rd32(Data + 26, Size - 26) % ((uint32_t)tilesize + 1)) : tilesize;

                (void)TIFFWriteTile(tifw, tilebuf, x, y, z, s);
                (void)TIFFWriteEncodedTile(tifw, tile, tilebuf, cc);
                (void)TIFFWriteRawTile(tifw, tile, tilebuf, cc);

                _TIFFfree(tilebuf);
            }

            TIFFClose(tifw);
        }
    }

    {
        TIFF *tifw = TIFFOpen("./dummy_file", "w");
        if (tifw) {
            setup_strip_writer(tifw, Data, Size);

            tmsize_t stripsize = TIFFStripSize(tifw);
            if (stripsize <= 0) stripsize = (Size > 0) ? (tmsize_t)Size : 1;

            void *stripbuf = NULL;
            if (stripsize > 0 && stripsize < (tmsize_t)(1 << 20)) {
                stripbuf = _TIFFmalloc((tmsize_t)stripsize);
            }
            if (stripbuf) {
                memset(stripbuf, 0, (size_t)stripsize);
                if (Size > 0) {
                    size_t copy = (Size < (size_t)stripsize) ? Size : (size_t)stripsize;
                    memcpy(stripbuf, Data, copy);
                }

                uint32_t strip = (Size > 33) ? rd32(Data + 30, Size - 30) : 0;
                tmsize_t cc = (Size > 37) ? (tmsize_t)(rd32(Data + 34, Size - 34) % ((uint32_t)stripsize + 1)) : stripsize;
                (void)TIFFWriteEncodedStrip(tifw, strip, stripbuf, cc);

                _TIFFfree(stripbuf);
            }

            TIFFClose(tifw);
        }
    }

    {
        TIFF *tifr = TIFFOpen("./dummy_file", "r");
        if (tifr) {
            if (TIFFIsTiled(tifr)) {
                tmsize_t tilesize = TIFFTileSize(tifr);
                if (tilesize <= 0) tilesize = (Size > 0) ? (tmsize_t)Size : 1;

                void *buf = NULL;
                if (tilesize > 0 && tilesize < (tmsize_t)(1 << 20)) {
                    buf = _TIFFmalloc((tmsize_t)tilesize);
                }
                if (buf) {
                    uint32_t tile = (Size > 41) ? rd32(Data + 38, Size - 38) : 0;
                    uint32_t x = (Size > 45) ? rd32(Data + 42, Size - 42) : 0;
                    uint32_t y = (Size > 49) ? rd32(Data + 46, Size - 46) : 0;
                    uint32_t z = 0;
                    uint16_t s = (Size > 50) ? (uint16_t)(Data[50] % 4) : 0;
                    tmsize_t readsz = (Size > 54) ? (tmsize_t)(rd32(Data + 51, Size - 51) % ((uint32_t)tilesize + 1)) : tilesize;

                    (void)TIFFReadEncodedTile(tifr, tile, buf, readsz);
                    (void)TIFFReadTile(tifr, buf, x, y, z, s);

                    _TIFFfree(buf);
                }
            }
            TIFFClose(tifr);
        }
    }

    return 0;
}