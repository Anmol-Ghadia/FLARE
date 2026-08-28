// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFYCbCrtoRGB at tif_color.c:181:1 in tiffio.h
// TIFFYCbCrtoRGB at tif_color.c:181:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_57(const uint8_t *Data, size_t Size) {
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

    char errbuf[1024];
    memset(errbuf, 0, sizeof(errbuf));
    (void)TIFFRGBAImageOK(tif, errbuf);

    uint32_t width = 0, height = 0;
    uint32_t rowsperstrip = 0;
    uint32_t tilewidth = 0, tilelength = 0;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);

    if (width == 0) width = (Size >= 4) ? (rd32(Data) % 64U) + 1U : 1U;
    if (height == 0) height = (Size >= 8) ? (rd32(Data + 4) % 64U) + 1U : 1U;

    uint64_t npixels64 = (uint64_t)width * (uint64_t)height;
    if (npixels64 > 0 && npixels64 <= (1U << 20)) {
        uint32_t *raster = (uint32_t *)_TIFFmalloc((tmsize_t)(npixels64 * sizeof(uint32_t)));
        if (raster) {
            memset(raster, 0, (size_t)(npixels64 * sizeof(uint32_t)));

            (void)TIFFReadRGBAImage(tif, width, height, raster, (Size > 0) ? (Data[0] & 1) : 0);

            static const int orientations[] = {
                ORIENTATION_TOPLEFT, ORIENTATION_TOPRIGHT,
                ORIENTATION_BOTRIGHT, ORIENTATION_BOTLEFT,
                ORIENTATION_LEFTTOP, ORIENTATION_RIGHTTOP,
                ORIENTATION_RIGHTBOT, ORIENTATION_LEFTBOT
            };
            size_t idx = (Size > 1) ? (Data[1] % (sizeof(orientations) / sizeof(orientations[0]))) : 0;
            (void)TIFFReadRGBAImageOriented(
                tif, width, height, raster, orientations[idx],
                (Size > 2) ? (Data[2] & 1) : 0);

            _TIFFfree(raster);
        }
    }

    if (!TIFFIsTiled(tif)) {
        if (rowsperstrip == 0) {
            rowsperstrip = 1;
        }

        uint32_t strip_row = 0;
        if (height > 0) {
            if (Size >= 12) {
                uint32_t candidate = rd32(Data + 8);
                strip_row = (candidate / rowsperstrip) * rowsperstrip;
                if (strip_row >= height) {
                    strip_row = 0;
                }
            }
        }

        tmsize_t stripsize = TIFFStripSize(tif);
        if (stripsize > 0 && stripsize <= (1 << 26)) {
            uint32_t *strip_raster = (uint32_t *)_TIFFmalloc((tmsize_t)stripsize);
            if (strip_raster) {
                memset(strip_raster, 0, (size_t)stripsize);
                (void)TIFFReadRGBAStripExt(
                    tif, strip_row, strip_raster,
                    (Size > 12) ? (Data[12] & 1) : 0);
                _TIFFfree(strip_raster);
            }
        }
    }

    if (TIFFIsTiled(tif)) {
        uint32_t tile = 0;
        ttile_t numtiles = TIFFNumberOfTiles(tif);
        if (numtiles > 0) {
            tile = (Size >= 16) ? (rd32(Data + 12) % numtiles) : 0;
        }

        tmsize_t tilesize = TIFFTileSize(tif);
        if (tilesize > 0 && tilesize <= (1 << 26)) {
            void *tilebuf = _TIFFmalloc(tilesize);
            if (tilebuf) {
                memset(tilebuf, 0, (size_t)tilesize);
                (void)TIFFReadEncodedTile(tif, tile, tilebuf, tilesize);

                if (Size > 16) {
                    tmsize_t alt_size = (tmsize_t)(Data[16] % (unsigned int)(tilesize + 1));
                    (void)TIFFReadEncodedTile(tif, tile, tilebuf, alt_size);
                }

                _TIFFfree(tilebuf);
            }
        }
    }

    {
        TIFFYCbCrToRGB *ycbcr = (TIFFYCbCrToRGB *)_TIFFmalloc(sizeof(TIFFYCbCrToRGB));
        if (ycbcr) {
            memset(ycbcr, 0, sizeof(TIFFYCbCrToRGB));

            uint32_t r = 0, g = 0, b = 0;
            uint32_t y = (Size > 0) ? Data[0] : 0;
            int32_t cb = (Size > 1) ? (int32_t)((int)Data[1] - 128) : 0;
            int32_t cr = (Size > 2) ? (int32_t)((int)Data[2] - 128) : 0;
            TIFFYCbCrtoRGB(ycbcr, y, cb, cr, &r, &g, &b);

            if (Size >= 6) {
                TIFFYCbCrtoRGB(
                    ycbcr,
                    (uint32_t)Data[3],
                    (int32_t)((int)Data[4] - 128),
                    (int32_t)((int)Data[5] - 128),
                    &r, &g, &b);
            }

            _TIFFfree(ycbcr);
        }
    }

    TIFFClose(tif);
    return 0;
}