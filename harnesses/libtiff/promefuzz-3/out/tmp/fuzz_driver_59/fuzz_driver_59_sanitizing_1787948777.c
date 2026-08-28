#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 1 >= n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 3 >= n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
        if (!tif) return 0;

        uint32_t width = rd32(Data, Size, 0, 1);
        uint32_t height = rd32(Data, Size, 4, 1);
        uint16_t spp = rd16(Data, Size, 8, 1);
        uint16_t bps = rd16(Data, Size, 10, 8);
        uint16_t planar = (rd16(Data, Size, 12, PLANARCONFIG_CONTIG) & 1)
                              ? PLANARCONFIG_SEPARATE
                              : PLANARCONFIG_CONTIG;
        uint16_t photometric = (rd16(Data, Size, 14, PHOTOMETRIC_MINISBLACK) % 7);
        uint16_t compression = COMPRESSION_NONE;
        uint16_t sampleformat = (rd16(Data, Size, 16, SAMPLEFORMAT_UINT) % 4) + 1;
        uint32_t rowsperstrip = rd32(Data, Size, 20, 1);
        uint32_t tilewidth = rd32(Data, Size, 24, 16);
        uint32_t tilelength = rd32(Data, Size, 28, 16);

        if (width == 0) width = 1;
        if (height == 0) height = 1;
        if (spp == 0) spp = 1;
        if (bps == 0) bps = 1;
        if (rowsperstrip == 0) rowsperstrip = 1;
        if (tilewidth == 0) tilewidth = 16;
        if (tilelength == 0) tilelength = 16;

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
        TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        if (Size > 32 && (Data[32] & 1)) {
            TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilewidth);
            TIFFSetField(tif, TIFFTAG_TILELENGTH, tilelength);
        }

        if (photometric == PHOTOMETRIC_YCBCR) {
            uint16_t ycbcrs[2];
            ycbcrs[0] = (uint16_t)((rd16(Data, Size, 34, 2) % 4) + 1);
            ycbcrs[1] = (uint16_t)((rd16(Data, Size, 36, 2) % 4) + 1);
            TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, ycbcrs[0], ycbcrs[1]);
        }

        TIFFWriteDirectory(tif);
    }

    if (tif) {
        do {
            (void)TIFFRasterScanlineSize64(tif);
            (void)TIFFTileSize64(tif);
            (void)TIFFStripSize(tif);
            (void)TIFFTileRowSize64(tif);
            (void)TIFFScanlineSize64(tif);

            uint32_t nstrips = TIFFNumberOfStrips(tif);
            uint32_t ntiles = TIFFNumberOfTiles(tif);
            uint32_t maxstriles = nstrips > ntiles ? nstrips : ntiles;

            if (maxstriles == 0) {
                int err = 0;
                (void)TIFFGetStrileByteCountWithErr(tif, 0, &err);
                (void)TIFFGetStrileByteCountWithErr(tif, 1, &err);
            } else {
                uint32_t idxs[4];
                idxs[0] = 0;
                idxs[1] = maxstriles - 1;
                idxs[2] = (Size >= 4) ? (rd32(Data, Size, 40, 0) % maxstriles) : 0;
                idxs[3] = maxstriles;

                for (int i = 0; i < 4; i++) {
                    int err = 0;
                    (void)TIFFGetStrileByteCountWithErr(tif, idxs[i], &err);
                }
            }
        } while (TIFFReadDirectory(tif));

        TIFFClose(tif);
    }

    return 0;
}