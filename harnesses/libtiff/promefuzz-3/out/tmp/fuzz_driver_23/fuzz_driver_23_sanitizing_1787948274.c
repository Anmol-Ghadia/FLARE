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
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I', 42,0, 8,0,0,0, 0,0
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

static void setup_basic_tiled_tags(TIFF *tif,
                                   uint32_t width,
                                   uint32_t height,
                                   uint16_t spp,
                                   uint16_t bps,
                                   uint16_t photometric,
                                   uint16_t compression,
                                   uint16_t planarconfig,
                                   uint32_t tilew,
                                   uint32_t tileh) {
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
    TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
}

static void setup_basic_striped_tags(TIFF *tif,
                                     uint32_t width,
                                     uint32_t height,
                                     uint16_t spp,
                                     uint16_t bps,
                                     uint16_t photometric,
                                     uint16_t compression,
                                     uint16_t planarconfig,
                                     uint32_t rowsperstrip) {
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    uint32_t width = 16, height = 16, tilew = 16, tileh = 16, rowsperstrip = 16;
    uint16_t spp = 1, bps = 8;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t planarconfig = PLANARCONFIG_CONTIG;

    if (Size >= 4) {
        width = (rd16(Data) % 128) + 1;
        height = (rd16(Data + 2) % 128) + 1;
    }
    if (Size >= 8) {
        tilew = (rd16(Data + 4) % 64) + 1;
        tileh = (rd16(Data + 6) % 64) + 1;
    }
    if (Size >= 10) {
        rowsperstrip = (rd16(Data + 8) % 128) + 1;
    }
    if (Size >= 11) {
        spp = (uint16_t)((Data[10] % 4) + 1);
    }
    if (Size >= 12) {
        uint8_t b = Data[11] % 4;
        bps = (b == 0) ? 1 : (b == 1) ? 8 : (b == 2) ? 16 : 32;
    }
    if (Size >= 13) {
        photometric = (Data[12] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        if (photometric == PHOTOMETRIC_RGB && spp < 3) spp = 3;
    }
    if (Size >= 14) {
        compression = (Data[13] & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }
    if (Size >= 15) {
        planarconfig = (Data[14] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }

    if (tilew > width) tilew = width;
    if (tileh > height) tileh = height;
    if (rowsperstrip > height) rowsperstrip = height;

    TIFF *tifw_tile = TIFFOpen("./dummy_file", "w");
    if (tifw_tile) {
        setup_basic_tiled_tags(tifw_tile, width, height, spp, bps, photometric,
                               compression, planarconfig, tilew, tileh);

        tmsize_t vts1 = TIFFVTileSize(tifw_tile, tileh);
        tmsize_t vts2 = TIFFVTileSize(tifw_tile, (height > 0) ? height : 1);
        tmsize_t vts3 = TIFFVTileSize(tifw_tile, 0);

        (void)vts1;
        (void)vts2;
        (void)vts3;

        uint32_t ntiles = TIFFNumberOfTiles(tifw_tile);
        if (ntiles == 0) ntiles = 1;

        tmsize_t tilesize = TIFFTileSize(tifw_tile);
        if (tilesize < 0) tilesize = 0;

        size_t allocsz = 1;
        if (tilesize > 0 && (size_t)tilesize < (1U << 20)) {
            allocsz = (size_t)tilesize;
        } else if (Size > 0 && Size < (1U << 20)) {
            allocsz = Size;
        }

        unsigned char *buf = (unsigned char *)malloc(allocsz ? allocsz : 1);
        if (buf) {
            if (Size > 0) {
                for (size_t i = 0; i < allocsz; ++i) {
                    buf[i] = Data[i % Size];
                }
            } else {
                memset(buf, 0, allocsz);
            }

            uint32_t tile_indices[4];
            tile_indices[0] = 0;
            tile_indices[1] = (ntiles > 0) ? (ntiles - 1) : 0;
            tile_indices[2] = (Size >= 19) ? (rd32(Data + 15) % (ntiles + 1)) : 1;
            tile_indices[3] = ntiles + 1;

            tmsize_t ccs[4];
            ccs[0] = (tmsize_t)allocsz;
            ccs[1] = (tilesize > 0) ? tilesize : (tmsize_t)allocsz;
            ccs[2] = (Size >= 20) ? (tmsize_t)(Data[19] % (allocsz + 1)) : 0;
            ccs[3] = (tmsize_t)-1;

            for (size_t i = 0; i < 4; ++i) {
                TIFFWriteEncodedTile(tifw_tile, tile_indices[i], buf, ccs[i]);
                TIFFWriteRawTile(tifw_tile, tile_indices[i], buf, ccs[i] < 0 ? 0 : ccs[i]);
            }

            free(buf);
        }

        TIFFClose(tifw_tile);
    }

    TIFF *tifw_strip = TIFFOpen("./dummy_file", "w");
    if (tifw_strip) {
        setup_basic_striped_tags(tifw_strip, width, height, spp, bps, photometric,
                                 compression, planarconfig, rowsperstrip);

        uint32_t nstrips = TIFFNumberOfStrips(tifw_strip);
        if (nstrips == 0) nstrips = 1;

        tmsize_t stripsize = TIFFStripSize(tifw_strip);
        if (stripsize < 0) stripsize = 0;

        size_t allocsz = 1;
        if (stripsize > 0 && (size_t)stripsize < (1U << 20)) {
            allocsz = (size_t)stripsize;
        } else if (Size > 0 && Size < (1U << 20)) {
            allocsz = Size;
        }

        unsigned char *buf = (unsigned char *)malloc(allocsz ? allocsz : 1);
        if (buf) {
            if (Size > 0) {
                for (size_t i = 0; i < allocsz; ++i) {
                    buf[i] = Data[i % Size];
                }
            } else {
                memset(buf, 0xA5, allocsz);
            }

            uint32_t strip_indices[4];
            strip_indices[0] = 0;
            strip_indices[1] = (nstrips > 0) ? (nstrips - 1) : 0;
            strip_indices[2] = (Size >= 24) ? (rd32(Data + 20) % (nstrips + 1)) : 1;
            strip_indices[3] = nstrips + 1;

            tmsize_t ccs[4];
            ccs[0] = (tmsize_t)allocsz;
            ccs[1] = (stripsize > 0) ? stripsize : (tmsize_t)allocsz;
            ccs[2] = (Size >= 25) ? (tmsize_t)(Data[24] % (allocsz + 1)) : 0;
            ccs[3] = (tmsize_t)-1;

            for (size_t i = 0; i < 4; ++i) {
                TIFFWriteRawStrip(tifw_strip, strip_indices[i], buf, ccs[i]);
            }

            free(buf);
        }

        TIFFClose(tifw_strip);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        if (TIFFIsTiled(tifr)) {
            uint32_t ntiles = TIFFNumberOfTiles(tifr);
            tmsize_t tilesize = TIFFTileSize(tifr);
            if (tilesize < 0) tilesize = 0;

            size_t allocsz = 1;
            if (tilesize > 0 && (size_t)tilesize < (1U << 20)) {
                allocsz = (size_t)tilesize;
            } else if (Size > 0 && Size < (1U << 20)) {
                allocsz = Size;
            }

            unsigned char *buf = (unsigned char *)malloc(allocsz ? allocsz : 1);
            if (buf) {
                uint32_t tile_indices[4];
                tile_indices[0] = 0;
                tile_indices[1] = (ntiles > 0) ? (ntiles - 1) : 0;
                tile_indices[2] = (Size >= 29) ? rd32(Data + 25) : 1;
                tile_indices[3] = ntiles + 1;

                tmsize_t sizes[4];
                sizes[0] = (tmsize_t)allocsz;
                sizes[1] = (tilesize > 0) ? tilesize : (tmsize_t)allocsz;
                sizes[2] = (tmsize_t)-1;
                sizes[3] = (Size >= 30) ? (tmsize_t)(Data[29] % (allocsz + 1)) : 0;

                for (size_t i = 0; i < 4; ++i) {
                    TIFFReadEncodedTile(tifr, tile_indices[i], buf, sizes[i]);
                    TIFFReadRawTile(tifr, tile_indices[i], buf, sizes[i]);
                }

                free(buf);
            }
        } else {
            uint32_t maybe_tile = 0;
            unsigned char smallbuf[16];
            TIFFReadEncodedTile(tifr, maybe_tile, smallbuf, (tmsize_t)sizeof(smallbuf));
            TIFFReadRawTile(tifr, maybe_tile, smallbuf, (tmsize_t)sizeof(smallbuf));
        }

        TIFFClose(tifr);
    }

    return 0;
}