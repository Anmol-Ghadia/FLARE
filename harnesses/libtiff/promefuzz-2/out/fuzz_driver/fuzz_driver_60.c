// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFCheckTile at tif_tile.c:73:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFCheckTile at tif_tile.c:73:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFReadScanline at tif_read.c:437:1 in tiffio.h
// TIFFReadScanline at tif_read.c:437:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFCheckTile at tif_tile.c:73:1 in tiffio.h
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

#include <tiffio.h>

#ifndef ORIENTATION_TOPLEFT
#define ORIENTATION_TOPLEFT 1
#endif
#ifndef ORIENTATION_TOPRIGHT
#define ORIENTATION_TOPRIGHT 2
#endif
#ifndef ORIENTATION_BOTRIGHT
#define ORIENTATION_BOTRIGHT 3
#endif
#ifndef ORIENTATION_BOTLEFT
#define ORIENTATION_BOTLEFT 4
#endif
#ifndef ORIENTATION_LEFTTOP
#define ORIENTATION_LEFTTOP 5
#endif
#ifndef ORIENTATION_RIGHTTOP
#define ORIENTATION_RIGHTTOP 6
#endif
#ifndef ORIENTATION_RIGHTBOT
#define ORIENTATION_RIGHTBOT 7
#endif
#ifndef ORIENTATION_LEFTBOT
#define ORIENTATION_LEFTBOT 8
#endif

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t off) {
    uint32_t v = 0;
    if (off < Size) v |= (uint32_t)Data[off];
    if (off + 1 < Size) v |= (uint32_t)Data[off + 1] << 8;
    if (off + 2 < Size) v |= (uint32_t)Data[off + 2] << 16;
    if (off + 3 < Size) v |= (uint32_t)Data[off + 3] << 24;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif;
    uint32_t width = 0, height = 0;
    uint32_t tilew = 0, tileh = 0;
    uint16_t samplesperpixel = 1;
    uint16_t bitspersample = 8;
    uint32_t *raster = NULL;
    uint32_t *tilebuf = NULL;
    tmsize_t scanline_size = 0;
    void *scanbuf = NULL;
    uint32_t x1, y1, z1, col1, row1;
    uint32_t x2, y2, z2, col2, row2;
    uint16_t s1, s2;
    uint32_t rw1, rh1, rw2, rh2;
    int stop1, stop2;
    int orient1, orient2;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilew);
    (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileh);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);

    x1 = read_u32(Data, Size, 0);
    y1 = read_u32(Data, Size, 4);
    z1 = read_u32(Data, Size, 8);
    s1 = (uint16_t)read_u32(Data, Size, 12);

    x2 = width ? (x1 % (width + 1)) : 0;
    y2 = height ? (y1 % (height + 1)) : 0;
    z2 = 0;
    s2 = samplesperpixel ? (uint16_t)(s1 % (samplesperpixel + 1)) : 0;

    (void)TIFFCheckTile(tif, x1, y1, z1, s1);
    (void)TIFFCheckTile(tif, x2, y2, z2, s2);

    if (width == 0) width = 1;
    if (height == 0) height = 1;

    rw1 = width;
    rh1 = height;
    rw2 = (read_u32(Data, Size, 16) % (width + 1));
    rh2 = (read_u32(Data, Size, 20) % (height + 1));
    if (rw2 == 0) rw2 = 1;
    if (rh2 == 0) rh2 = 1;

    stop1 = (Size > 24) ? (Data[24] & 1) : 0;
    stop2 = (Size > 25) ? (Data[25] & 1) : 1;

    orient1 = ORIENTATION_BOTLEFT;
    orient2 = ORIENTATION_TOPLEFT + ((Size > 26 ? Data[26] : 0) % 8);

    if (rw1 != 0 && rh1 != 0 && rw1 <= 1U << 20 && rh1 <= 1U << 20 &&
        rh1 <= ((size_t)-1) / sizeof(uint32_t) / rw1) {
        raster = (uint32_t *)malloc((size_t)rw1 * rh1 * sizeof(uint32_t));
        if (raster) {
            (void)TIFFReadRGBAImage(tif, rw1, rh1, raster, stop1);
            (void)TIFFReadRGBAImageOriented(tif, rw1, rh1, raster, orient1, stop2);
            free(raster);
            raster = NULL;
        }
    }

    if (rw2 != 0 && rh2 != 0 && rw2 <= 1U << 20 && rh2 <= 1U << 20 &&
        rh2 <= ((size_t)-1) / sizeof(uint32_t) / rw2) {
        raster = (uint32_t *)malloc((size_t)rw2 * rh2 * sizeof(uint32_t));
        if (raster) {
            (void)TIFFReadRGBAImage(tif, rw2, rh2, raster, stop2);
            (void)TIFFReadRGBAImageOriented(tif, rw2, rh2, raster, orient2, stop1);
            free(raster);
            raster = NULL;
        }
    }

    if (TIFFIsTiled(tif)) {
        if (tilew == 0) tilew = 1;
        if (tileh == 0) tileh = 1;

        if (tileh <= ((size_t)-1) / sizeof(uint32_t) / tilew) {
            tilebuf = (uint32_t *)malloc((size_t)tilew * tileh * sizeof(uint32_t));
            if (tilebuf) {
                col1 = (tilew != 0) ? (x1 / tilew) * tilew : 0;
                row1 = (tileh != 0) ? (y1 / tileh) * tileh : 0;

                if (width && col1 >= width)
                    col1 = ((width - 1) / tilew) * tilew;
                if (height && row1 >= height)
                    row1 = ((height - 1) / tileh) * tileh;

                col2 = 0;
                row2 = 0;
                if (width > tilew)
                    col2 = ((read_u32(Data, Size, 28) % ((width + tilew - 1) / tilew)) * tilew);
                if (height > tileh)
                    row2 = ((read_u32(Data, Size, 32) % ((height + tileh - 1) / tileh)) * tileh);

                (void)TIFFCheckTile(tif, col1, row1, 0, 0);
                (void)TIFFReadRGBATile(tif, col1, row1, tilebuf);
                (void)TIFFReadRGBATileExt(tif, col1, row1, tilebuf, stop1);

                (void)TIFFCheckTile(tif, col2, row2, 0, s2);
                (void)TIFFReadRGBATile(tif, col2, row2, tilebuf);
                (void)TIFFReadRGBATileExt(tif, col2, row2, tilebuf, stop2);

                free(tilebuf);
                tilebuf = NULL;
            }
        }
    }

    scanline_size = TIFFScanlineSize(tif);
    if (scanline_size > 0 && scanline_size < (tmsize_t)(1 << 26)) {
        scanbuf = malloc((size_t)scanline_size);
        if (scanbuf) {
            uint32_t rowa = height ? (read_u32(Data, Size, 36) % (height + 1)) : 0;
            uint32_t rowb = height ? (read_u32(Data, Size, 40) % (height + 1)) : 0;
            uint16_t sa = samplesperpixel ? (uint16_t)(read_u32(Data, Size, 44) % (samplesperpixel + 1)) : 0;
            uint16_t sb = 0;

            (void)TIFFReadScanline(tif, scanbuf, rowa, sa);
            (void)TIFFReadScanline(tif, scanbuf, rowb, sb);

            free(scanbuf);
            scanbuf = NULL;
        }
    }

    TIFFClose(tif);
    return 0;
}