// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p, size_t n) {
    if (n < 2) return 0;
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n) {
    if (n < 4) {
        uint32_t v = 0;
        size_t i;
        for (i = 0; i < n; i++) v |= ((uint32_t)p[i]) << (8 * i);
        return v;
    }
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const char *path = "./dummy_file";
    FILE *fp = fopen(path, "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen(path, "w");
    TIFF *tif_read_tile = NULL;
    TIFF *tif_read_strip = NULL;

    if (tif) {
        uint32_t width = (Size > 0) ? (rd32(Data, Size) % 64U) + 1U : 1U;
        uint32_t height = (Size > 4) ? (rd32(Data + (Size > 4 ? 1 : 0), Size > 4 ? Size - 1 : 0) % 64U) + 1U : 1U;
        uint16_t spp = (Size > 8) ? (uint16_t)((rd16(Data + 2, Size - 2) % 4U) + 1U) : 1U;
        uint16_t bps_choices[] = {1, 8, 16, 32};
        uint16_t bps = bps_choices[(Size > 10) ? (Data[10] % 4) : 1];
        uint16_t planar = (Size > 11 && (Data[11] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
        uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        uint16_t compression = COMPRESSION_NONE;
        uint16_t sampleformat = SAMPLEFORMAT_UINT;
        uint32_t rowsperstrip = (Size > 12) ? ((uint32_t)Data[12] % height) + 1U : height;
        uint32_t tilew = 16U;
        uint32_t tileh = 16U;

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
        TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);

        if (Size > 13 && (Data[13] & 1)) {
            TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
            TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
        } else {
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        }

        TIFFDeferStrileArrayWriting(tif);
        TIFFWriteCheck(tif, (Size > 13 && (Data[13] & 1)) ? 1 : 0, "fuzz");
        TIFFWriteDirectory(tif);

        TIFFSetDirectory(tif, 0);
        TIFFForceStrileArrayWriting(tif);

        TIFFSetDirectory(tif, 1);
        TIFFForceStrileArrayWriting(tif);

        TIFFSetDirectory(tif, 0);

        if (Size > 13 && (Data[13] & 1)) {
            uint32_t tile = 0;
            tmsize_t cc = (tmsize_t)Size;
            void *buf = (void *)(uintptr_t)Data;
            TIFFWriteEncodedTile(tif, tile, buf, cc);
        } else {
            uint32_t strip = 0;
            tmsize_t cc = (tmsize_t)Size;
            void *buf = (void *)(uintptr_t)Data;
            TIFFWriteEncodedStrip(tif, strip, buf, cc);
        }

        TIFFClose(tif);
        tif = NULL;
    }

    tif_read_tile = TIFFOpen(path, "r");
    if (tif_read_tile) {
        tmsize_t rsz = (Size > 0) ? (tmsize_t)Size : 1;
        unsigned char stackbuf[4096];
        tmsize_t use = rsz;
        if (use > (tmsize_t)sizeof(stackbuf)) use = (tmsize_t)sizeof(stackbuf);
        TIFFReadEncodedTile(tif_read_tile, 0, stackbuf, use);
        TIFFClose(tif_read_tile);
        tif_read_tile = NULL;
    }

    tif_read_strip = TIFFOpen(path, "r");
    if (tif_read_strip) {
        tmsize_t rsz = (Size > 0) ? (tmsize_t)Size : 1;
        unsigned char stackbuf[4096];
        tmsize_t use = rsz;
        if (use > (tmsize_t)sizeof(stackbuf)) use = (tmsize_t)sizeof(stackbuf);
        TIFFReadEncodedStrip(tif_read_strip, 0, stackbuf, use);
        TIFFClose(tif_read_strip);
        tif_read_strip = NULL;
    }

    if (tif) TIFFClose(tif);
    if (tif_read_tile) TIFFClose(tif_read_tile);
    if (tif_read_strip) TIFFClose(tif_read_strip);

    return 0;
}