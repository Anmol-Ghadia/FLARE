// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFRasterScanlineSize at tif_strip.c:337:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFTileRowSize at tif_tile.c:180:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
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
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static uint32_t clamp_u32(uint32_t v, uint32_t minv, uint32_t maxv) {
    if (v < minv) return minv;
    if (v > maxv) return maxv;
    return v;
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
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1, rowsperstrip = 1;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t fillorder = FILLORDER_MSB2LSB;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    int use_tiles = 0;

    if (Size >= 4) width = clamp_u32(rd32(Data) % 1024 + 1, 1, 1024);
    if (Size >= 8) height = clamp_u32(rd32(Data + 4) % 1024 + 1, 1, 1024);
    if (Size >= 10) spp = (uint16_t)(rd16(Data + 8) % 4 + 1);
    if (Size >= 12) {
        static const uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
        bps = bps_choices[rd16(Data + 10) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    }
    if (Size >= 14) {
        static const uint16_t comp_choices[] = {
            COMPRESSION_NONE,
            COMPRESSION_LZW,
            COMPRESSION_PACKBITS,
            COMPRESSION_DEFLATE
        };
        compression = comp_choices[rd16(Data + 12) % (sizeof(comp_choices) / sizeof(comp_choices[0]))];
    }
    if (Size >= 16) {
        static const uint16_t photo_choices[] = {
            PHOTOMETRIC_MINISBLACK,
            PHOTOMETRIC_RGB,
            PHOTOMETRIC_PALETTE
        };
        photo = photo_choices[rd16(Data + 14) % (sizeof(photo_choices) / sizeof(photo_choices[0]))];
    }
    if (photo != PHOTOMETRIC_RGB) {
        spp = 1;
    } else if (spp < 3) {
        spp = 3;
    }

    if (Size >= 18) planar = (rd16(Data + 16) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    if (Size >= 20) sampleformat = (rd16(Data + 18) & 1) ? SAMPLEFORMAT_INT : SAMPLEFORMAT_UINT;
    if (Size >= 22) fillorder = (rd16(Data + 20) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;
    if (Size >= 24) orientation = (uint16_t)((rd16(Data + 22) % 8) + 1);
    if (Size >= 28) rowsperstrip = rd32(Data + 24) % height + 1;
    if (Size >= 29) use_tiles = Data[28] & 1;

    (void)use_tiles; /* target function set does not require tile writing */

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    tmsize_t scanline = TIFFScanlineSize(tif);
    tmsize_t rasterscanline = TIFFRasterScanlineSize(tif);
    tmsize_t stripsize = TIFFStripSize(tif);
    tmsize_t vstripsize1 = TIFFVStripSize(tif, 1);
    tmsize_t vstripsizeN = TIFFVStripSize(tif, rowsperstrip);
    tmsize_t tilerow = TIFFTileRowSize(tif);

    (void)scanline;
    (void)rasterscanline;
    (void)stripsize;
    (void)vstripsize1;
    (void)vstripsizeN;
    (void)tilerow;

    if (scanline <= 0) {
        TIFFClose(tif);
        return 0;
    }

    size_t bufsize = (size_t)scanline;
    if (bufsize == 0) {
        TIFFClose(tif);
        return 0;
    }

    unsigned char *buf = (unsigned char *)_TIFFmalloc(bufsize);
    if (!buf) {
        TIFFClose(tif);
        return 0;
    }

    if (Size == 0) {
        memset(buf, 0, bufsize);
    } else {
        for (size_t i = 0; i < bufsize; ++i) {
            buf[i] = Data[i % Size];
        }
    }

    uint32_t strips_per_image = (rowsperstrip == 0) ? 0 : ((height + rowsperstrip - 1) / rowsperstrip);
    if (strips_per_image == 0) {
        strips_per_image = 1;
    }
    if (planar == PLANARCONFIG_SEPARATE) {
        if (spp > 0 && strips_per_image <= 0xFFFFFFFFu / spp) {
            strips_per_image *= spp;
        } else {
            strips_per_image = 1;
        }
    }

    uint32_t strip0 = 0;
    uint32_t strip1 = (strips_per_image > 1) ? 1 : 0;
    uint32_t strip_last = strips_per_image - 1;

    (void)TIFFWriteEncodedStrip(tif, strip0, buf, scanline);
    (void)TIFFWriteEncodedStrip(tif, strip1, buf, scanline);
    (void)TIFFWriteEncodedStrip(tif, strip_last, buf, scanline);

    _TIFFfree(buf);
    TIFFClose(tif);
    return 0;
}