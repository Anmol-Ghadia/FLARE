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

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void setup_tiled_writer(TIFF *tif, const uint8_t *Data, size_t Size)
{
    uint32_t width = (rd16(Data, Size, 0) % 64) + 1;
    uint32_t height = (rd16(Data, Size, 2) % 64) + 1;
    uint32_t tilew = ((rd16(Data, Size, 4) % 16) + 1) * 16;
    uint32_t tileh = ((rd16(Data, Size, 6) % 16) + 1) * 16;
    uint16_t spp = (uint16_t)((rd16(Data, Size, 8) % 4) + 1);
    uint16_t bps_choices[] = {1, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10) % 4];
    uint16_t planar = (rd16(Data, Size, 12) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
    TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
}

static void setup_striped_writer(TIFF *tif, const uint8_t *Data, size_t Size)
{
    uint32_t width = (rd16(Data, Size, 14) % 128) + 1;
    uint32_t height = (rd16(Data, Size, 16) % 128) + 1;
    uint16_t spp = (uint16_t)((rd16(Data, Size, 18) % 4) + 1);
    uint16_t bps_choices[] = {1, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 20) % 4];
    uint16_t planar = (rd16(Data, Size, 22) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint32_t rowsperstrip = (rd16(Data, Size, 24) % 64) + 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    static const uint8_t fallback_buf[64] = {0};

    write_dummy_file(Data, Size);

    {
        TIFF *tifw = TIFFOpen("./dummy_file", "w");
        if (tifw)
        {
            setup_tiled_writer(tifw, Data, Size);

            (void)TIFFVTileSize(tifw, (uint32_t)((rd16(Data, Size, 26) % 128) + 1));
            (void)TIFFVTileSize(tifw, 0);

            if (Size > 32)
            {
                uint32_t tile = rd32(Data, Size, 28);
                tmsize_t cc = (tmsize_t)(Size - 32);
                TIFFWriteEncodedTile(tifw, tile, (void *)(Data + 32), cc);
                TIFFWriteEncodedTile(tifw, 0, (void *)(Data + 32), cc / 2);
            }
            else
            {
                TIFFWriteEncodedTile(tifw, 0, (void *)fallback_buf, (tmsize_t)sizeof(fallback_buf));
            }

            TIFFClose(tifw);
        }
    }

    {
        TIFF *tifw = TIFFOpen("./dummy_file", "w");
        if (tifw)
        {
            setup_striped_writer(tifw, Data, Size);

            if (Size > 8)
            {
                uint32_t strip = rd32(Data, Size, 0);
                tmsize_t cc = (tmsize_t)(Size - 8);
                TIFFWriteRawStrip(tifw, strip, (void *)(Data + 8), cc);
                TIFFWriteRawStrip(tifw, 0, (void *)(Data + 8), cc / 2);
            }
            else
            {
                TIFFWriteRawStrip(tifw, 0, (void *)fallback_buf, (tmsize_t)sizeof(fallback_buf));
            }

            TIFFClose(tifw);
        }
    }

    {
        TIFF *tifr = TIFFOpen("./dummy_file", "r");
        if (tifr)
        {
            tmsize_t alloc = (Size > 0 && Size < (1U << 20)) ? (tmsize_t)Size : 4096;
            if (alloc <= 0)
                alloc = 4096;

            void *buf = malloc((size_t)alloc);
            if (buf)
            {
                uint32_t tile = rd32(Data, Size, 4);
                tmsize_t sz1 = (Size > 16) ? (tmsize_t)(rd32(Data, Size, 8) % (uint32_t)alloc) : (tmsize_t)-1;
                tmsize_t sz2 = (Size > 20) ? (tmsize_t)(rd32(Data, Size, 12) % (uint32_t)alloc) : alloc;

                TIFFReadRawTile(tifr, tile, buf, sz1);
                TIFFReadRawTile(tifr, 0, buf, (tmsize_t)-1);

                TIFFReadEncodedTile(tifr, tile, buf, sz2);
                TIFFReadEncodedTile(tifr, 0, buf, alloc);

                TIFFReadTile(tifr, buf,
                             rd32(Data, Size, 16),
                             rd32(Data, Size, 20),
                             rd32(Data, Size, 24),
                             rd16(Data, Size, 28));
                TIFFReadTile(tifr, buf, 0, 0, 0, 0);

                free(buf);
            }

            TIFFClose(tifr);
        }
    }

    return 0;
}