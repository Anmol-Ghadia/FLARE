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

static void write_file(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (!f) return;
    if (Size > 0) (void)fwrite(Data, 1, Size, f);
    fclose(f);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_file(Data, Size);

    uint32_t width = rd32(Data, Size, 0, 1) % 128 + 1;
    uint32_t height = rd32(Data, Size, 4, 1) % 128 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 4 + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10, 0) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    uint16_t planar = (rd16(Data, Size, 12, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = rd32(Data, Size, 16, 0, 0);
    if (rowsperstrip == 0) rowsperstrip = (rd32(Data, Size, 16, 1) % height) + 1;

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        (void)TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, photometric);
        (void)TIFFSetField(tifw, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tifw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        uint32_t nstrips = TIFFNumberOfStrips(tifw);
        uint32_t ntile_before = TIFFNumberOfTiles(tifw);

        uint32_t row = height ? (rd32(Data, Size, 20, 0) % (height * 2U + 1U)) : 0;
        uint16_t sample = (uint16_t)(rd16(Data, Size, 24, 0) % (spp + 3));
        uint32_t strip = TIFFComputeStrip(tifw, row, sample);

        tmsize_t scanline = TIFFScanlineSize(tifw);
        tmsize_t stripbufsz = 0;
        if (scanline > 0) {
            uint32_t rps = rowsperstrip ? rowsperstrip : 1;
            stripbufsz = scanline * (tmsize_t)rps;
            if (stripbufsz <= 0 || stripbufsz > (tmsize_t)(1 << 20))
                stripbufsz = scanline;
        } else {
            stripbufsz = (tmsize_t)((Size % 4096) + 1);
        }

        void *buf = malloc((size_t)stripbufsz);
        if (buf) {
            if (Size > 0) {
                size_t copy = (size_t)stripbufsz < Size ? (size_t)stripbufsz : Size;
                memcpy(buf, Data, copy);
                if (copy < (size_t)stripbufsz)
                    memset((uint8_t *)buf + copy, 0, (size_t)stripbufsz - copy);
            } else {
                memset(buf, 0, (size_t)stripbufsz);
            }

            (void)TIFFWriteEncodedStrip(tifw, strip, buf, stripbufsz);
            if (nstrips > 0) {
                uint32_t altstrip = rd32(Data, Size, 28, 0) % (nstrips + 2);
                tmsize_t altcc = (tmsize_t)(Size % ((size_t)stripbufsz + 1));
                (void)TIFFWriteEncodedStrip(tifw, altstrip, buf, altcc);
            } else {
                (void)TIFFWriteEncodedStrip(tifw, 0, buf, stripbufsz);
            }

            free(buf);
        }

        uint32_t tilew = rd32(Data, Size, 32, 0) % 64 + 1;
        uint32_t tileh = rd32(Data, Size, 36, 0) % 64 + 1;
        if (Data && Size > 40 && (Data[40] & 1)) {
            (void)TIFFSetField(tifw, TIFFTAG_TILEWIDTH, tilew);
            (void)TIFFSetField(tifw, TIFFTAG_TILELENGTH, tileh);
            if (Data[40] & 2) {
                uint32_t tiled = rd32(Data, Size, 41, 1) % 16 + 1;
                (void)TIFFSetField(tifw, TIFFTAG_TILEDEPTH, tiled);
            }
            uint32_t ntile = TIFFNumberOfTiles(tifw);
            uint32_t x = rd32(Data, Size, 45, 0) % (width * 2U + 1U);
            uint32_t y = rd32(Data, Size, 49, 0) % (height * 2U + 1U);
            uint32_t z = rd32(Data, Size, 53, 0) % 32U;
            uint16_t s = (uint16_t)(rd16(Data, Size, 57, 0) % (spp + 3));
            (void)ntile_before;
            (void)ntile;
            (void)TIFFComputeTile(tifw, x, y, z, s);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        uint32_t nstrips_r = TIFFNumberOfStrips(tifr);
        uint32_t ntile_r = TIFFNumberOfTiles(tifr);

        uint32_t img_h = 1;
        uint16_t rspp = 1;
        (void)TIFFGetField(tifr, TIFFTAG_IMAGELENGTH, &img_h);
        (void)TIFFGetField(tifr, TIFFTAG_SAMPLESPERPIXEL, &rspp);

        uint32_t row_r = img_h ? (rd32(Data, Size, 60, 0) % (img_h * 2U + 1U)) : 0;
        uint16_t sample_r = (uint16_t)(rd16(Data, Size, 64, 0) % (rspp + 3));
        uint32_t strip_r = TIFFComputeStrip(tifr, row_r, sample_r);

        tmsize_t stripsize = TIFFStripSize(tifr);
        if (stripsize <= 0 || stripsize > (tmsize_t)(1 << 20))
            stripsize = (tmsize_t)((Size % 4096) + 1);

        void *rbuf = malloc((size_t)stripsize);
        if (rbuf) {
            if (nstrips_r > 0) {
                uint32_t s0 = strip_r % nstrips_r;
                (void)TIFFReadEncodedStrip(tifr, s0, rbuf, stripsize);

                uint32_t s1 = rd32(Data, Size, 68, 0) % (nstrips_r + 2);
                tmsize_t req = (tmsize_t)(Size % ((size_t)stripsize + 1));
                (void)TIFFReadEncodedStrip(tifr, s1, rbuf, req);
            } else {
                (void)TIFFReadEncodedStrip(tifr, 0, rbuf, stripsize);
            }
            free(rbuf);
        }

        uint32_t x = rd32(Data, Size, 72, 0);
        uint32_t y = rd32(Data, Size, 76, 0);
        uint32_t z = rd32(Data, Size, 80, 0);
        uint16_t s = (uint16_t)(rd16(Data, Size, 84, 0) % (rspp + 3));
        (void)ntile_r;
        (void)TIFFComputeTile(tifr, x, y, z, s);

        TIFFClose(tifr);
    }

    return 0;
}