// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFTileRowSize64 at tif_tile.c:142:1 in tiffio.h
// TIFFTileSize64 at tif_tile.c:252:1 in tiffio.h
// TIFFVTileSize64 at tif_tile.c:192:1 in tiffio.h
// TIFFVTileSize64 at tif_tile.c:192:1 in tiffio.h
// TIFFVTileSize64 at tif_tile.c:192:1 in tiffio.h
// TIFFScanlineSize64 at tif_strip.c:250:1 in tiffio.h
// TIFFRasterScanlineSize64 at tif_strip.c:322:1 in tiffio.h
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
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFScanlineSize64 at tif_strip.c:250:1 in tiffio.h
// TIFFRasterScanlineSize64 at tif_strip.c:322:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
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

static void setup_directory_from_data(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd32(Data, Size, 0, 1);
    uint32_t height = rd32(Data, Size, 4, 1);
    uint16_t spp = rd16(Data, Size, 8, 1);
    uint16_t bps = rd16(Data, Size, 10, 8);
    uint16_t planar = (rd16(Data, Size, 12, PLANARCONFIG_CONTIG) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photo_choices[] = {
        PHOTOMETRIC_MINISBLACK,
        PHOTOMETRIC_RGB,
        PHOTOMETRIC_YCBCR,
        PHOTOMETRIC_PALETTE
    };
    uint16_t photo = photo_choices[rd16(Data, Size, 14, 0) % 4];
    uint32_t rowsperstrip = rd32(Data, Size, 16, 1);
    uint32_t tilew = rd32(Data, Size, 20, 16);
    uint32_t tileh = rd32(Data, Size, 24, 16);
    uint16_t ycbcrsubx = (uint16_t)((rd16(Data, Size, 28, 2) % 8) + 1);
    uint16_t ycbcrsuby = (uint16_t)((rd16(Data, Size, 30, 2) % 8) + 1);

    if ((Size > 32) && (Data[32] & 1)) width = 0;
    if ((Size > 33) && (Data[33] & 1)) height = 0;
    if ((Size > 34) && (Data[34] & 1)) spp = 0;
    if ((Size > 35) && (Data[35] & 1)) tilew = 0;
    if ((Size > 36) && (Data[36] & 1)) tileh = 0;
    if ((Size > 37) && (Data[37] & 1)) rowsperstrip = 0;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);

    if (photo == PHOTOMETRIC_YCBCR) {
        TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, ycbcrsubx, ycbcrsuby);
    }

    if (tiled) {
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
    } else {
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip ? rowsperstrip : 1);
    }
}

int LLVMFuzzerTestOneInput_51(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    setup_directory_from_data(tif, Data, Size, 0);

    (void)TIFFScanlineSize64(tif);
    (void)TIFFRasterScanlineSize64(tif);

    {
        uint32_t nrows1 = rd32(Data, Size, 40, 0);
        uint32_t nrows2 = (uint32_t)-1;
        uint32_t nrows3 = rd32(Data, Size, 44, 1);

        (void)TIFFVStripSize64(tif, nrows1);
        (void)TIFFVStripSize64(tif, nrows2);
        (void)TIFFVStripSize64(tif, nrows3);
    }

    (void)TIFFWriteDirectory(tif);

    setup_directory_from_data(tif, Data, Size, 1);

    (void)TIFFTileRowSize64(tif);
    (void)TIFFTileSize64(tif);

    {
        uint32_t nrows1 = rd32(Data, Size, 48, 0);
        uint32_t nrows2 = rd32(Data, Size, 52, 1);
        uint32_t nrows3 = rd32(Data, Size, 56, 0xFFFFFFFFU);

        (void)TIFFVTileSize64(tif, nrows1);
        (void)TIFFVTileSize64(tif, nrows2);
        (void)TIFFVTileSize64(tif, nrows3);
    }

    (void)TIFFScanlineSize64(tif);
    (void)TIFFRasterScanlineSize64(tif);

    TIFFClose(tif);
    return 0;
}