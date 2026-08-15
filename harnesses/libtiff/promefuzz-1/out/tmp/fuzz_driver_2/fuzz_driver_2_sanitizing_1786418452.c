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
#ifndef TIFFTAG_EXIFIFD
#define TIFFTAG_EXIFIFD 34665
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

static void write_seed_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }

    if (Data != NULL && Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I', 42,0, 8,0,0,0, 0,0
        };
        (void)fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

static void write_minimal_valid_tiff(const uint8_t *Data, size_t Size) {
    uint32_t width = 1;
    uint32_t length = 1;
    uint16_t bps = 8;
    uint16_t spp = 1;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint32_t rowsperstrip = 1;
    uint8_t pixel = 0;

    if (Size > 0) pixel = Data[0];
    if (Size > 1) width = (uint32_t)(Data[1] ? Data[1] : 1);
    if (Size > 2) length = (uint32_t)(Data[2] ? Data[2] : 1);
    if (Size > 3) rowsperstrip = (uint32_t)(Data[3] ? Data[3] : 1);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
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
    uint32_t width1 = 1, length1 = 1, width2 = 2, length2 = 2;
    uint16_t dir_index = 0;
    uint64_t custom_dir_offset = 0;
    uint64_t exif_ifd_offset = 0;
    uint32_t got_width = 0, got_length = 0;
    TIFF *tif1 = NULL;
    TIFF *tif2 = NULL;

    write_seed_file(Data, Size);
    write_minimal_valid_tiff(Data, Size);

    if (Size > 0) width1 = (uint32_t)(Data[0] ? Data[0] : 1);
    if (Size > 1) length1 = (uint32_t)(Data[1] ? Data[1] : 1);
    if (Size > 2) width2 = (uint32_t)(Data[2] ? Data[2] : 2);
    if (Size > 3) length2 = (uint32_t)(Data[3] ? Data[3] : 2);
    if (Size > 4) dir_index = (uint16_t)Data[4];
    if (Size > 12) {
        exif_ifd_offset = ((uint64_t)Data[5]) |
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
        static TIFFField custom_fields[2];
        static TIFFFieldArray custom_field_array;

        memset(custom_fields, 0, sizeof(custom_fields));
        memset(&custom_field_array, 0, sizeof(custom_field_array));

        custom_fields[0].field_tag = TIFFTAG_IMAGEWIDTH;
        custom_fields[0].field_readcount = 1;
        custom_fields[0].field_writecount = 1;
        custom_fields[0].field_name = (char *)"ImageWidth";

        custom_fields[1].field_tag = TIFFTAG_IMAGELENGTH;
        custom_fields[1].field_readcount = 1;
        custom_fields[1].field_writecount = 1;
        custom_fields[1].field_name = (char *)"ImageLength";

        custom_field_array.count = 2;
        custom_field_array.fields = custom_fields;

        TIFFFreeDirectory(tif1);
        (void)TIFFCreateCustomDirectory(tif1, &custom_field_array);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width1);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length1);
        (void)TIFFWriteCustomDirectory(tif1, &custom_dir_offset);
        (void)TIFFSetDirectory(tif1, dir_index);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width2);
        (void)TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length2);
        TIFFClose(tif1);
        tif1 = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2 != NULL) {
        static TIFFField custom_fields2[2];
        static TIFFFieldArray custom_field_array2;

        memset(custom_fields2, 0, sizeof(custom_fields2));
        memset(&custom_field_array2, 0, sizeof(custom_field_array2));

        custom_fields2[0].field_tag = TIFFTAG_IMAGEWIDTH;
        custom_fields2[0].field_readcount = 1;
        custom_fields2[0].field_writecount = 1;
        custom_fields2[0].field_name = (char *)"ImageWidth";

        custom_fields2[1].field_tag = TIFFTAG_IMAGELENGTH;
        custom_fields2[1].field_readcount = 1;
        custom_fields2[1].field_writecount = 1;
        custom_fields2[1].field_name = (char *)"ImageLength";

        custom_field_array2.count = 2;
        custom_field_array2.fields = custom_fields2;

        (void)TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &got_width);
        (void)TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &got_length);
        (void)TIFFReadEXIFDirectory(tif2, (toff_t)exif_ifd_offset);
        (void)TIFFGetField(tif2, TIFFTAG_EXIFIFD, &exif_ifd_offset);
        (void)TIFFReadCustomDirectory(tif2, (toff_t)custom_dir_offset, &custom_field_array2);
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