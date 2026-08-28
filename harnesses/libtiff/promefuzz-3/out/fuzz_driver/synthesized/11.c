// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFWriteScanline at tif_write.c:47:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFWriteScanline at tif_write.c:47:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    uint32_t width = rd32(Data, Size, 0, 1);
    uint32_t height = rd32(Data, Size, 4, 1);
    uint16_t spp = (uint16_t)((rd16(Data, Size, 8, 1) % 4) + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10, 0) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    uint16_t planar = (rd16(Data, Size, 12, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;

    if (width == 0) width = 1;
    if (height == 0) height = 1;
    width = (width % 2048) + 1;
    height = (height % 2048) + 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1U);

    {
        uint32_t got_width = 0;
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &got_width);

        tmsize_t scanline = TIFFScanlineSize(tif);
        if (scanline > 0 && scanline < (tmsize_t)(1 << 20)) {
            unsigned char *buf = (unsigned char *)malloc((size_t)scanline);
            if (buf) {
                if (Size > 0) {
                    size_t i;
                    for (i = 0; i < (size_t)scanline; i++) {
                        buf[i] = Data[i % Size];
                    }
                } else {
                    memset(buf, 0, (size_t)scanline);
                }

                uint32_t row = (height > 0) ? (rd32(Data, Size, 16, 0) % height) : 0;
                uint16_t sample = 0;
                if (planar == PLANARCONFIG_SEPARATE && spp > 0) {
                    sample = (uint16_t)(rd16(Data, Size, 20, 0) % spp);
                }

                (void)TIFFWriteScanline(tif, buf, row, sample);

                if (height > 1) {
                    uint32_t row2 = (row + 1) % height;
                    uint16_t sample2 = (planar == PLANARCONFIG_SEPARATE && spp > 0)
                                           ? (uint16_t)((sample + 1) % spp)
                                           : 0;
                    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &got_width);
                    (void)TIFFScanlineSize(tif);
                    (void)TIFFWriteScanline(tif, buf, row2, sample2);
                }

                free(buf);
            }
        }
    }

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}