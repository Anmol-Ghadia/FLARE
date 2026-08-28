// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
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
#ifndef TIFFTAG_SAMPLESPERPIXEL
#define TIFFTAG_SAMPLESPERPIXEL 277
#endif
#ifndef TIFFTAG_EXIFIFD
#define TIFFTAG_EXIFIFD 34665
#endif
#ifndef TIFFTAG_DATETIME
#define TIFFTAG_DATETIME 306
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL)
        return;
    if (Data != NULL && Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput_2(const uint8_t *Data, size_t Size) {
    TIFF *tif1 = NULL;
    TIFF *tif2 = NULL;
    uint64_t custom_dir_off = 0;
    uint64_t exif_off = 0;
    uint32_t width = 1;
    uint32_t length = 1;
    uint16_t dir_index = 0;
    uint16_t bits = 1;
    uint16_t spp = 1;
    char *datetime_str = NULL;

    if (Data == NULL)
        return 0;

    if (Size > 0) width = (uint32_t)Data[0] + 1;
    if (Size > 1) length = (uint32_t)Data[1] + 1;
    if (Size > 2) dir_index = (uint16_t)Data[2];
    if (Size > 3) bits = (uint16_t)((Data[3] % 32) + 1);
    if (Size > 4) spp = (uint16_t)((Data[4] % 8) + 1);
    if (Size > 12) {
        exif_off = ((uint64_t)Data[5] << 56) |
                   ((uint64_t)Data[6] << 48) |
                   ((uint64_t)Data[7] << 40) |
                   ((uint64_t)Data[8] << 32) |
                   ((uint64_t)Data[9] << 24) |
                   ((uint64_t)Data[10] << 16) |
                   ((uint64_t)Data[11] << 8) |
                   (uint64_t)Data[12];
    }

    write_dummy_file(Data, Size);

    tif1 = TIFFOpen("./dummy_file", "w+");
    if (tif1 != NULL) {
        TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length);
        TIFFSetField(tif1, TIFFTAG_BITSPERSAMPLE, bits);
        TIFFSetField(tif1, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFWriteDirectory(tif1);
        TIFFSetDirectory(tif1, dir_index);
        TIFFClose(tif1);
        tif1 = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2 != NULL) {
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);
        TIFFReadEXIFDirectory(tif2, (toff_t)exif_off);
        TIFFGetField(tif2, TIFFTAG_DATETIME, &datetime_str);
        if (custom_dir_off != 0)
            TIFFReadCustomDirectory(tif2, (toff_t)custom_dir_off, NULL);
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);
        TIFFClose(tif2);
        tif2 = NULL;
    }

    if (tif1 != NULL)
        TIFFClose(tif1);
    if (tif2 != NULL)
        TIFFClose(tif2);

    return 0;
}