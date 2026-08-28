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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (!f) return;
    if (Size > 0) fwrite(Data, 1, Size, f);
    fclose(f);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        uint32_t width = rd32(Data, Size, 0, 1) % 2048 + 1;
        uint32_t height = rd32(Data, Size, 4, 1) % 256 + 1;
        uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
        uint16_t spp_choices[] = {1, 2, 3, 4};
        uint16_t photo_choices[] = {
            PHOTOMETRIC_MINISBLACK,
            PHOTOMETRIC_MINISWHITE,
            PHOTOMETRIC_RGB,
            PHOTOMETRIC_PALETTE
        };
        uint16_t comp_choices[] = {
            COMPRESSION_NONE,
            COMPRESSION_LZW,
            COMPRESSION_PACKBITS,
            COMPRESSION_ADOBE_DEFLATE
        };
        uint16_t planar = (Size > 8 && (Data[8] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
        uint16_t rowsperstrip = (uint16_t)((rd32(Data, Size, 9, 1) % 64) + 1);
        uint16_t bps = bps_choices[(Size > 13) ? (Data[13] % (sizeof(bps_choices)/sizeof(bps_choices[0]))) : 0];
        uint16_t spp = spp_choices[(Size > 14) ? (Data[14] % (sizeof(spp_choices)/sizeof(spp_choices[0]))) : 0];
        uint16_t photo = photo_choices[(Size > 15) ? (Data[15] % (sizeof(photo_choices)/sizeof(photo_choices[0]))) : 0];
        uint16_t comp = comp_choices[(Size > 16) ? (Data[16] % (sizeof(comp_choices)/sizeof(comp_choices[0]))) : 0];

        if (photo == PHOTOMETRIC_RGB && spp < 3) spp = 3;
        if (photo == PHOTOMETRIC_PALETTE) spp = 1;

        if (!(Size > 17 && (Data[17] & 1))) TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
        if (!(Size > 18 && (Data[18] & 1))) TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(tifw, TIFFTAG_COMPRESSION, comp);
        TIFFSetField(tifw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        if (photo == PHOTOMETRIC_PALETTE) {
            uint16_t *r = (uint16_t *)_TIFFmalloc(256 * sizeof(uint16_t));
            uint16_t *g = (uint16_t *)_TIFFmalloc(256 * sizeof(uint16_t));
            uint16_t *b = (uint16_t *)_TIFFmalloc(256 * sizeof(uint16_t));
            if (r && g && b) {
                size_t i;
                for (i = 0; i < 256; i++) {
                    uint8_t v = (Size > 19 + i) ? Data[19 + i] : (uint8_t)i;
                    r[i] = (uint16_t)(v << 8);
                    g[i] = (uint16_t)(v << 8);
                    b[i] = (uint16_t)(v << 8);
                }
                TIFFSetField(tifw, TIFFTAG_COLORMAP, r, g, b);
            }
            if (r) _TIFFfree(r);
            if (g) _TIFFfree(g);
            if (b) _TIFFfree(b);
        }

        (void)TIFFWriteCheck(tifw, (Size > 300 && (Data[300] & 1)) ? 1 : 0, "fuzz");

        tmsize_t scanline = TIFFScanlineSize(tifw);
        if (scanline > 0 && scanline < (1 << 20)) {
            unsigned char *buf = (unsigned char *)_TIFFmalloc((tmsize_t)scanline);
            if (buf) {
                if (Size > 0) {
                    tmsize_t i;
                    for (i = 0; i < scanline; i++) {
                        buf[i] = Data[(size_t)i % Size];
                    }
                } else {
                    memset(buf, 0, (size_t)scanline);
                }

                uint32_t row = (height > 0) ? (rd32(Data, Size, 20, 0) % (height + 2)) : 0;
                uint16_t sample = 0;
                if (planar == PLANARCONFIG_SEPARATE && spp > 0)
                    sample = (uint16_t)(rd16(Data, Size, 24, 0) % (spp + 1));

                (void)TIFFWriteScanline(tifw, buf, row, sample);

                uint32_t strip = rd32(Data, Size, 26, 0);
                tmsize_t cc = (Size > 0) ? (tmsize_t)(rd32(Data, Size, 30, 0) % (uint32_t)(scanline + 64)) : 0;
                if (cc > scanline) cc = scanline;
                (void)TIFFWriteEncodedStrip(tifw, strip, buf, cc);

                _TIFFfree(buf);
            }
        }

        {
            int err = 0;
            (void)TIFFGetStrileByteCountWithErr(tifw, rd32(Data, Size, 34, 0), &err);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        tmsize_t scanline = TIFFScanlineSize(tifr);
        if (scanline > 0 && scanline < (1 << 20)) {
            void *buf = _TIFFmalloc((tmsize_t)scanline);
            if (buf) {
                uint32_t row = rd32(Data, Size, 38, 0);
                uint16_t sample = rd16(Data, Size, 42, 0);
                (void)TIFFReadScanline(tifr, buf, row, sample);
                _TIFFfree(buf);
            }
        }

        {
            int err = 0;
            (void)TIFFGetStrileByteCountWithErr(tifr, rd32(Data, Size, 44, 0), &err);
        }

        TIFFClose(tifr);
    }

    remove("./dummy_file");
    return 0;
}