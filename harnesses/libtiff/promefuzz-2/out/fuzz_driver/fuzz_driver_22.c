// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 2 > n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 4 > n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void write_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void fuzz_read_paths(void) {
    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) return;

    (void)TIFFCurrentStrip(tif);
    (void)TIFFNumberOfStrips(tif);
    (void)TIFFStripSize(tif);

    uint32_t nstrips = TIFFNumberOfStrips(tif);
    tmsize_t stripsz = TIFFStripSize(tif);
    if (stripsz < 0) stripsz = 0;

    if (nstrips > 0) {
        size_t allocsz = (stripsz > 0 && stripsz < (tmsize_t)(1 << 20)) ? (size_t)stripsz : (size_t)4096;
        if (allocsz == 0) allocsz = 1;
        void *buf = malloc(allocsz);
        if (buf) {
            uint32_t strip = 0;
            (void)TIFFReadEncodedStrip(tif, strip, buf, (tmsize_t)allocsz);
            (void)TIFFCurrentStrip(tif);
            (void)TIFFReadEncodedStrip(tif, strip, buf, (tmsize_t)-1);
            (void)TIFFCurrentStrip(tif);
            if (nstrips > 1) {
                strip = nstrips - 1;
                (void)TIFFReadEncodedStrip(tif, strip, buf, (tmsize_t)allocsz);
                (void)TIFFCurrentStrip(tif);
            }
            free(buf);
        }
    }

    if (TIFFIsTiled(tif)) {
        uint32_t tw = 0, tl = 0;
        uint16_t spp = 1, bps = 8;
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tl);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);

        tmsize_t tilesz = TIFFTileSize(tif);
        if (tilesz < 0) tilesz = 0;
        size_t allocsz = (tilesz > 0 && tilesz < (tmsize_t)(1 << 20)) ? (size_t)tilesz : (size_t)4096;
        if (allocsz == 0) allocsz = 1;

        void *buf = malloc(allocsz);
        if (buf) {
            uint32_t tile = 0;
            (void)tw;
            (void)tl;
            (void)spp;
            (void)bps;
            (void)TIFFReadEncodedTile(tif, tile, buf, (tmsize_t)allocsz);
            (void)TIFFReadEncodedTile(tif, tile, buf, (tmsize_t)-1);
            free(buf);
        }
    }

    TIFFClose(tif);
}

static void fuzz_write_paths(const uint8_t *Data, size_t Size) {
    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return;

    uint32_t width = 1 + (Size > 0 ? (uint32_t)(Data[0] % 64) : 1);
    uint32_t height = 1 + (Size > 1 ? (uint32_t)(Data[1] % 64) : 1);
    uint16_t spp = 1 + (Size > 2 ? (uint16_t)(Data[2] % 4) : 0);
    uint16_t bps_choices[4] = {8, 16, 1, 32};
    uint16_t bps = bps_choices[(Size > 3 ? Data[3] : 0) % 4];
    uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (Size > 4 && (Data[4] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint32_t rowsperstrip = 1 + (Size > 5 ? (uint32_t)(Data[5] % height) : 0);
    uint16_t compression = COMPRESSION_NONE;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    (void)TIFFCurrentStrip(tif);
    uint32_t nstrips = TIFFNumberOfStrips(tif);
    tmsize_t stripsz = TIFFStripSize(tif);
    if (stripsz <= 0 || stripsz > (tmsize_t)(1 << 20)) {
        stripsz = (tmsize_t)(width * spp * ((bps + 7) / 8) * rowsperstrip);
        if (stripsz <= 0) stripsz = 1;
        if (stripsz > (tmsize_t)(1 << 20)) stripsz = (tmsize_t)(1 << 20);
    }

    void *buf = malloc((size_t)stripsz);
    if (buf) {
        if (Size > 0) {
            for (size_t i = 0; i < (size_t)stripsz; ++i)
                ((uint8_t *)buf)[i] = Data[i % Size];
        } else {
            memset(buf, 0, (size_t)stripsz);
        }

        (void)TIFFWriteEncodedStrip(tif, 0, buf, stripsz);
        (void)TIFFCurrentStrip(tif);

        if (nstrips > 1) {
            (void)TIFFWriteEncodedStrip(tif, nstrips - 1, buf, stripsz);
            (void)TIFFCurrentStrip(tif);
        } else {
            (void)TIFFWriteEncodedStrip(tif, 1, buf, stripsz / 2);
            (void)TIFFCurrentStrip(tif);
        }

        free(buf);
    }

    TIFFClose(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_file(Data, Size);
    fuzz_read_paths();
    fuzz_write_paths(Data, Size);

    if (Size > 8) {
        uint32_t off = rd32(Data, Size, 0, 0) % (uint32_t)Size;
        uint16_t len = rd16(Data, Size, 4, 0);
        if (off < Size) {
            size_t sublen = Size - off;
            if (len < sublen) sublen = len;
            write_file(Data + off, sublen);
            fuzz_read_paths();
        }
    }

    remove("./dummy_file");
    return 0;
}