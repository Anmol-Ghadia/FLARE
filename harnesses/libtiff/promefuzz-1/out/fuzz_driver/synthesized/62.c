// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadScanline at tif_read.c:437:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFReadScanline at tif_read.c:437:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_62(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    do {
        uint32_t width = 0, height = 0;
        uint16_t samplesperpixel = 1;
        uint16_t bitspersample = 8;
        uint32_t rowsperstrip = 0;
        uint16_t planarconfig = PLANARCONFIG_CONTIG;
        uint32_t tilewidth = 0, tilelength = 0;

        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
        (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);

        (void)TIFFCurrentRow(tif);
        (void)TIFFCurrentStrip(tif);

        tmsize_t scanline = TIFFScanlineSize(tif);
        if (scanline > 0 && scanline < (tmsize_t)(1 << 26) && height > 0) {
            void *buf = _TIFFmalloc((tmsize_t)scanline);
            if (buf) {
                uint32_t row = 0;
                if (Size >= 4) {
                    row = ((uint32_t)Data[0] << 24) |
                          ((uint32_t)Data[1] << 16) |
                          ((uint32_t)Data[2] << 8) |
                          (uint32_t)Data[3];
                    row %= height;
                }

                uint16_t sample = 0;
                if (planarconfig == PLANARCONFIG_SEPARATE && samplesperpixel > 0) {
                    sample = (uint16_t)((Size > 4 ? Data[4] : 0) % samplesperpixel);
                }

                (void)TIFFReadScanline(tif, buf, row, sample);
                (void)TIFFCurrentRow(tif);
                (void)TIFFCurrentStrip(tif);

                if (height > 1) {
                    uint32_t row2 = (row + 1) % height;
                    (void)TIFFReadScanline(tif, buf, row2, sample);
                    (void)TIFFCurrentRow(tif);
                    (void)TIFFCurrentStrip(tif);
                }

                _TIFFfree(buf);
            }
        }

        if (!TIFFIsTiled(tif)) {
            uint32_t raster_w = width ? width : 1;
            uint32_t raster_h = 0;

            if (rowsperstrip == 0) {
                rowsperstrip = height ? height : 1;
            }
            raster_h = rowsperstrip;
            if (height > 0 && raster_h > height) {
                raster_h = height;
            }
            if (raster_h == 0) {
                raster_h = 1;
            }

            if (raster_w <= (1U << 20) && raster_h <= (1U << 20)) {
                size_t npixels = (size_t)raster_w * (size_t)raster_h;
                if (npixels > 0 && npixels < (size_t)(1 << 26)) {
                    uint32_t *raster = (uint32_t *)_TIFFmalloc((tmsize_t)(npixels * sizeof(uint32_t)));
                    if (raster) {
                        uint32_t row = 0;
                        if (height > 0 && rowsperstrip > 0) {
                            uint32_t nstrips = (height + rowsperstrip - 1) / rowsperstrip;
                            uint32_t strip_index = 0;
                            if (Size > 5 && nstrips > 0) {
                                strip_index = Data[5] % nstrips;
                            }
                            row = strip_index * rowsperstrip;
                            if (row >= height) {
                                row = 0;
                            }
                        }
                        (void)TIFFReadRGBAStripExt(tif, row, raster, 0);
                        (void)TIFFReadRGBAStripExt(tif, row, raster, 1);
                        (void)TIFFCurrentRow(tif);
                        (void)TIFFCurrentStrip(tif);
                        _TIFFfree(raster);
                    }
                }
            }
        } else {
            uint32_t tw = tilewidth ? tilewidth : 1;
            uint32_t tl = tilelength ? tilelength : 1;
            if (tw <= (1U << 16) && tl <= (1U << 16)) {
                size_t npixels = (size_t)tw * (size_t)tl;
                if (npixels > 0 && npixels < (size_t)(1 << 26)) {
                    uint32_t *raster = (uint32_t *)_TIFFmalloc((tmsize_t)(npixels * sizeof(uint32_t)));
                    if (raster) {
                        uint32_t col = 0, row = 0;
                        if (width > 0 && tw > 0) {
                            uint32_t ntx = (width + tw - 1) / tw;
                            uint32_t tx = (Size > 6 && ntx > 0) ? (Data[6] % ntx) : 0;
                            col = tx * tw;
                        }
                        if (height > 0 && tl > 0) {
                            uint32_t nty = (height + tl - 1) / tl;
                            uint32_t ty = (Size > 7 && nty > 0) ? (Data[7] % nty) : 0;
                            row = ty * tl;
                        }

                        (void)TIFFReadRGBATileExt(tif, col, row, raster, 0);
                        (void)TIFFReadRGBATileExt(tif, col, row, raster, 1);
                        (void)TIFFCurrentRow(tif);
                        (void)TIFFCurrentStrip(tif);
                        _TIFFfree(raster);
                    }
                }
            }
        }
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}