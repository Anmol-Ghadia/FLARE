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

static uint16_t rd16(const uint8_t *p, size_t n, size_t off)
{
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off)
{
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void set_common_fields(TIFF *tif, const uint8_t *Data, size_t Size, int tiled)
{
    uint32_t width = rd32(Data, Size, 0) % 4096 + 1;
    uint32_t height = rd32(Data, Size, 4) % 4096 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8) % 8);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    uint16_t photo_choices[] = {
        PHOTOMETRIC_MINISBLACK,
        PHOTOMETRIC_RGB,
#ifdef PHOTOMETRIC_YCBCR
        PHOTOMETRIC_YCBCR,
#endif
#ifdef PHOTOMETRIC_PALETTE
        PHOTOMETRIC_PALETTE,
#endif
#ifdef PHOTOMETRIC_SEPARATED
        PHOTOMETRIC_SEPARATED,
#endif
    };
    uint16_t photo = photo_choices[rd16(Data, Size, 12) % (sizeof(photo_choices) / sizeof(photo_choices[0]))];
    uint16_t planar = (rd16(Data, Size, 14) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
#ifdef COMPRESSION_LZW
    if (Size > 16 && (Data[16] & 1))
        compression = COMPRESSION_LZW;
#endif

    if (spp == 0) spp = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

#ifdef PHOTOMETRIC_YCBCR
    if (photo == PHOTOMETRIC_YCBCR)
    {
        uint16_t hsub = (uint16_t)((rd16(Data, Size, 18) % 4) + 1);
        uint16_t vsub = (uint16_t)((rd16(Data, Size, 20) % 4) + 1);
        TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, hsub, vsub);
    }
#endif

    if (tiled)
    {
        uint32_t tw = rd32(Data, Size, 22) % 512 + 1;
        uint32_t th = rd32(Data, Size, 26) % 512 + 1;
        TIFFDefaultTileSize(tif, &tw, &th);
        if (tw == 0) tw = 1;
        if (th == 0) th = 1;
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, th);
    }
    else
    {
        uint32_t req = rd32(Data, Size, 22);
        uint32_t rps = TIFFDefaultStripSize(tif, req);
        if (rps == 0) rps = 1;
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rps);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;

    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif_strip = TIFFOpen("./dummy_file", "w");
    if (tif_strip)
    {
        set_common_fields(tif_strip, Data, Size, 0);

        (void)TIFFVStripSize64(tif_strip, (uint32_t)-1);
        (void)TIFFVStripSize64(tif_strip, 0);
        (void)TIFFVStripSize64(tif_strip, 1);
        (void)TIFFVStripSize64(tif_strip, rd32(Data, Size, 30));
        (void)TIFFStripSize64(tif_strip);
        (void)TIFFDefaultStripSize(tif_strip, 0);
        (void)TIFFDefaultStripSize(tif_strip, rd32(Data, Size, 34));

        TIFFClose(tif_strip);
    }

    TIFF *tif_tile = TIFFOpen("./dummy_file", "w");
    if (tif_tile)
    {
        set_common_fields(tif_tile, Data, Size, 1);

        uint32_t tw = 0, th = 0;
        TIFFDefaultTileSize(tif_tile, &tw, &th);
        (void)TIFFNumberOfTiles(tif_tile);

        tsize_t tilesize = TIFFTileSize(tif_tile);
        if (tilesize < 0)
            tilesize = 0;

        uint32_t nt = TIFFNumberOfTiles(tif_tile);
        if (nt > 0)
        {
            uint32_t tile = rd32(Data, Size, 38);
            if (nt != 0)
                tile %= nt;

            tmsize_t cc = (tmsize_t)((Size > 42) ? (Size - 42) : 0);
            if (tilesize > 0 && cc > tilesize)
                cc = tilesize;

            void *buf = NULL;
            if (cc > 0)
            {
                buf = malloc((size_t)cc);
                if (buf)
                    memcpy(buf, Data + 42, (size_t)cc);
            }

            (void)TIFFWriteEncodedTile(tif_tile, tile, buf, cc);
            (void)TIFFWriteEncodedTile(tif_tile, nt, buf, cc);
            (void)TIFFWriteEncodedTile(tif_tile, 0, buf, 0);

            free(buf);
        }
        else
        {
            (void)TIFFWriteEncodedTile(tif_tile, 0, NULL, 0);
        }

        TIFFClose(tif_tile);
    }

    return 0;
}