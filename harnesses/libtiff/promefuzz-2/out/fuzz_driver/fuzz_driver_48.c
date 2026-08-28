// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFieldWithTag at tif_dirinfo.c:732:1 in tiffio.h
// TIFFFieldDataType at tif_dirinfo.c:768:1 in tiffio.h
// TIFFFieldReadCount at tif_dirinfo.c:780:1 in tiffio.h
// TIFFFieldWriteCount at tif_dirinfo.c:786:1 in tiffio.h
// TIFFFieldPassCount at tif_dirinfo.c:774:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void exercise_field_accessors(TIFF *tif, uint32_t tag) {
    const TIFFField *field = TIFFFieldWithTag(tif, tag);
    if (field) {
        (void)TIFFFieldDataType(field);
        (void)TIFFFieldReadCount(field);
        (void)TIFFFieldWriteCount(field);
        (void)TIFFFieldPassCount(field);
    }
}

static void exercise_getters(TIFF *tif, uint32_t selector) {
    uint32_t u32 = 0;
    uint16_t u16 = 0;
    float f = 0.0f;
    char *str = NULL;
    uint16_t *u16p1 = NULL, *u16p2 = NULL, *u16p3 = NULL;

    switch (selector % 12) {
        case 0:
            (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &u32);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &u32);
            exercise_field_accessors(tif, TIFFTAG_IMAGEWIDTH);
            break;
        case 1:
            (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &u32);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &u32);
            exercise_field_accessors(tif, TIFFTAG_IMAGELENGTH);
            break;
        case 2:
            (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &u16);
            exercise_field_accessors(tif, TIFFTAG_BITSPERSAMPLE);
            break;
        case 3:
            (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
            exercise_field_accessors(tif, TIFFTAG_SAMPLESPERPIXEL);
            break;
        case 4:
            (void)TIFFGetField(tif, TIFFTAG_COMPRESSION, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &u16);
            exercise_field_accessors(tif, TIFFTAG_COMPRESSION);
            break;
        case 5:
            (void)TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &u16);
            exercise_field_accessors(tif, TIFFTAG_PHOTOMETRIC);
            break;
        case 6:
            (void)TIFFGetField(tif, TIFFTAG_ORIENTATION, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &u16);
            exercise_field_accessors(tif, TIFFTAG_ORIENTATION);
            break;
        case 7:
            (void)TIFFGetField(tif, TIFFTAG_XRESOLUTION, &f);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &f);
            exercise_field_accessors(tif, TIFFTAG_XRESOLUTION);
            break;
        case 8:
            (void)TIFFGetField(tif, TIFFTAG_YRESOLUTION, &f);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &f);
            exercise_field_accessors(tif, TIFFTAG_YRESOLUTION);
            break;
        case 9:
            (void)TIFFGetField(tif, TIFFTAG_SOFTWARE, &str);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SOFTWARE, &str);
            exercise_field_accessors(tif, TIFFTAG_SOFTWARE);
            break;
        case 10:
            (void)TIFFGetField(tif, TIFFTAG_COLORMAP, &u16p1, &u16p2, &u16p3);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COLORMAP, &u16p1, &u16p2, &u16p3);
            exercise_field_accessors(tif, TIFFTAG_COLORMAP);
            break;
        case 11:
            (void)TIFFGetField(tif, TIFFTAG_TRANSFERFUNCTION, &u16p1, &u16p2, &u16p3);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_TRANSFERFUNCTION, &u16p1, &u16p2, &u16p3);
            exercise_field_accessors(tif, TIFFTAG_TRANSFERFUNCTION);
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    if (Size == 0) {
        exercise_getters(tif, 0);
        TIFFClose(tif);
        return 0;
    }

    size_t iterations = Size < 64 ? Size : 64;
    for (size_t i = 0; i < iterations; ++i) {
        exercise_getters(tif, Data[i]);

        if ((Data[i] & 1) == 0) {
            (void)TIFFReadDirectory(tif);
        } else {
            (void)TIFFSetDirectory(tif, (tdir_t)(Data[i] % 8));
        }

        if ((Data[i] & 2) != 0) {
            uint32_t arbitrary_tag = ((uint32_t)Data[i] << 8) | (uint32_t)(i & 0xFF);
            exercise_field_accessors(tif, arbitrary_tag);
        }
    }

    TIFFClose(tif);
    return 0;
}