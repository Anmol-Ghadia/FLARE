// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadRGBATile at tif_getimage.c:2972:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_6(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        uint32_t tilewidth = 0;
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);

        tmsize_t tile_size1 = TIFFTileSize(tif);
        if (tile_size1 > 0 && tile_size1 < (tmsize_t)(1 << 24)) {
            void *buf1 = malloc((size_t)tile_size1);
            if (buf1) {
                (void)TIFFReadEncodedTile(tif, 0, buf1, tile_size1);
                free(buf1);
            }
        } else {
            uint8_t dummy = 0;
            (void)TIFFReadEncodedTile(tif, 0, &dummy, 1);
        }

        if (Size >= 4) {
            uint32_t new_tilewidth =
                ((uint32_t)Data[0] << 24) |
                ((uint32_t)Data[1] << 16) |
                ((uint32_t)Data[2] << 8) |
                (uint32_t)Data[3];
            if (new_tilewidth == 0) {
                new_tilewidth = 1;
            }
            (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, new_tilewidth);
        } else {
            (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, (uint32_t)16);
        }

        tmsize_t tile_size2 = TIFFTileSize(tif);
        if (tile_size2 > 0 && tile_size2 < (tmsize_t)(1 << 24)) {
            void *buf2 = malloc((size_t)tile_size2);
            if (buf2) {
                uint32_t tile_index = 0;
                if (Size >= 8) {
                    tile_index =
                        ((uint32_t)Data[4 % Size] << 24) |
                        ((uint32_t)Data[5 % Size] << 16) |
                        ((uint32_t)Data[6 % Size] << 8) |
                        (uint32_t)Data[7 % Size];
                }
                (void)TIFFReadEncodedTile(tif, tile_index, buf2, tile_size2);
                free(buf2);
            }
        } else {
            uint8_t dummy = 0;
            (void)TIFFReadEncodedTile(tif, 0, &dummy, 1);
        }

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        uint32_t *raster = (uint32_t *)malloc(256 * 256 * sizeof(uint32_t));
        if (raster) {
            uint32_t col = 0;
            uint32_t row = 0;
            if (Size >= 8) {
                col = ((uint32_t)Data[0] << 8) | (uint32_t)Data[1];
                row = ((uint32_t)Data[2] << 8) | (uint32_t)Data[3];
            }
            (void)TIFFReadRGBATile(tif, col, row, raster);
            free(raster);
        }
        TIFFClose(tif);
    }

    return 0;
}