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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    uint32_t width = 32, height = 32;
    uint32_t tilew = 16, tileh = 16;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;

    if (Size >= 2) width = (uint32_t)(rd16(Data) % 128U) + 1U;
    if (Size >= 4) height = (uint32_t)(rd16(Data + 2) % 128U) + 1U;
    if (Size >= 6) tilew = ((uint32_t)(rd16(Data + 4) % 32U) + 1U);
    if (Size >= 8) tileh = ((uint32_t)(rd16(Data + 6) % 32U) + 1U);
    if (Size >= 9) spp = (uint16_t)((Data[8] % 4U) + 1U);
    if (Size >= 10) bps = (Data[9] & 1U) ? 8 : 16;
    if (Size >= 11) planar = (Data[10] & 1U) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    if (Size >= 12) photo = (Data[11] & 1U) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    if (Size >= 13) compression = (Data[12] & 1U) ? COMPRESSION_LZW : COMPRESSION_NONE;

    if (photo != PHOTOMETRIC_RGB) {
        spp = 1;
    } else if (spp < 3) {
        spp = 3;
    }

    if (tilew < 1) tilew = 1;
    if (tileh < 1) tileh = 1;
    if (tilew > width) tilew = width;
    if (tileh > height) tileh = height;

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(tifw, TIFFTAG_COMPRESSION, compression);
        TIFFSetField(tifw, TIFFTAG_SAMPLEFORMAT, sampleformat);
        TIFFSetField(tifw, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tifw, TIFFTAG_TILELENGTH, tileh);

        tmsize_t tilesize = TIFFTileSize(tifw);
        if (tilesize > 0 && tilesize < (tmsize_t)(1 << 20)) {
            unsigned char *tilebuf = (unsigned char *)_TIFFmalloc((tmsize_t)tilesize);
            if (tilebuf) {
                if (Size > 0) {
                    for (tmsize_t i = 0; i < tilesize; i++) {
                        tilebuf[i] = Data[(size_t)i % Size];
                    }
                } else {
                    memset(tilebuf, 0, (size_t)tilesize);
                }

                uint32_t x = 0, y = 0, z = 0;
                uint16_t s = 0;
                if (Size >= 17) {
                    x = rd32(Data + 13);
                }
                if (Size >= 21) {
                    y = rd32(Data + 17);
                }
                if (Size >= 25) {
                    z = rd32(Data + 21);
                }
                if (Size >= 27) {
                    s = (uint16_t)(rd16(Data + 25) % (spp ? spp : 1));
                }

                (void)TIFFComputeTile(tifw, x, y, z, s);
                (void)TIFFCurrentTile(tifw);
                (void)TIFFCurrentStrip(tifw);

                (void)TIFFWriteTile(tifw, tilebuf, x, y, z, s);
                (void)TIFFCurrentTile(tifw);
                (void)TIFFCurrentStrip(tifw);

                uint32_t tile_index = TIFFComputeTile(tifw, x, y, z, s);
                (void)TIFFWriteEncodedTile(tifw, tile_index, tilebuf, tilesize);
                if (tilesize > 1) {
                    (void)TIFFWriteEncodedTile(tifw, tile_index, tilebuf, tilesize / 2);
                }
                (void)TIFFWriteEncodedTile(tifw, tile_index, tilebuf, (tmsize_t)-1);

                if (width > 0 && height > 0) {
                    uint32_t x2 = width - 1;
                    uint32_t y2 = height - 1;
                    uint16_t s2 = (planar == PLANARCONFIG_SEPARATE && spp > 0) ? (uint16_t)(spp - 1) : 0;
                    uint32_t tile_index2 = TIFFComputeTile(tifw, x2, y2, 0, s2);
                    (void)TIFFWriteTile(tifw, tilebuf, x2, y2, 0, s2);
                    (void)TIFFWriteEncodedTile(tifw, tile_index2, tilebuf, tilesize);
                    (void)TIFFCurrentTile(tifw);
                    (void)TIFFCurrentStrip(tifw);
                }

                _TIFFfree(tilebuf);
            }
        }
        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        if (TIFFIsTiled(tifr)) {
            tmsize_t rtilesize = TIFFTileSize(tifr);
            if (rtilesize > 0 && rtilesize < (tmsize_t)(1 << 20)) {
                unsigned char *rbuf = (unsigned char *)_TIFFmalloc((tmsize_t)rtilesize);
                if (rbuf) {
                    memset(rbuf, 0, (size_t)rtilesize);

                    uint32_t x = 0, y = 0, z = 0;
                    uint16_t s = 0;
                    if (Size >= 31) x = rd32(Data + 27);
                    if (Size >= 35) y = rd32(Data + 31);
                    if (Size >= 39) z = rd32(Data + 35);
                    if (Size >= 41) s = rd16(Data + 39);

                    (void)TIFFComputeTile(tifr, x, y, z, s);
                    (void)TIFFReadTile(tifr, rbuf, x, y, z, s);
                    (void)TIFFCurrentTile(tifr);
                    (void)TIFFCurrentStrip(tifr);

                    (void)TIFFReadTile(tifr, rbuf, 0, 0, 0, 0);
                    (void)TIFFCurrentTile(tifr);
                    (void)TIFFCurrentStrip(tifr);
                    (void)TIFFComputeTile(tifr, 0, 0, 0, 0);

                    uint32_t rw = 0, rh = 0;
                    uint16_t rspp = 1, rplanar = PLANARCONFIG_CONTIG;
                    TIFFGetField(tifr, TIFFTAG_IMAGEWIDTH, &rw);
                    TIFFGetField(tifr, TIFFTAG_IMAGELENGTH, &rh);
                    TIFFGetFieldDefaulted(tifr, TIFFTAG_SAMPLESPERPIXEL, &rspp);
                    TIFFGetFieldDefaulted(tifr, TIFFTAG_PLANARCONFIG, &rplanar);

                    if (rw > 0 && rh > 0) {
                        uint16_t s2 = (rplanar == PLANARCONFIG_SEPARATE && rspp > 0) ? (uint16_t)(rspp - 1) : 0;
                        (void)TIFFComputeTile(tifr, rw - 1, rh - 1, 0, s2);
                        (void)TIFFReadTile(tifr, rbuf, rw - 1, rh - 1, 0, s2);
                        (void)TIFFCurrentTile(tifr);
                        (void)TIFFCurrentStrip(tifr);
                    }

                    _TIFFfree(rbuf);
                }
            }
        } else {
            (void)TIFFCurrentTile(tifr);
            (void)TIFFCurrentStrip(tifr);
        }
        TIFFClose(tifr);
    }

    return 0;
}