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

    static const TIFFFieldArray custom_array = {
        (TIFFFieldArrayType)0, 0, 0, NULL
    };

    uint64_t custom_dir_off = 0;
    uint64_t exif_off = 0;
    uint32_t width = 0;
    uint32_t length = 0;
    char *datetime_str = NULL;

    if (tif1) {
        TIFFFreeDirectory(tif1);
        TIFFCreateCustomDirectory(tif1, &custom_array);

        if (Size >= 4) {
            width = ((uint32_t)Data[0] << 8) | Data[1];
            length = ((uint32_t)Data[2] << 8) | Data[3];
        } else {
            width = 1;
            length = 1;
        }
        if (width == 0) width = 1;
        if (length == 0) length = 1;

        TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length);

        TIFFWriteCustomDirectory(tif1, &custom_dir_off);

        TIFFSetDirectory(tif1, (uint16_t)(Size ? Data[0] : 0));

        TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, (uint32_t)(width ^ 1U));
        TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, (uint32_t)(length ^ 1U));

        TIFFClose(tif1);
        tif1 = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);

        TIFFGetField(tif2, TIFFTAG_EXIFIFD, &exif_off);
        TIFFReadEXIFDirectory(tif2, (toff_t)exif_off);
        TIFFGetField(tif2, TIFFTAG_DATETIME, &datetime_str);

        TIFFReadCustomDirectory(tif2, (toff_t)custom_dir_off, &custom_array);
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);

        TIFFClose(tif2);
        tif2 = NULL;
    }

    if (tif1) TIFFClose(tif1);
    if (tif2) TIFFClose(tif2);

    return 0;
}