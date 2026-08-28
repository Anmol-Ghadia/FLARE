// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
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
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFRasterScanlineSize at tif_strip.c:337:1 in tiffio.h
// TIFFRasterScanlineSize64 at tif_strip.c:322:1 in tiffio.h
// TIFFScanlineSize64 at tif_strip.c:250:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFStripSize64 at tif_strip.c:186:1 in tiffio.h
// TIFFTileRowSize64 at tif_tile.c:142:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFRasterScanlineSize at tif_strip.c:337:1 in tiffio.h
// TIFFRasterScanlineSize64 at tif_strip.c:322:1 in tiffio.h
// TIFFScanlineSize64 at tif_strip.c:250:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFStripSize64 at tif_strip.c:186:1 in tiffio.h
// TIFFTileRowSize64 at tif_tile.c:142:1 in tiffio.h
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

static void set_basic_tags(TIFF *tif, const uint8_t *Data, size_t Size, size_t off, int tiled) {
    uint32_t width = 1, length = 1, rowsperstrip = 1, tilewidth = 1, tilelength = 1;
    uint16_t bps = 1, spp = 1, planar = PLANARCONFIG_CONTIG, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t ycbcrsubsampling[2] = {2, 2};

    if (Size > off + 3) width = rd32(Data + off);
    if (Size > off + 7) length = rd32(Data + off + 4);
    if (Size > off + 9) bps = rd16(Data + off + 8);
    if (Size > off + 11) spp = rd16(Data + off + 10);
    if (Size > off + 13) planar = (rd16(Data + off + 12) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    if (Size > off + 15) photo = rd16(Data + off + 14) % 9;
    if (Size > off + 19) rowsperstrip = rd32(Data + off + 16);
    if (Size > off + 23) tilewidth = rd32(Data + off + 20);
    if (Size > off + 27) tilelength = rd32(Data + off + 24);
    if (Size > off + 29) compression = rd16(Data + off + 28);
    if (Size > off + 31) ycbcrsubsampling[0] = (uint16_t)((Data[off + 30] % 8) + 1);
    if (Size > off + 31) ycbcrsubsampling[1] = (uint16_t)((Data[off + 31] % 8) + 1);

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    if (photo == PHOTOMETRIC_YCBCR) {
        TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, ycbcrsubsampling[0], ycbcrsubsampling[1]);
    }

    if (tiled) {
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilewidth);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tilelength);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        remove("./dummy_file");
        return 0;
    }

    set_basic_tags(tif, Data, Size, 0, 0);

    (void)TIFFRasterScanlineSize(tif);
    (void)TIFFRasterScanlineSize64(tif);
    (void)TIFFScanlineSize64(tif);
    (void)TIFFStripSize(tif);
    (void)TIFFStripSize64(tif);
    (void)TIFFTileRowSize64(tif);

    TIFFWriteDirectory(tif);

    if (TIFFSetSubDirectory(tif, 0)) {
        set_basic_tags(tif, Data, Size, 8, 1);
        (void)TIFFRasterScanlineSize(tif);
        (void)TIFFRasterScanlineSize64(tif);
        (void)TIFFScanlineSize64(tif);
        (void)TIFFStripSize(tif);
        (void)TIFFStripSize64(tif);
        (void)TIFFTileRowSize64(tif);
    }

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}