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

    TIFF *tif = TIFFOpen("./dummy_file", "w+");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1, tilew = 16, tileh = 16;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = 1;

    if (Size >= 4) width = (rd32(Data) % 128) + 1;
    if (Size >= 8) height = (rd32(Data + 4) % 128) + 1;
    if (Size >= 10) bps = (uint16_t)(((Data[8] % 4) + 1) * 8);
    if (Size >= 11) spp = (uint16_t)((Data[9] % 4) + 1);
    if (Size >= 15) {
        tilew = (rd32(Data + 10) % 64) + 1;
        tileh = (Size >= 19) ? (rd32(Data + 14) % 64) + 1 : 16;
    }
    if (Size >= 20) rowsperstrip = (rd32(Data + 16) % height) + 1;
    if (Size >= 21) photo = (Data[20] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
    TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);

    TIFFDeferStrileArrayWriting(tif);
    TIFFWriteCheck(tif, 1, "fuzz");
    TIFFWriteDirectory(tif);
    TIFFSetDirectory(tif, 0);
    TIFFForceStrileArrayWriting(tif);
    TIFFSetDirectory(tif, 0);
    TIFFForceStrileArrayWriting(tif);
    TIFFSetDirectory(tif, 0);

    tmsize_t tileSize = TIFFTileSize(tif);
    if (tileSize <= 0) {
        tileSize = (tmsize_t)((Size > 0 && Size < 4096) ? Size : 256);
    }
    void *tileBuf = NULL;
    if (tileSize > 0) {
        tileBuf = malloc((size_t)tileSize);
    }
    if (tileBuf) {
        if (Size > 0) {
            size_t copy = (size_t)tileSize < Size ? (size_t)tileSize : Size;
            memcpy(tileBuf, Data, copy);
            if ((size_t)tileSize > copy) {
                memset((uint8_t *)tileBuf + copy, 0, (size_t)tileSize - copy);
            }
        } else {
            memset(tileBuf, 0, (size_t)tileSize);
        }
    }

    TIFFWriteEncodedTile(tif, 0, tileBuf, tileSize);

    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    tmsize_t stripSize = TIFFStripSize(tif);
    if (stripSize <= 0) {
        stripSize = (tmsize_t)((Size > 0 && Size < 4096) ? Size : 256);
    }
    void *stripBuf = NULL;
    if (stripSize > 0) {
        stripBuf = malloc((size_t)stripSize);
    }
    if (stripBuf) {
        if (Size > 0) {
            size_t copy = (size_t)stripSize < Size ? (size_t)stripSize : Size;
            memcpy(stripBuf, Data, copy);
            if ((size_t)stripSize > copy) {
                memset((uint8_t *)stripBuf + copy, 0xAA, (size_t)stripSize - copy);
            }
        } else {
            memset(stripBuf, 0xAA, (size_t)stripSize);
        }
    }

    TIFFWriteEncodedStrip(tif, 0, stripBuf, stripSize);
    TIFFClose(tif);
    tif = NULL;

    TIFF *tif_tile = TIFFOpen("./dummy_file", "r");
    if (tif_tile) {
        tmsize_t rTileSize = TIFFTileSize(tif_tile);
        if (rTileSize <= 0) {
            rTileSize = tileSize > 0 ? tileSize : 256;
        }
        void *readTileBuf = NULL;
        if (rTileSize > 0) {
            readTileBuf = malloc((size_t)rTileSize);
        }
        if (readTileBuf) {
            TIFFReadEncodedTile(tif_tile, 0, readTileBuf, rTileSize);
            free(readTileBuf);
        } else {
            TIFFReadEncodedTile(tif_tile, 0, NULL, 0);
        }
        TIFFClose(tif_tile);
    }

    TIFF *tif_strip = TIFFOpen("./dummy_file", "r");
    if (tif_strip) {
        tmsize_t rStripSize = TIFFStripSize(tif_strip);
        if (rStripSize <= 0) {
            rStripSize = stripSize > 0 ? stripSize : 256;
        }
        void *readStripBuf = NULL;
        if (rStripSize > 0) {
            readStripBuf = malloc((size_t)rStripSize);
        }
        if (readStripBuf) {
            TIFFReadEncodedStrip(tif_strip, 0, readStripBuf, rStripSize);
            free(readStripBuf);
        } else {
            TIFFReadEncodedStrip(tif_strip, 0, NULL, 0);
        }
        TIFFClose(tif_strip);
    }

    free(tileBuf);
    free(stripBuf);

    return 0;
}