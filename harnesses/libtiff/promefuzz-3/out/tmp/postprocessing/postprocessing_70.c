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
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 2 > n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 4 > n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
        if (!tif) {
            return 0;
        }

        uint32_t width = rd32(Data, Size, 0, 1) % 2048 + 1;
        uint32_t height = rd32(Data, Size, 4, 1) % 2048 + 1;
        uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 8 + 1);
        uint16_t bps = (uint16_t)((rd16(Data, Size, 10, 8) % 32) + 1);
        uint16_t photo = (uint16_t)(rd16(Data, Size, 12, PHOTOMETRIC_MINISBLACK) % 10);
        uint16_t planar = (rd16(Data, Size, 14, PLANARCONFIG_CONTIG) & 1)
                              ? PLANARCONFIG_SEPARATE
                              : PLANARCONFIG_CONTIG;
        uint32_t tilew = rd32(Data, Size, 16, 16) % 512 + 1;
        uint32_t tileh = rd32(Data, Size, 20, 16) % 512 + 1;
        uint16_t compression = (rd16(Data, Size, 24, COMPRESSION_NONE) & 1)
                                   ? COMPRESSION_LZW
                                   : COMPRESSION_NONE;

        if (tilew < 16) tilew = 16;
        if (tileh < 16) tileh = 16;

        (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
        (void)TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);

        if (photo == PHOTOMETRIC_YCBCR && planar == PLANARCONFIG_CONTIG) {
            uint16_t sub[2];
            sub[0] = (uint16_t)((rd16(Data, Size, 26, 1) % 4) + 1);
            sub[1] = (uint16_t)((rd16(Data, Size, 28, 1) % 4) + 1);
            (void)TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, sub[0], sub[1]);
        }

        (void)TIFFWriteDirectory(tif);
        TIFFClose(tif);

        tif = TIFFOpen("./dummy_file", "r");
        if (!tif) {
            return 0;
        }
    }

    do {
        uint64_t tsz = TIFFTileSize64(tif);
        (void)tsz;

        uint64_t row = TIFFTileRowSize64(tif);
        (void)row;

        uint32_t nrows1 = rd32(Data, Size, 0, 0);
        uint32_t nrows2 = rd32(Data, Size, 4, 1);
        uint32_t nrows3 = rd32(Data, Size, 8, 0xFFFFFFFFu);

        uint64_t v1 = TIFFVTileSize64(tif, nrows1);
        uint64_t v2 = TIFFVTileSize64(tif, nrows2);
        uint64_t v3 = TIFFVTileSize64(tif, nrows3);
        (void)v1;
        (void)v2;
        (void)v3;

        uint32_t striles[6];
        striles[0] = 0;
        striles[1] = 1;
        striles[2] = rd32(Data, Size, 12, 0);
        striles[3] = rd32(Data, Size, 16, 0xFFFFFFFFu);
        striles[4] = rd32(Data, Size, 20, 1024);
        striles[5] = rd32(Data, Size, 24, 17);

        for (size_t i = 0; i < sizeof(striles) / sizeof(striles[0]); i++) {
            int err = 0;
            uint64_t bc1 = TIFFGetStrileByteCount(tif, striles[i]);
            uint64_t bc2 = TIFFGetStrileByteCountWithErr(tif, striles[i], &err);
            uint64_t off = TIFFGetStrileOffset(tif, striles[i]);
            (void)bc1;
            (void)bc2;
            (void)off;
            (void)err;
        }
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}