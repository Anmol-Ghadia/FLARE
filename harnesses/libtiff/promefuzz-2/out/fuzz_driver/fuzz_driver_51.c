// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFCurrentTile at tif_open.c:631:1 in tiffio.h
// TIFFCurrentTile at tif_open.c:631:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
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

#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    do {
        uint32_t width = 0, length = 0, tilewidth = 0, tilelength = 0;
        uint16_t samplesperpixel = 1;
        uint16_t planarconfig = PLANARCONFIG_CONTIG;
        uint32_t rowsperstrip = 0;

        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &length);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);
        (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);

        uint32_t x = width ? (rd32(Data, Size, 0) % width) : rd32(Data, Size, 0);
        uint32_t y = length ? (rd32(Data, Size, 4) % length) : rd32(Data, Size, 4);
        uint32_t z = rd32(Data, Size, 8);
        uint16_t s = samplesperpixel ? (uint16_t)(rd16(Data, Size, 12) % samplesperpixel) : 0;
        uint16_t s_any = rd16(Data, Size, 14);

        uint32_t computed_strip = TIFFComputeStrip(tif, y, s);
        (void)TIFFComputeStrip(tif, y, s_any);

        {
            tmsize_t strip_size = TIFFStripSize(tif);
            if (strip_size > 0 && strip_size < (tmsize_t)(1 << 26)) {
                void *buf = malloc((size_t)strip_size);
                if (buf) {
                    (void)TIFFReadEncodedStrip(tif, computed_strip, buf, strip_size);
                    (void)TIFFReadEncodedStrip(tif, computed_strip, buf, (tmsize_t)-1);
                    free(buf);
                }
            }
        }

        if (TIFFIsTiled(tif)) {
            uint32_t tx = tilewidth ? (x / tilewidth) * tilewidth : x;
            uint32_t ty = tilelength ? (y / tilelength) * tilelength : y;

            uint32_t tile = TIFFComputeTile(tif, x, y, z, s);
            (void)TIFFComputeTile(tif, x, y, z, s_any);

            tmsize_t tile_size = TIFFTileSize(tif);
            if (tile_size > 0 && tile_size < (tmsize_t)(1 << 26)) {
                void *tbuf = malloc((size_t)tile_size);
                if (tbuf) {
                    (void)TIFFReadEncodedTile(tif, tile, tbuf, tile_size);
                    (void)TIFFReadEncodedTile(tif, tile, tbuf, (tmsize_t)-1);
                    free(tbuf);
                }
            }

            {
                tmsize_t rgba_count = TIFFTileSize(tif);
                if (rgba_count > 0 && rgba_count < (tmsize_t)(1 << 26)) {
                    size_t pixels = (size_t)rgba_count;
                    if (pixels <= ((size_t)-1) / sizeof(uint32_t)) {
                        uint32_t *raster = (uint32_t *)malloc(pixels * sizeof(uint32_t));
                        if (raster) {
                            (void)TIFFReadRGBATile(tif, tx, ty, raster);
                            free(raster);
                        }
                    }
                }
            }

            (void)TIFFCurrentTile(tif);
        } else {
            (void)TIFFCurrentTile(tif);
        }
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}