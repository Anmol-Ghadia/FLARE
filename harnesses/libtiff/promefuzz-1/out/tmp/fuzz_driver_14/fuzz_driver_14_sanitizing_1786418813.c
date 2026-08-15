#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 1 >= n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 3 >= n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void set_common_fields(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd32(Data, Size, 0, 1);
    uint32_t length = rd32(Data, Size, 4, 1);
    uint16_t spp = rd16(Data, Size, 8, 1);
    uint16_t bps = rd16(Data, Size, 10, 8);
    uint16_t planar = (rd16(Data, Size, 12, PLANARCONFIG_CONTIG) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photo_choices[] = {
        PHOTOMETRIC_MINISBLACK,
        PHOTOMETRIC_RGB,
        PHOTOMETRIC_YCBCR,
        PHOTOMETRIC_PALETTE,
        PHOTOMETRIC_SEPARATED
    };
    uint16_t photo = photo_choices[(Size > 14 ? Data[14] : 0) % (sizeof(photo_choices) / sizeof(photo_choices[0]))];
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = rd32(Data, Size, 16, 1);
    uint16_t ycbcrsubsampling_h = (uint16_t)((Size > 20 ? Data[20] : 2) ? (Size > 20 ? Data[20] : 2) : 1);
    uint16_t ycbcrsubsampling_v = (uint16_t)((Size > 21 ? Data[21] : 2) ? (Size > 21 ? Data[21] : 2) : 1);

    if (width == 0) width = 1;
    if (length == 0) length = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    if (photo == PHOTOMETRIC_YCBCR) {
        TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, ycbcrsubsampling_h, ycbcrsubsampling_v);
    }

    if (tiled) {
        uint32_t tw = rd32(Data, Size, 24, 16);
        uint32_t tl = rd32(Data, Size, 28, 16);
        uint32_t td = rd32(Data, Size, 32, 1);
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tl);
#ifdef TIFFTAG_TILEDEPTH
        TIFFSetField(tif, TIFFTAG_TILEDEPTH, td);
#else
        (void)td;
#endif
    } else {
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (tif) {
        set_common_fields(tif, Data, Size, 0);

        (void)TIFFScanlineSize64(tif);
        (void)TIFFRasterScanlineSize64(tif);
        (void)TIFFStripSize64(tif);

        {
            uint32_t nrows1 = rd32(Data, Size, 36, 1);
            uint32_t nrows2 = (uint32_t)-1;
            uint32_t nrows3 = rd32(Data, Size, 40, 0);

            (void)TIFFVStripSize64(tif, nrows1);
            (void)TIFFVStripSize64(tif, nrows2);
            (void)TIFFVStripSize64(tif, nrows3);
        }

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "w");
    if (tif) {
        set_common_fields(tif, Data, Size, 1);

        (void)TIFFScanlineSize64(tif);
        (void)TIFFRasterScanlineSize64(tif);
        (void)TIFFTileRowSize64(tif);

        {
            uint32_t nrows1 = rd32(Data, Size, 44, 1);
            uint32_t nrows2 = rd32(Data, Size, 48, 0);
            uint32_t nrows3 = (uint32_t)-1;

            (void)TIFFVTileSize64(tif, nrows1);
            (void)TIFFVTileSize64(tif, nrows2);
            (void)TIFFVTileSize64(tif, nrows3);
        }

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "w");
    if (tif) {
        uint32_t width = rd32(Data, Size, 52, 0);
        uint32_t length = rd32(Data, Size, 56, 0);
        uint16_t spp = rd16(Data, Size, 60, 0);
        uint16_t bps = rd16(Data, Size, 62, 1);
        uint16_t planar = (Size > 64 && (Data[64] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
        uint16_t photo = (Size > 65 && (Data[65] & 1)) ? PHOTOMETRIC_YCBCR : PHOTOMETRIC_RGB;

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rd32(Data, Size, 66, 0));
        if (photo == PHOTOMETRIC_YCBCR) {
            uint16_t h = (uint16_t)(Size > 70 ? Data[70] : 0);
            uint16_t v = (uint16_t)(Size > 71 ? Data[71] : 0);
            TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, h, v);
        }

        (void)TIFFScanlineSize64(tif);
        (void)TIFFRasterScanlineSize64(tif);
        (void)TIFFStripSize64(tif);
        (void)TIFFVStripSize64(tif, rd32(Data, Size, 72, 0));

        TIFFClose(tif);
    }

    remove("./dummy_file");
    return 0;
}