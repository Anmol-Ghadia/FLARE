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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w+");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint16_t fillorder = FILLORDER_MSB2LSB;
    uint16_t rowsperstrip = 1;

    if (Size >= 4) width = (rd32(Data) % 64U) + 1U;
    if (Size >= 8) height = (rd32(Data + 4) % 64U) + 1U;
    if (Size >= 10) spp = (uint16_t)((rd16(Data + 8) % 4U) + 1U);
    if (Size >= 12) {
        static const uint16_t bps_choices[] = {1, 4, 8, 16, 32};
        bps = bps_choices[rd16(Data + 10) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    }
    if (Size >= 14) {
        static const uint16_t photo_choices[] = {
            PHOTOMETRIC_MINISWHITE,
            PHOTOMETRIC_MINISBLACK,
            PHOTOMETRIC_RGB,
            PHOTOMETRIC_PALETTE
        };
        photo = photo_choices[rd16(Data + 12) % (sizeof(photo_choices) / sizeof(photo_choices[0]))];
    }
    if (Size >= 16) {
        planar = (rd16(Data + 14) & 1U) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size >= 18) {
        compression = (rd16(Data + 16) & 1U) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }
    if (Size >= 20) {
        rowsperstrip = (uint16_t)((rd16(Data + 18) % (height ? height : 1U)) + 1U);
    }

    if (photo == PHOTOMETRIC_RGB && spp < 3) {
        spp = 3;
    }
    if (photo == PHOTOMETRIC_PALETTE) {
        spp = 1;
    }

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    (void)TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    if (photo == PHOTOMETRIC_PALETTE) {
        static uint16_t cmap_r[256], cmap_g[256], cmap_b[256];
        for (int i = 0; i < 256; i++) {
            cmap_r[i] = (uint16_t)(i << 8);
            cmap_g[i] = (uint16_t)(255 - i) << 8;
            cmap_b[i] = (uint16_t)((i ^ 0xAA) << 8);
        }
        (void)TIFFSetField(tif, TIFFTAG_COLORMAP, cmap_r, cmap_g, cmap_b);
    }

    {
        uint32_t out_width = 0;
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &out_width);
    }

    tmsize_t scanline = TIFFScanlineSize(tif);
    if (scanline <= 0 || scanline > (1 << 20)) {
        TIFFClose(tif);
        return 0;
    }

    unsigned char *buf = (unsigned char *)malloc((size_t)scanline);
    if (!buf) {
        TIFFClose(tif);
        return 0;
    }

    for (tmsize_t i = 0; i < scanline; i++) {
        buf[i] = (unsigned char)(Size ? Data[i % Size] : 0);
    }

    uint32_t row = 0;
    uint16_t sample = 0;
    if (Size >= 24) row = rd32(Data + 20) % (height + 4U);
    if (planar == PLANARCONFIG_SEPARATE && Size >= 26) {
        sample = (uint16_t)(rd16(Data + 24) % (spp ? spp : 1U));
    }

    (void)TIFFWriteScanline(tif, buf, row, sample);

    if (height > 0) {
        uint32_t row2 = height - 1;
        uint16_t sample2 = (planar == PLANARCONFIG_SEPARATE && spp > 1) ? (uint16_t)(spp - 1) : 0;
        if (scanline > 0) {
            memset(buf, (int)(Size ? Data[0] : 0xA5), (size_t)scanline);
        }
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFScanlineSize(tif);
        (void)TIFFWriteScanline(tif, buf, row2, sample2);
    }

    free(buf);
    TIFFClose(tif);
    return 0;
}