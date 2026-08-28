// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFCheckTile at tif_tile.c:73:1 in tiffio.h
// TIFFReadTile at tif_read.c:900:1 in tiffio.h
// TIFFReadTile at tif_read.c:900:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFCheckTile at tif_tile.c:73:1 in tiffio.h
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
    FILE *fp;
    TIFF *tif = NULL;
    uint32_t width = 0, height = 0, depth = 1;
    uint32_t tilewidth = 0, tilelength = 0;
    uint16_t samplesperpixel = 1, planarconfig = PLANARCONFIG_CONTIG;
    uint32_t x1, y1, z1, col1, row1;
    uint16_t s1;
    uint32_t x2, y2, z2, col2, row2;
    uint16_t s2;
    uint32_t *rgba = NULL;
    tmsize_t tile_size = 0;
    void *tile_buf = NULL;
    uint32_t tile_index = 0;
    size_t rgba_count = 0;
    int stop1, stop2;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEDEPTH, &depth);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);
    (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);

    if (width == 0) width = 1;
    if (height == 0) height = 1;
    if (depth == 0) depth = 1;

    rgba_count = (size_t)width * (size_t)height;
    if (rgba_count != 0 && rgba_count <= (1U << 24)) {
        rgba = (uint32_t *)malloc(rgba_count * sizeof(uint32_t));
        if (rgba) {
            stop1 = (Size > 0) ? (Data[0] & 1) : 0;
            stop2 = (Size > 1) ? (Data[1] & 1) : 1;
            (void)TIFFReadRGBAImage(tif, width, height, rgba, stop1);
            (void)TIFFReadRGBAImage(tif, width, height, rgba, stop2);
        }
    }

    if (TIFFIsTiled(tif)) {
        tile_size = TIFFTileSize(tif);
        if (tile_size > 0 && tile_size <= (tmsize_t)(1 << 26)) {
            tile_buf = malloc((size_t)tile_size);
        }

        if (tilewidth == 0) tilewidth = 1;
        if (tilelength == 0) tilelength = 1;

        x1 = (width > 0) ? ((Size > 4 ? rd32(Data, Size, 2) : 0) % width) : 0;
        y1 = (height > 0) ? ((Size > 8 ? rd32(Data, Size, 6) : 0) % height) : 0;
        z1 = (depth > 0) ? ((Size > 12 ? rd32(Data, Size, 10) : 0) % depth) : 0;
        s1 = (planarconfig == PLANARCONFIG_SEPARATE && samplesperpixel > 0)
                 ? (uint16_t)((Size > 14 ? rd16(Data, Size, 14) : 0) % samplesperpixel)
                 : 0;

        x2 = width ? width : 0;
        y2 = height ? height : 0;
        z2 = depth ? depth : 0;
        s2 = samplesperpixel;

        (void)TIFFCheckTile(tif, x1, y1, z1, s1);
        (void)TIFFCheckTile(tif, x2, y2, z2, s2);

        if (tile_buf) {
            (void)TIFFReadTile(tif, tile_buf, x1, y1, z1, s1);
            (void)TIFFReadTile(tif, tile_buf, x2, y2, z2, s2);

            tile_index = TIFFComputeTile(tif, x1, y1, z1, s1);
            (void)TIFFReadEncodedTile(tif, tile_index, tile_buf, tile_size);
            (void)TIFFReadEncodedTile(tif, tile_index, tile_buf, tile_size / 2);
            (void)TIFFReadEncodedTile(tif, tile_index, tile_buf, (tmsize_t)-1);
        }

        if (tilewidth > 0 && tilelength > 0) {
            col1 = (x1 / tilewidth) * tilewidth;
            row1 = (y1 / tilelength) * tilelength;
        } else {
            col1 = 0;
            row1 = 0;
        }

        col2 = width;
        row2 = height;

        if (tilewidth > 0 && tilelength > 0) {
            size_t tile_pixels = (size_t)tilewidth * (size_t)tilelength;
            if (tile_pixels != 0 && tile_pixels <= (1U << 24)) {
                uint32_t *tile_rgba = (uint32_t *)malloc(tile_pixels * sizeof(uint32_t));
                if (tile_rgba) {
                    (void)TIFFReadRGBATile(tif, col1, row1, tile_rgba);
                    (void)TIFFReadRGBATileExt(tif, col1, row1, tile_rgba,
                                              (Size > 16) ? (Data[16] & 1) : 0);
                    (void)TIFFReadRGBATile(tif, col2, row2, tile_rgba);
                    (void)TIFFReadRGBATileExt(tif, col2, row2, tile_rgba,
                                              (Size > 17) ? (Data[17] & 1) : 1);
                    free(tile_rgba);
                }
            }
        }
    } else {
        if (rgba) {
            (void)TIFFReadRGBATile(tif, 0, 0, rgba);
            (void)TIFFReadRGBATileExt(tif, 0, 0, rgba, 0);
        }
    }

    free(tile_buf);
    free(rgba);
    TIFFClose(tif);
    return 0;
}