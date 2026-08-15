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

#include <tiffio.h>

#ifndef TIFFTAG_IMAGEWIDTH
#define TIFFTAG_IMAGEWIDTH 256
#endif
#ifndef TIFFTAG_IMAGELENGTH
#define TIFFTAG_IMAGELENGTH 257
#endif
#ifndef TIFFTAG_BITSPERSAMPLE
#define TIFFTAG_BITSPERSAMPLE 258
#endif
#ifndef TIFFTAG_COMPRESSION
#define TIFFTAG_COMPRESSION 259
#endif
#ifndef TIFFTAG_PHOTOMETRIC
#define TIFFTAG_PHOTOMETRIC 262
#endif
#ifndef TIFFTAG_SAMPLESPERPIXEL
#define TIFFTAG_SAMPLESPERPIXEL 277
#endif
#ifndef TIFFTAG_ROWSPERSTRIP
#define TIFFTAG_ROWSPERSTRIP 278
#endif
#ifndef TIFFTAG_PLANARCONFIG
#define TIFFTAG_PLANARCONFIG 284
#endif

#ifndef COMPRESSION_NONE
#define COMPRESSION_NONE 1
#endif
#ifndef PHOTOMETRIC_MINISBLACK
#define PHOTOMETRIC_MINISBLACK 1
#endif
#ifndef PLANARCONFIG_CONTIG
#define PLANARCONFIG_CONTIG 1
#endif
#ifndef PLANARCONFIG_SEPARATE
#define PLANARCONFIG_SEPARATE 2
#endif

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    if (Size < 16)
        return 0;

    uint32_t width = (rd32(Data) % 64U) + 1U;
    uint32_t height = (rd32(Data + 4) % 64U) + 1U;
    uint16_t spp = (uint16_t)((Data[8] % 4U) + 1U);
    uint16_t bps_choice = (uint16_t)(Data[9] % 4U);
    uint16_t bps = (bps_choice == 0) ? 1 : (bps_choice == 1) ? 8 : (bps_choice == 2) ? 16 : 32;
    uint16_t planar = (Data[10] & 1U) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint32_t rowsperstrip = (rd32(Data + 11) % height) + 1U;
    uint32_t extra_row = rd32(Data + 7) % (height + 4U);
    uint16_t sample_sel = (uint16_t)(Data[15] % (spp ? spp : 1));

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tifw, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
        TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(tifw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        (void)TIFFDeferStrileArrayWriting(tifw);

        tsize_t scanline = TIFFScanlineSize(tifw);
        if (scanline > 0 && scanline < (1 << 20)) {
            unsigned char *buf = (unsigned char *)malloc((size_t)scanline);
            if (buf) {
                for (uint32_t row = 0; row < height; row++) {
                    for (tsize_t i = 0; i < scanline; i++) {
                        buf[i] = Data[(size_t)((row + i) % Size)];
                    }
                    if (planar == PLANARCONFIG_SEPARATE) {
                        for (uint16_t s = 0; s < spp; s++) {
                            (void)TIFFWriteScanline(tifw, buf, row, s);
                        }
                    } else {
                        (void)TIFFWriteScanline(tifw, buf, row, 0);
                    }

                    if ((row % 3U) == 0U) {
                        (void)TIFFCheckpointDirectory(tifw);
                    }
                }

                (void)TIFFWriteScanline(tifw, buf, extra_row, sample_sel);

                free(buf);
            }
        }

        (void)TIFFWriteDirectory(tifw);
        (void)TIFFForceStrileArrayWriting(tifw);

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        tsize_t rscanline = TIFFScanlineSize(tifr);
        if (rscanline <= 0 || rscanline > (1 << 20))
            rscanline = 1;
        unsigned char *rbuf = (unsigned char *)malloc((size_t)rscanline);
        if (rbuf) {
            memset(rbuf, 0, (size_t)rscanline);

            uint32_t read_rows[4];
            read_rows[0] = 0;
            read_rows[1] = height ? (height - 1U) : 0U;
            read_rows[2] = extra_row;
            read_rows[3] = rd32(Data) % (height + 8U);

            for (size_t i = 0; i < 4; i++) {
                (void)TIFFReadScanline(tifr, rbuf, read_rows[i], 0);
                if (spp > 1) {
                    (void)TIFFReadScanline(tifr, rbuf, read_rows[i], sample_sel);
                }
            }

            free(rbuf);
        }
        TIFFClose(tifr);
    }

    return 0;
}