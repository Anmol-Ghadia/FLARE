#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

#ifndef TIFFTAG_IMAGEWIDTH
#define TIFFTAG_IMAGEWIDTH 256
#endif
#ifndef TIFFTAG_IMAGELENGTH
#define TIFFTAG_IMAGELENGTH 257
#endif
#ifndef TIFFTAG_EXIFIFD
#define TIFFTAG_EXIFIFD 34665
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

static void write_minimal_tiff(const uint8_t *Data, size_t Size) {
    uint32_t width = 1;
    uint32_t length = 1;
    uint16_t bps = 8;
    uint16_t spp = 1;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint32_t rowsperstrip = 1;
    uint8_t pixel = 0;
    TIFF *tif;

    if (Size > 0) pixel = Data[0];
    if (Size > 1) width = (uint32_t)(Data[1] ? Data[1] : 1);
    if (Size > 2) length = (uint32_t)(Data[2] ? Data[2] : 1);
    if (Size > 3) rowsperstrip = (uint32_t)(Data[3] ? Data[3] : 1);

    tif = TIFFOpen("./dummy_file", "w");
    if (tif == NULL) {
        return;
    }

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    (void)TIFFWriteScanline(tif, &pixel, 0, 0);
    TIFFClose(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif1 = NULL;
    TIFF *tif2 = NULL;
    uint64_t custom_dir_offset = 0;
    uint64_t exif_offset = 0;
    uint32_t width_a = 1, length_a = 1;
    uint32_t width_b = 2, length_b = 2;
    uint32_t got_width = 0, got_length = 0;
    uint16_t dir_index = 0;

    write_minimal_tiff(Data, Size);

    if (Size > 0) width_a = (uint32_t)(Data[0] ? Data[0] : 1);
    if (Size > 1) length_a = (uint32_t)(Data[1] ? Data[1] : 1);
    if (Size > 2) width_b = (uint32_t)(Data[2] ? Data[2] : 2);
    if (Size > 3) length_b = (uint32_t)(Data[3] ? Data[3] : 2);
    if (Size > 4) dir_index = (uint16_t)Data[4];
    if (Size > 12) {
        exif_offset = ((uint64_t)Data[5]) |
                      ((uint64_t)Data[6] << 8) |
                      ((uint64_t)Data[7] << 16) |
                      ((uint64_t)Data[8] << 24) |
                      ((uint64_t)Data[9] << 32) |
                      ((uint64_t)Data[10] << 40) |
                      ((uint64_t)Data[11] << 48) |
                      ((uint64_t)Data[12] << 56);
    }

    tif1 = TIFFOpen("./dummy_file", "r+");
    if (tif1 != NULL) {
        TIFFFreeDirectory(tif1);
        /* Avoid passing NULL to TIFFCreateCustomDirectory(), which libtiff
           dereferences internally via _TIFFSetupFields(). */
        (void)TIFFSetDirectory(tif1, dir_index);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width_a);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length_a);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width_b);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length_b);
        TIFFClose(tif1);
        tif1 = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2 != NULL) {
        (void)TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &got_width);
        (void)TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &got_length);
        (void)TIFFReadEXIFDirectory(tif2, (toff_t)exif_offset);
        (void)TIFFGetField(tif2, TIFFTAG_EXIFIFD, &exif_offset);
        if (custom_dir_offset != 0) {
            (void)TIFFReadCustomDirectory(tif2, (toff_t)custom_dir_offset, NULL);
        }
        (void)TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &got_width);
        (void)TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &got_length);
        TIFFClose(tif2);
        tif2 = NULL;
    }

    if (tif1 != NULL) {
        TIFFClose(tif1);
    }
    if (tif2 != NULL) {
        TIFFClose(tif2);
    }

    return 0;
}