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
#ifndef TIFFTAG_EXIFIFD
#define TIFFTAG_EXIFIFD 34665
#endif
#ifndef TIFFTAG_DATETIME
#define TIFFTAG_DATETIME 306
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif1 = TIFFOpen("./dummy_file", "w+");
    TIFF *tif2 = NULL;

    uint64_t custom_dir_off = 0;
    uint64_t exif_off = 0;
    uint32_t width = 1;
    uint32_t length = 1;
    char *datetime_str = NULL;
    uint16_t dir_index = 0;

    if (Size >= 2) {
        width = ((uint32_t)Data[0] << 8) | (uint32_t)Data[1];
        if (width == 0) width = 1;
    }
    if (Size >= 4) {
        length = ((uint32_t)Data[2] << 8) | (uint32_t)Data[3];
        if (length == 0) length = 1;
    }
    if (Size >= 5) {
        dir_index = Data[4];
    }

    if (tif1) {
        TIFFFreeDirectory(tif1);
        TIFFCreateCustomDirectory(tif1, NULL);
        TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length);
        TIFFWriteCustomDirectory(tif1, &custom_dir_off);
        TIFFSetDirectory(tif1, dir_index);
        TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width ^ 1U);
        TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length ^ 1U);
        TIFFClose(tif1);
        tif1 = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);
        TIFFReadEXIFDirectory(tif2, (toff_t)exif_off);
        TIFFGetField(tif2, TIFFTAG_DATETIME, &datetime_str);
        TIFFReadCustomDirectory(tif2, (toff_t)custom_dir_off, NULL);
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);
        TIFFClose(tif2);
        tif2 = NULL;
    }

    if (tif1) TIFFClose(tif1);
    if (tif2) TIFFClose(tif2);

    return 0;
}