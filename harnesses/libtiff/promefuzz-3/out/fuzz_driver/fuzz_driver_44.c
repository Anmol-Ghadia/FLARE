// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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
// TIFFSetupStrips at tif_write.c:523:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
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

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t dflt) {
    if (off + 2 > n) return dflt;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t dflt) {
    if (off + 4 > n) return dflt;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w8");
        if (!tif) return 0;
    }

    uint32_t width = rd32(Data, Size, 0, 32) % 2048 + 1;
    uint32_t height = rd32(Data, Size, 4, 32) % 2048 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 4 + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10, 3) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (rd16(Data, Size, 12, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    uint32_t def_tw = rd32(Data, Size, 14, 0);
    uint32_t def_th = rd32(Data, Size, 18, 0);
    TIFFDefaultTileSize(tif, &def_tw, &def_th);
    if (def_tw == 0) def_tw = 16;
    if (def_th == 0) def_th = 16;

    uint32_t tile_w = rd32(Data, Size, 22, def_tw);
    uint32_t tile_h = rd32(Data, Size, 26, def_th);

    tile_w = (tile_w % 512) + 1;
    tile_h = (tile_h % 512) + 1;

    if (tile_w < 16) tile_w = 16;
    if (tile_h < 16) tile_h = 16;

    tile_w = ((tile_w + 15) / 16) * 16;
    tile_h = ((tile_h + 15) / 16) * 16;

    (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tile_w);
    (void)TIFFSetField(tif, TIFFTAG_TILELENGTH, tile_h);

    (void)TIFFSetupStrips(tif);

    (void)TIFFDefaultStripSize(tif, rd32(Data, Size, 30, 0));
    (void)TIFFDefaultStripSize(tif, 0);
    (void)TIFFDefaultStripSize(tif, 1);
    (void)TIFFDefaultStripSize(tif, 0xFFFFFFFFU);

    tmsize_t tile_size = TIFFTileSize(tif);
    if (tile_size < 0) tile_size = 0;

    uint32_t tile_index = rd32(Data, Size, 34, 0);
    int err = 0;
    (void)TIFFGetStrileByteCountWithErr(tif, tile_index, &err);
    (void)TIFFGetStrileByteCountWithErr(tif, 0, &err);
    (void)TIFFGetStrileByteCountWithErr(tif, 0xFFFFFFFFU, &err);

    if (tile_size > 0 && tile_size < (tmsize_t)(1 << 20)) {
        void *buf = malloc((size_t)tile_size);
        if (buf) {
            if (Size > 0) {
                size_t copy = (size_t)tile_size < Size ? (size_t)tile_size : Size;
                memcpy(buf, Data, copy);
                if ((size_t)tile_size > copy) {
                    memset((uint8_t *)buf + copy, 0, (size_t)tile_size - copy);
                }
            } else {
                memset(buf, 0, (size_t)tile_size);
            }

            (void)TIFFWriteEncodedTile(tif, 0, buf, tile_size);
            (void)TIFFWriteEncodedTile(tif, tile_index, buf, tile_size);
            (void)TIFFWriteEncodedTile(tif, tile_index, buf, tile_size / 2);
            (void)TIFFWriteEncodedTile(tif, 0xFFFFFFFFU, buf, tile_size);

            free(buf);
        }
    } else {
        uint8_t dummy = 0;
        (void)TIFFWriteEncodedTile(tif, 0, &dummy, 1);
    }

    TIFFClose(tif);
    return 0;
}