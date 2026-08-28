// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFReadScanline at tif_read.c:437:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
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

static uint32_t safe_mul_u32(uint32_t a, uint32_t b) {
    if (a == 0 || b == 0) return 0;
    if (a > 0xFFFFFFFFU / b) return 0;
    return a * b;
}

static void fuzz_tiff_handle(TIFF *tif, const uint8_t *Data, size_t Size) {
    char emsg[1024];
    uint32_t width = 0, height = 0;
    uint32_t tilew = 0, tileh = 0;
    uint32_t rowsperstrip = 0;
    uint16_t orientation = ORIENTATION_BOTLEFT;
    uint32_t raster_w, raster_h, npixels;
    uint32_t *raster = NULL;
    tmsize_t scanline_size = 0;
    void *scanbuf = NULL;
    int ok_rgba = 0;
    size_t i;

    (void)Data;
    (void)Size;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilew);
    (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileh);
    (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orientation);

    raster_w = width ? width : 1;
    raster_h = height ? height : 1;
    if (raster_w > 4096) raster_w = 4096;
    if (raster_h > 4096) raster_h = 4096;

    npixels = safe_mul_u32(raster_w, raster_h);
    if (npixels > 0 && npixels <= (1U << 24)) {
        raster = (uint32_t *)malloc((size_t)npixels * sizeof(uint32_t));
    }

    if (raster) {
        memset(raster, 0, (size_t)npixels * sizeof(uint32_t));

        ok_rgba = TIFFRGBAImageOK(tif, emsg);
        if (ok_rgba) {
            (void)TIFFReadRGBAImage(tif, raster_w, raster_h, raster, 0);
            (void)TIFFReadRGBAImage(tif, raster_w, raster_h, raster, 1);

            (void)TIFFReadRGBAImageOriented(tif, raster_w, raster_h, raster, ORIENTATION_BOTLEFT, 0);
            (void)TIFFReadRGBAImageOriented(tif, raster_w, raster_h, raster, ORIENTATION_TOPLEFT, 1);
            (void)TIFFReadRGBAImageOriented(tif, raster_w, raster_h, raster, orientation, 0);
        }
    }

    if (TIFFIsTiled(tif)) {
        uint32_t use_tilew = tilew ? tilew : 1;
        uint32_t use_tileh = tileh ? tileh : 1;
        uint32_t tile_pixels = safe_mul_u32(use_tilew, use_tileh);
        uint32_t *tilebuf = NULL;

        if (tile_pixels > 0 && tile_pixels <= (1U << 24)) {
            tilebuf = (uint32_t *)malloc((size_t)tile_pixels * sizeof(uint32_t));
        }

        if (tilebuf) {
            uint32_t coords[8][2];
            size_t ncoords = 0;

            memset(tilebuf, 0, (size_t)tile_pixels * sizeof(uint32_t));

            coords[ncoords][0] = 0;
            coords[ncoords++][1] = 0;

            coords[ncoords][0] = use_tilew;
            coords[ncoords++][1] = 0;

            coords[ncoords][0] = 0;
            coords[ncoords++][1] = use_tileh;

            coords[ncoords][0] = (width >= use_tilew) ? (width - use_tilew) : 0;
            coords[ncoords++][1] = (height >= use_tileh) ? (height - use_tileh) : 0;

            coords[ncoords][0] = width ? (width - 1) : 0;
            coords[ncoords++][1] = height ? (height - 1) : 0;

            coords[ncoords][0] = 1;
            coords[ncoords++][1] = 1;

            for (i = 0; i < ncoords; i++) {
                (void)TIFFReadRGBATile(tif, coords[i][0], coords[i][1], tilebuf);
                (void)TIFFReadRGBATileExt(tif, coords[i][0], coords[i][1], tilebuf, 0);
                (void)TIFFReadRGBATileExt(tif, coords[i][0], coords[i][1], tilebuf, 1);
            }

            free(tilebuf);
        }
    } else {
        uint32_t use_rps = rowsperstrip ? rowsperstrip : 1;
        uint32_t strip_pixels = safe_mul_u32(width ? width : 1, use_rps);
        uint32_t *stripbuf = NULL;
        uint32_t rows_to_try[6];
        size_t nrows = 0;

        if (strip_pixels > 0 && strip_pixels <= (1U << 24)) {
            stripbuf = (uint32_t *)malloc((size_t)strip_pixels * sizeof(uint32_t));
        }

        if (stripbuf) {
            memset(stripbuf, 0, (size_t)strip_pixels * sizeof(uint32_t));

            rows_to_try[nrows++] = 0;
            rows_to_try[nrows++] = use_rps;
            rows_to_try[nrows++] = (height > use_rps) ? (height - use_rps) : 0;
            rows_to_try[nrows++] = height ? (height - 1) : 0;
            rows_to_try[nrows++] = 1;

            for (i = 0; i < nrows; i++) {
                (void)TIFFReadRGBAStripExt(tif, rows_to_try[i], stripbuf, 0);
                (void)TIFFReadRGBAStripExt(tif, rows_to_try[i], stripbuf, 1);
            }

            free(stripbuf);
        }
    }

    scanline_size = TIFFScanlineSize(tif);
    if (scanline_size > 0 && scanline_size <= (tmsize_t)(1 << 20)) {
        scanbuf = malloc((size_t)scanline_size);
    }

    if (scanbuf) {
        uint16_t spp = 1;
        uint32_t rows_to_try[4];
        size_t nrows = 0;
        uint16_t samples_to_try[4];
        size_t nsamples = 0;
        size_t r, s;

        memset(scanbuf, 0, (size_t)scanline_size);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

        rows_to_try[nrows++] = 0;
        rows_to_try[nrows++] = height ? (height - 1) : 0;
        rows_to_try[nrows++] = 1;
        rows_to_try[nrows++] = height;

        samples_to_try[nsamples++] = 0;
        if (spp > 1) samples_to_try[nsamples++] = (uint16_t)(spp - 1);
        samples_to_try[nsamples++] = spp;

        for (r = 0; r < nrows; r++) {
            for (s = 0; s < nsamples; s++) {
                (void)TIFFReadScanline(tif, scanbuf, rows_to_try[r], samples_to_try[s]);
            }
        }

        free(scanbuf);
    }

    free(raster);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    do {
        fuzz_tiff_handle(tif, Data, Size);
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}