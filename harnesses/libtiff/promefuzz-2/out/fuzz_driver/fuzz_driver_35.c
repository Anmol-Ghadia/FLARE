// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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
// TIFFDefaultTileSize at tif_tile.c:272:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFStripSize64 at tif_strip.c:186:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultTileSize at tif_tile.c:272:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static uint16_t rd16(const uint8_t *p, size_t n, size_t off)
{
    if (off + 1 >= n)
        return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off)
{
    if (off + 3 >= n)
        return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void set_common_fields(TIFF *tif, const uint8_t *Data, size_t Size, int tiled)
{
    uint32_t width = rd32(Data, Size, 0) % 1024 + 1;
    uint32_t height = rd32(Data, Size, 4) % 1024 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8) % 4 + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16};
    uint16_t bps = bps_choices[rd16(Data, Size, 10) %
                               (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    uint16_t planar = (rd16(Data, Size, 14) & 1) ? PLANARCONFIG_SEPARATE
                                                 : PLANARCONFIG_CONTIG;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    if (spp >= 3)
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    else
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    if (tiled)
    {
        uint32_t tw = rd32(Data, Size, 22) % 128 + 1;
        uint32_t th = rd32(Data, Size, 26) % 128 + 1;
        TIFFDefaultTileSize(tif, &tw, &th);
        if (tw == 0)
            tw = 16;
        if (th == 0)
            th = 16;
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, th);
    }
    else
    {
        uint32_t req = rd32(Data, Size, 22);
        uint32_t rps = TIFFDefaultStripSize(tif, req);
        if (rps == 0)
            rps = 1;
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rps);
    }
}

static void exercise_strip_apis(TIFF *tif, const uint8_t *Data, size_t Size)
{
    (void)TIFFVStripSize64(tif, (uint32_t)-1);
    (void)TIFFVStripSize64(tif, 0);
    (void)TIFFVStripSize64(tif, 1);
    (void)TIFFVStripSize64(tif, rd32(Data, Size, 30));
    (void)TIFFStripSize64(tif);
    (void)TIFFDefaultStripSize(tif, 0);
    (void)TIFFDefaultStripSize(tif, rd32(Data, Size, 34));
}

static void exercise_tile_apis(TIFF *tif)
{
    uint32_t tw = 0, th = 0;
    TIFFDefaultTileSize(tif, &tw, &th);
    (void)TIFFNumberOfTiles(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    {
        TIFF *tif_strip = TIFFOpen("./dummy_file", "w");
        if (tif_strip)
        {
            set_common_fields(tif_strip, Data, Size, 0);
            exercise_strip_apis(tif_strip, Data, Size);
            TIFFClose(tif_strip);
        }
    }

    {
        TIFF *tif_tile = TIFFOpen("./dummy_file", "w");
        if (tif_tile)
        {
            set_common_fields(tif_tile, Data, Size, 1);
            exercise_tile_apis(tif_tile);

            /*
             * Avoid TIFFWriteEncodedTile() here: invoking the encoder requires
             * codec-specific invariants on the input buffer and setup state.
             * We still fuzz the requested target APIs that are safe to call
             * after directory setup.
             */

            TIFFClose(tif_tile);
        }
    }

    return 0;
}