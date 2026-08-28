// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFRasterScanlineSize64 at tif_strip.c:322:1 in tiffio.h
// TIFFTileSize64 at tif_tile.c:252:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFTileRowSize at tif_tile.c:180:1 in tiffio.h
// TIFFTileRowSize64 at tif_tile.c:142:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

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

static void set_common_tags(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd32(Data, Size, 0, 1);
    uint32_t height = rd32(Data, Size, 4, 1);
    uint16_t bps = rd16(Data, Size, 8, 8);
    uint16_t spp = rd16(Data, Size, 10, 1);
    uint16_t planar = (rd16(Data, Size, 12, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photo = (rd16(Data, Size, 14, 0) % 6);
    uint16_t compression = (rd16(Data, Size, 16, 0) & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    uint16_t fillorder = (rd16(Data, Size, 18, 0) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;
    uint32_t rowsperstrip = rd32(Data, Size, 20, 1);
    uint32_t tilew = rd32(Data, Size, 24, 16);
    uint32_t tilel = rd32(Data, Size, 28, 16);

    if (width == 0) width = 1;
    if (height == 0) height = 1;
    if (bps == 0) bps = 1;
    if (spp == 0) spp = 1;
    if (rowsperstrip == 0) rowsperstrip = 1;
    if (tilew == 0) tilew = 1;
    if (tilel == 0) tilel = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);

    if (tiled) {
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tilel);
    } else {
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

static void exercise_api(TIFF *tif, int tiled) {
    int err = 0;
    uint64_t v1 = TIFFRasterScanlineSize64(tif);
    uint64_t v2 = TIFFTileSize64(tif);
    tmsize_t v3 = TIFFStripSize(tif);
    tmsize_t v4 = TIFFTileRowSize(tif);
    uint64_t v5 = TIFFTileRowSize64(tif);
    uint64_t v6 = TIFFGetStrileByteCountWithErr(tif, 0, &err);

    (void)v1;
    (void)v2;
    (void)v3;
    (void)v4;
    (void)v5;
    (void)v6;
    (void)tiled;

    err = 0;
    (void)TIFFGetStrileByteCountWithErr(tif, 1, &err);
    err = 0;
    (void)TIFFGetStrileByteCountWithErr(tif, 0xffffffffU, &err);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (tif) {
        set_common_tags(tif, Data, Size, 0);
        exercise_api(tif, 0);

        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);
        exercise_api(tif, 0);

        TIFFClose(tif);
    }

    TIFF *tif_tile = TIFFOpen("./dummy_file", "w8");
    if (tif_tile) {
        set_common_tags(tif_tile, Data, Size, 1);
        exercise_api(tif_tile, 1);

        TIFFSetField(tif_tile, TIFFTAG_TILEWIDTH, (uint32_t)1);
        TIFFSetField(tif_tile, TIFFTAG_TILELENGTH, (uint32_t)1);
        exercise_api(tif_tile, 1);

        TIFFClose(tif_tile);
    }

    TIFF *tif_read = TIFFOpen("./dummy_file", "r");
    if (tif_read) {
        exercise_api(tif_read, 0);
        TIFFClose(tif_read);
    }

    return 0;
}