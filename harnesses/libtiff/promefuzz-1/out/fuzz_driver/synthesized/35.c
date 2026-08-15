// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
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
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

int LLVMFuzzerTestOneInput_35(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    uint32_t width = (Size > 0) ? ((rd32(Data, Size, 0, 1) % 64U) + 1U) : 1U;
    uint32_t height = (Size > 4) ? ((rd32(Data, Size, 4, 1) % 64U) + 1U) : 1U;
    uint16_t spp = (Size > 8) ? (uint16_t)((rd16(Data, Size, 8, 1) % 4U) + 1U) : 1U;
    uint16_t bps_choices[] = {1, 2, 4, 8, 16};
    uint16_t bps = bps_choices[(Size > 10) ? (rd16(Data, Size, 10, 0) % 5U) : 3U];
    uint16_t planar = (Size > 12 && (Data[12] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint32_t rowsperstrip = (Size > 13) ? ((uint32_t)(Data[13] % height) + 1U) : height;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        (void)TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tifw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        (void)TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, photometric);
        (void)TIFFSetField(tifw, TIFFTAG_COMPRESSION, compression);

        uint32_t nstrips = TIFFNumberOfStrips(tifw);
        (void)TIFFStripSize(tifw);
        (void)TIFFVStripSize(tifw, rowsperstrip);
        if (height > 1) (void)TIFFVStripSize(tifw, height - 1);
        (void)TIFFVStripSize(tifw, height);

        if (nstrips > 0) {
            uint32_t strip = 0;
            if (Size > 14) strip = rd32(Data, Size, 14, 0) % nstrips;

            tmsize_t strip_sz = TIFFStripSize(tifw);
            tmsize_t vstrip_sz = TIFFVStripSize(tifw, rowsperstrip);
            tmsize_t raw_sz_before = TIFFRawStripSize(tifw, strip);

            tmsize_t alloc_sz = strip_sz;
            if (alloc_sz <= 0 || alloc_sz > (1 << 20)) {
                alloc_sz = vstrip_sz;
            }
            if (alloc_sz <= 0 || alloc_sz > (1 << 20)) {
                alloc_sz = (Size > 0 && Size < (1 << 20)) ? (tmsize_t)Size : 1;
            }

            void *buf = malloc((size_t)alloc_sz);
            if (buf) {
                if (Size > 0) {
                    size_t copy = (size_t)alloc_sz < Size ? (size_t)alloc_sz : Size;
                    memcpy(buf, Data, copy);
                    if (copy < (size_t)alloc_sz) {
                        memset((uint8_t *)buf + copy, 0, (size_t)alloc_sz - copy);
                    }
                } else {
                    memset(buf, 0, (size_t)alloc_sz);
                }

                tmsize_t cc1 = alloc_sz;
                if (Size > 18) {
                    uint8_t mode = Data[18] % 4;
                    if (mode == 0) cc1 = alloc_sz;
                    else if (mode == 1) cc1 = alloc_sz > 0 ? alloc_sz / 2 : 0;
                    else if (mode == 2) cc1 = alloc_sz > 1 ? alloc_sz - 1 : alloc_sz;
                    else cc1 = 0;
                }

                (void)TIFFWriteRawStrip(tifw, strip, buf, cc1);
                (void)TIFFRawStripSize(tifw, strip);

                if (nstrips > 1) {
                    uint32_t strip2 = (strip + 1) % nstrips;
                    tmsize_t cc2 = alloc_sz > 0 ? alloc_sz : 1;
                    (void)TIFFWriteRawStrip(tifw, strip2, buf, cc2);
                    (void)TIFFRawStripSize(tifw, strip2);
                }

                free(buf);
            }

            (void)raw_sz_before;
        }

        (void)TIFFWriteDirectory(tifw);
        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        uint32_t nstrips_r = TIFFNumberOfStrips(tifr);
        (void)TIFFStripSize(tifr);
        (void)TIFFVStripSize(tifr, 1);
        (void)TIFFVStripSize(tifr, rowsperstrip);

        if (nstrips_r > 0) {
            uint32_t strip = 0;
            if (Size > 22) strip = rd32(Data, Size, 22, 0) % nstrips_r;

            tmsize_t raw_sz = TIFFRawStripSize(tifr, strip);
            tmsize_t strip_sz = TIFFStripSize(tifr);
            tmsize_t alloc_sz = raw_sz;

            if (alloc_sz <= 0 || alloc_sz > (1 << 20)) {
                alloc_sz = strip_sz;
            }
            if (alloc_sz <= 0 || alloc_sz > (1 << 20)) {
                alloc_sz = (Size > 0 && Size < (1 << 20)) ? (tmsize_t)Size : 1;
            }

            void *rbuf = malloc((size_t)alloc_sz);
            if (rbuf) {
                (void)TIFFReadRawStrip(tifr, strip, rbuf, alloc_sz);
                (void)TIFFReadRawStrip(tifr, strip, rbuf, -1);
                if (alloc_sz > 1) {
                    (void)TIFFReadRawStrip(tifr, strip, rbuf, alloc_sz / 2);
                }
                free(rbuf);
            }
        }

        TIFFClose(tifr);
    }

    remove("./dummy_file");
    return 0;
}