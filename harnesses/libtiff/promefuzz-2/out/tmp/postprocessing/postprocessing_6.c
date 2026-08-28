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
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static tmsize_t clamp_tile_buf_size(tmsize_t sz) {
    const tmsize_t kMax = 1 << 20;
    if (sz <= 0) return 4096;
    if (sz > kMax) return kMax;
    return sz;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        uint32_t imagewidth = 0, imagelength = 0;
        uint32_t tilewidth = 0, tilelength = 0;

        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);

        tmsize_t tile_size1 = TIFFTileSize(tif);
        tmsize_t buf_size1 = clamp_tile_buf_size(tile_size1);
        void *buf1 = malloc((size_t)buf_size1);
        if (buf1) {
            memset(buf1, 0, (size_t)buf_size1);
            uint32_t tile_index1 = 0;
            if (Size >= 4) {
                tile_index1 = ((uint32_t)Data[0] << 24) |
                              ((uint32_t)Data[1] << 16) |
                              ((uint32_t)Data[2] << 8) |
                              (uint32_t)Data[3];
            }
            (void)TIFFReadEncodedTile(tif, tile_index1, buf1, buf_size1);
            free(buf1);
        }

        if (tilewidth != 0) {
            uint32_t new_tilewidth = tilewidth;
            if (Size >= 8) {
                new_tilewidth ^= (((uint32_t)Data[4] << 8) | (uint32_t)Data[5]);
                if (new_tilewidth == 0) new_tilewidth = tilewidth;
            }
            (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, new_tilewidth);
        } else if (Size >= 2) {
            uint32_t candidate = (((uint32_t)Data[0] << 8) | (uint32_t)Data[1]);
            if (candidate == 0) candidate = 16;
            (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, candidate);
        }

        tmsize_t tile_size2 = TIFFTileSize(tif);
        tmsize_t buf_size2 = clamp_tile_buf_size(tile_size2);
        void *buf2 = malloc((size_t)buf_size2);
        if (buf2) {
            memset(buf2, 0xA5, (size_t)buf_size2);
            uint32_t tile_index2 = 0;
            if (Size >= 8) {
                tile_index2 = ((uint32_t)Data[Size - 4] << 24) |
                              ((uint32_t)Data[Size - 3] << 16) |
                              ((uint32_t)Data[Size - 2] << 8) |
                              (uint32_t)Data[Size - 1];
            }
            (void)TIFFReadEncodedTile(tif, tile_index2, buf2, buf_size2);
            free(buf2);
        }

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        uint32_t tw = 0, tl = 0;
        uint32_t col = 0, row = 0;

        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tl);

        if (tw == 0) tw = 16;
        if (tl == 0) tl = 16;

        if (Size >= 2) {
            col = (uint32_t)Data[0] % tw;
            row = (uint32_t)Data[1] % tl;
        }

        size_t rgba_count = (size_t)tw * (size_t)tl;
        if (rgba_count == 0 || rgba_count > (1U << 20)) {
            rgba_count = 256;
        }

        uint32_t *raster = (uint32_t *)malloc(rgba_count * sizeof(uint32_t));
        if (raster) {
            memset(raster, 0, rgba_count * sizeof(uint32_t));
            (void)TIFFReadRGBATile(tif, col, row, raster);
            free(raster);
        }

        TIFFClose(tif);
    }

    return 0;
}