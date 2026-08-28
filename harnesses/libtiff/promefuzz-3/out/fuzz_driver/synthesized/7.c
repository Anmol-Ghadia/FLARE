// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
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
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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

int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    const char *mode = (Size > 0 && (Data[0] & 1)) ? "w8" : "w";
    TIFF *tif = TIFFOpen("./dummy_file", mode);
    if (!tif) {
        return 0;
    }

    uint32_t width = rd32(Data, Size, 1, 1) % 4096 + 1;
    uint32_t height = rd32(Data, Size, 5, 1) % 4096 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 9, 1) % 4 + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 11, 0) % 6];
    uint16_t photo_choices[] = {
        PHOTOMETRIC_MINISWHITE,
        PHOTOMETRIC_MINISBLACK,
        PHOTOMETRIC_RGB,
        PHOTOMETRIC_PALETTE,
        PHOTOMETRIC_SEPARATED
    };
    uint16_t photo = photo_choices[rd16(Data, Size, 13, 0) % 5];
    uint16_t planar = (rd16(Data, Size, 15, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t orient = (uint16_t)((rd16(Data, Size, 17, 0) % 8) + 1);
    uint16_t compression_choices[] = {
        COMPRESSION_NONE,
        COMPRESSION_LZW,
        COMPRESSION_PACKBITS,
        COMPRESSION_ADOBE_DEFLATE
    };
    uint16_t compression = compression_choices[rd16(Data, Size, 19, 0) % 4];
    uint32_t rowsperstrip = rd32(Data, Size, 21, 1) % (height + 1);
    if (rowsperstrip == 0) rowsperstrip = 1;
    uint16_t fillorder = (rd16(Data, Size, 25, 0) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orient);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    if (Size > 26) {
        (void)TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    } else {
        (void)TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    }

    (void)TIFFDeferStrileArrayWriting(tif);
    (void)TIFFWriteCheck(tif, 0, "fuzz");
    (void)TIFFWriteDirectory(tif);
    TIFFFreeDirectory(tif);
    (void)TIFFCreateDirectory(tif);

    TIFFClose(tif);
    return 0;
}