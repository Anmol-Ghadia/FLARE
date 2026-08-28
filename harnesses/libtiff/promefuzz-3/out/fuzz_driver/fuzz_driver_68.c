// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFRGBAImageEnd at tif_getimage.c:215:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStrip at tif_getimage.c:2914:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStrip at tif_getimage.c:2914:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
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

static uint16_t read_u16(const uint8_t *Data, size_t Size, size_t Off, uint16_t DefVal) {
    if (Off + 1 >= Size) return DefVal;
    return (uint16_t)(Data[Off] | ((uint16_t)Data[Off + 1] << 8));
}

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t Off, uint32_t DefVal) {
    if (Off + 3 >= Size) return DefVal;
    return (uint32_t)Data[Off] |
           ((uint32_t)Data[Off + 1] << 8) |
           ((uint32_t)Data[Off + 2] << 16) |
           ((uint32_t)Data[Off + 3] << 24);
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
        remove("./dummy_file");
        return 0;
    }

    uint32_t width = 0, height = 0;
    uint32_t tilewidth = 0, tilelength = 0;
    uint32_t rowsperstrip = 0;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);
    (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);

    if (width == 0) width = 1;
    if (height == 0) height = 1;
    if (tilewidth == 0) tilewidth = width;
    if (tilelength == 0) tilelength = height;
    if (rowsperstrip == 0) rowsperstrip = 1;

    size_t npixels = (size_t)width * (size_t)height;
    if (npixels == 0 || npixels > (1U << 26)) {
        npixels = 1;
    }

    uint32_t *full_raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
    if (full_raster) {
        memset(full_raster, 0, npixels * sizeof(uint32_t));

        (void)TIFFReadRGBAImage(tif, width, height, full_raster, (int)(Size ? (Data[0] & 1) : 0));

        int orientations[] = {
            ORIENTATION_TOPLEFT,
            ORIENTATION_TOPRIGHT,
            ORIENTATION_BOTRIGHT,
            ORIENTATION_BOTLEFT,
            ORIENTATION_LEFTTOP,
            ORIENTATION_RIGHTTOP,
            ORIENTATION_RIGHTBOT,
            ORIENTATION_LEFTBOT
        };
        size_t i;
        for (i = 0; i < sizeof(orientations) / sizeof(orientations[0]); i++) {
            memset(full_raster, 0, npixels * sizeof(uint32_t));
            (void)TIFFReadRGBAImageOriented(
                tif, width, height, full_raster, orientations[i],
                (int)((Size > i + 1) ? (Data[i + 1] & 1) : 0));
        }
    }

    {
        TIFFRGBAImage img;
        char emsg[1024];
        memset(&img, 0, sizeof(img));
        memset(emsg, 0, sizeof(emsg));

        if (TIFFRGBAImageBegin(&img, tif, (int)(Size > 2 ? (Data[2] & 1) : 0), emsg)) {
            if (img.width == 0) img.width = width;
            if (img.height == 0) img.height = height;

            size_t img_pixels = (size_t)img.width * (size_t)img.height;
            if (img_pixels == 0 || img_pixels > (1U << 26)) {
                img_pixels = 1;
            }

            uint32_t *img_raster = (uint32_t *)malloc(img_pixels * sizeof(uint32_t));
            if (img_raster) {
                memset(img_raster, 0, img_pixels * sizeof(uint32_t));
                (void)TIFFReadRGBAImageOriented(
                    tif, img.width, img.height, img_raster,
                    img.req_orientation ? img.req_orientation : ORIENTATION_BOTLEFT,
                    img.stoponerr);
                free(img_raster);
            }

            TIFFRGBAImageEnd(&img);
        }
    }

    if (TIFFIsTiled(tif)) {
        tmsize_t tile_pixels = (tmsize_t)tilewidth * (tmsize_t)tilelength;
        if (tile_pixels <= 0 || tile_pixels > (1 << 24)) {
            tile_pixels = 1;
        }

        uint32_t *tile_raster = (uint32_t *)malloc((size_t)tile_pixels * sizeof(uint32_t));
        if (tile_raster) {
            memset(tile_raster, 0, (size_t)tile_pixels * sizeof(uint32_t));

            uint32_t col = 0, row = 0;
            if (tilewidth != 0) {
                uint32_t max_tiles_x = (width + tilewidth - 1) / tilewidth;
                if (max_tiles_x > 0) {
                    uint32_t idx = read_u32(Data, Size, 4, 0) % max_tiles_x;
                    col = idx * tilewidth;
                }
            }
            if (tilelength != 0) {
                uint32_t max_tiles_y = (height + tilelength - 1) / tilelength;
                if (max_tiles_y > 0) {
                    uint32_t idx = read_u32(Data, Size, 8, 0) % max_tiles_y;
                    row = idx * tilelength;
                }
            }

            (void)TIFFReadRGBATileExt(tif, col, row, tile_raster, (int)(Size > 3 ? (Data[3] & 1) : 0));
            (void)TIFFReadRGBATileExt(tif, 0, 0, tile_raster, 0);

            free(tile_raster);
        }
    } else {
        uint32_t strip_h = rowsperstrip ? rowsperstrip : 1;
        size_t strip_pixels = (size_t)width * (size_t)strip_h;
        if (strip_pixels == 0 || strip_pixels > (1U << 26)) {
            strip_pixels = 1;
        }

        uint32_t *strip_raster = (uint32_t *)malloc(strip_pixels * sizeof(uint32_t));
        if (strip_raster) {
            memset(strip_raster, 0, strip_pixels * sizeof(uint32_t));

            uint32_t row = 0;
            if (rowsperstrip != 0) {
                uint32_t nstrips = (height + rowsperstrip - 1) / rowsperstrip;
                if (nstrips > 0) {
                    uint32_t strip_index = read_u32(Data, Size, 12, 0) % nstrips;
                    row = strip_index * rowsperstrip;
                }
            }

            (void)TIFFReadRGBAStripExt(tif, row, strip_raster, (int)(Size > 4 ? (Data[4] & 1) : 0));
            (void)TIFFReadRGBAStrip(tif, row, strip_raster);
            (void)TIFFReadRGBAStripExt(tif, 0, strip_raster, 0);
            (void)TIFFReadRGBAStrip(tif, 0, strip_raster);

            free(strip_raster);
        }
    }

    if (full_raster) {
        free(full_raster);
    }

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}