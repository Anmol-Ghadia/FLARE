// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    TIFF *tif2 = NULL;
    uint32_t imagewidth = 0, imagelength = 0;
    uint32_t tilewidth = 0, tilelength = 0;
    uint16_t bitspersample = 0, samplesperpixel = 0;
    tmsize_t tilesize1 = 0, tilesize2 = 0;
    void *buf1 = NULL, *buf2 = NULL;
    uint32_t *rgba = NULL;
    uint32_t rgba_tile_w = 0, rgba_tile_h = 0;
    size_t rgba_count = 0;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);
        (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
        (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);

        tilesize1 = TIFFTileSize(tif);
        if (tilesize1 > 0 && tilesize1 < (tmsize_t)(1 << 24)) {
            buf1 = malloc((size_t)tilesize1);
            if (buf1) {
                memset(buf1, 0, (size_t)tilesize1);
                (void)TIFFReadEncodedTile(tif, 0, buf1, tilesize1);
            }
        } else {
            (void)TIFFReadEncodedTile(tif, 0, NULL, 0);
        }

        if (tilewidth == 0 && Size >= 4) {
            tilewidth = ((uint32_t)Data[0] << 8) | Data[1];
            if (tilewidth == 0) tilewidth = 16;
        }
        if (tilelength == 0 && Size >= 6) {
            tilelength = ((uint32_t)Data[2] << 8) | Data[3];
            if (tilelength == 0) tilelength = 16;
        }
        if (bitspersample == 0) {
            bitspersample = 8;
        }
        if (samplesperpixel == 0) {
            samplesperpixel = 1;
        }

        (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilewidth ? tilewidth : 16);
        (void)TIFFSetField(tif, TIFFTAG_TILELENGTH, tilelength ? tilelength : 16);

        tilesize2 = TIFFTileSize(tif);
        if (tilesize2 > 0 && tilesize2 < (tmsize_t)(1 << 24)) {
            buf2 = malloc((size_t)tilesize2);
            if (buf2) {
                memset(buf2, 0xA5, (size_t)tilesize2);
                (void)TIFFReadEncodedTile(tif, 0, buf2, tilesize2);
            }
        } else {
            (void)TIFFReadEncodedTile(tif, 0, NULL, 0);
        }

        TIFFClose(tif);
        tif = NULL;
    }

    free(buf1);
    free(buf2);
    buf1 = NULL;
    buf2 = NULL;

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        rgba_tile_w = 0;
        rgba_tile_h = 0;
        (void)TIFFGetField(tif2, TIFFTAG_TILEWIDTH, &rgba_tile_w);
        (void)TIFFGetField(tif2, TIFFTAG_TILELENGTH, &rgba_tile_h);

        if (rgba_tile_w == 0) rgba_tile_w = 16;
        if (rgba_tile_h == 0) rgba_tile_h = 16;

        if (rgba_tile_w <= 4096 && rgba_tile_h <= 4096) {
            rgba_count = (size_t)rgba_tile_w * (size_t)rgba_tile_h;
            if (rgba_count > 0 && rgba_count <= (1U << 22)) {
                rgba = (uint32_t *)malloc(rgba_count * sizeof(uint32_t));
                if (rgba) {
                    memset(rgba, 0, rgba_count * sizeof(uint32_t));
                    (void)TIFFReadRGBATile(tif2, 0, 0, rgba);
                }
            }
        }

        TIFFClose(tif2);
        tif2 = NULL;
    }

    free(rgba);
    return 0;
}