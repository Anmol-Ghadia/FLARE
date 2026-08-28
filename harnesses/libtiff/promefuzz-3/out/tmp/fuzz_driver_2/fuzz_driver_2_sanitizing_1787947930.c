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
#ifndef TIFFTAG_SAMPLESPERPIXEL
#define TIFFTAG_SAMPLESPERPIXEL 277
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

static TIFFFieldArray make_custom_array(void) {
    static TIFFField fields[2];
    static TIFFFieldArray array;
    static int initialized = 0;

    if (!initialized) {
        memset(fields, 0, sizeof(fields));
        memset(&array, 0, sizeof(array));

        fields[0].field_tag = TIFFTAG_IMAGEWIDTH;
        fields[1].field_tag = TIFFTAG_IMAGELENGTH;

        array.type = tfiatOther;
        array.allocated_size = 0;
        array.count = 2;
        array.fields = fields;

        initialized = 1;
    }

    return array;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif1 = NULL;
    TIFF *tif2 = NULL;
    TIFFFieldArray custom_array = make_custom_array();
    uint64_t custom_dir_off = 0;
    uint64_t exif_off = 0;
    uint32_t width = 1, length = 1;
    uint16_t dir_index = 0;
    uint16_t shortv = 1;
    char *datetime_str = NULL;

    if (Size < 8)
        return 0;

    width = ((uint32_t)Data[0] << 8) | (uint32_t)Data[1];
    length = ((uint32_t)Data[2] << 8) | (uint32_t)Data[3];
    dir_index = ((uint16_t)Data[4] << 8) | (uint16_t)Data[5];
    exif_off = ((uint64_t)Data[6] << 8) | (uint64_t)Data[7];

    if (width == 0) width = 1;
    if (length == 0) length = 1;
    if (Size > 8 && Data[8] != 0)
        shortv = Data[8];

    write_dummy_file(Data, Size);

    tif1 = TIFFOpen("./dummy_file", "w+");
    if (tif1) {
        TIFFFreeDirectory(tif1);
        TIFFCreateCustomDirectory(tif1, &custom_array);
        TIFFSetField(tif1, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif1, TIFFTAG_IMAGELENGTH, length);
        TIFFWriteCustomDirectory(tif1, &custom_dir_off);
        TIFFSetDirectory(tif1, dir_index);
        TIFFSetField(tif1, TIFFTAG_BITSPERSAMPLE, shortv);
        TIFFSetField(tif1, TIFFTAG_SAMPLESPERPIXEL, shortv);
        TIFFClose(tif1);
        tif1 = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &length);
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