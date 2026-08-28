#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tiffio.h"

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE (-1)
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

#ifndef TIFF_SETGET_UINT16
#define TIFF_SETGET_UINT16 2
#endif

#ifndef TIFF_SETGET_UINT32
#define TIFF_SETGET_UINT32 3
#endif

#ifndef TIFF_SETGET_ASCII
#define TIFF_SETGET_ASCII 7
#endif

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

#ifndef TIFF_ANY
#define TIFF_ANY 0
#endif

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r+");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w+");
    }
    if (!tif) {
        return 0;
    }

    char name1[] = "FuzzCustomShort";
    char name2[] = "FuzzCustomLong";
    char name3[] = "FuzzCustomAscii";

    TIFFFieldInfo compat_fields[3];
    memset(compat_fields, 0, sizeof(compat_fields));

    uint32_t base_tag = 65000U;
    if (Size >= 4) {
        base_tag = 65000U + (rd16(Data, Size, 0) % 100U);
    }

    compat_fields[0].field_tag = base_tag;
    compat_fields[0].field_readcount = 1;
    compat_fields[0].field_writecount = 1;
    compat_fields[0].field_type = TIFF_SHORT;
    compat_fields[0].field_bit = FIELD_CUSTOM;
    compat_fields[0].field_oktochange = 1;
    compat_fields[0].field_passcount = 0;
    compat_fields[0].field_name = name1;

    compat_fields[1].field_tag = base_tag + 1;
    compat_fields[1].field_readcount = 1;
    compat_fields[1].field_writecount = 1;
    compat_fields[1].field_type = TIFF_LONG;
    compat_fields[1].field_bit = FIELD_CUSTOM;
    compat_fields[1].field_oktochange = 1;
    compat_fields[1].field_passcount = 0;
    compat_fields[1].field_name = name2;

    compat_fields[2].field_tag = base_tag + 2;
    compat_fields[2].field_readcount = TIFF_VARIABLE;
    compat_fields[2].field_writecount = TIFF_VARIABLE;
    compat_fields[2].field_type = TIFF_ASCII;
    compat_fields[2].field_bit = FIELD_CUSTOM;
    compat_fields[2].field_oktochange = 1;
    compat_fields[2].field_passcount = 0;
    compat_fields[2].field_name = name3;

    (void)TIFFMergeFieldInfo(tif, compat_fields, 3);

    TIFFField custom_fields[3];
    memset(custom_fields, 0, sizeof(custom_fields));

    custom_fields[0].field_tag = base_tag + 10;
    custom_fields[0].field_readcount = 1;
    custom_fields[0].field_writecount = 1;
    custom_fields[0].field_type = TIFF_SHORT;
    custom_fields[0].set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT16;
    custom_fields[0].get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT16;
    custom_fields[0].field_bit = FIELD_CUSTOM;
    custom_fields[0].field_oktochange = 1;
    custom_fields[0].field_passcount = 0;
    custom_fields[0].field_name = name1;
    custom_fields[0].field_subfields = NULL;

    custom_fields[1].field_tag = base_tag + 11;
    custom_fields[1].field_readcount = 1;
    custom_fields[1].field_writecount = 1;
    custom_fields[1].field_type = TIFF_LONG;
    custom_fields[1].set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT32;
    custom_fields[1].get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT32;
    custom_fields[1].field_bit = FIELD_CUSTOM;
    custom_fields[1].field_oktochange = 1;
    custom_fields[1].field_passcount = 0;
    custom_fields[1].field_name = name2;
    custom_fields[1].field_subfields = NULL;

    custom_fields[2].field_tag = base_tag + 12;
    custom_fields[2].field_readcount = TIFF_VARIABLE;
    custom_fields[2].field_writecount = TIFF_VARIABLE;
    custom_fields[2].field_type = TIFF_ASCII;
    custom_fields[2].set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_ASCII;
    custom_fields[2].get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_ASCII;
    custom_fields[2].field_bit = FIELD_CUSTOM;
    custom_fields[2].field_oktochange = 1;
    custom_fields[2].field_passcount = 0;
    custom_fields[2].field_name = name3;
    custom_fields[2].field_subfields = NULL;

    TIFFFieldArray custom_array;
    memset(&custom_array, 0, sizeof(custom_array));
    custom_array.type = (TIFFFieldArrayType)TIFF_ANY;
    custom_array.allocated_size = 0;
    custom_array.count = 3;
    custom_array.fields = custom_fields;

    (void)TIFFCreateCustomDirectory(tif, &custom_array);

    (void)TIFFUnsetField(tif, base_tag);
    (void)TIFFUnsetField(tif, base_tag + 1);
    (void)TIFFUnsetField(tif, base_tag + 2);
    (void)TIFFUnsetField(tif, base_tag + 10);
    (void)TIFFUnsetField(tif, base_tag + 11);
    (void)TIFFUnsetField(tif, base_tag + 12);

    if (Size > 0) {
        uint32_t dyn_tag = base_tag + (Data[0] % 32U);
        (void)TIFFUnsetField(tif, dyn_tag);
    }

    for (int i = -2; i < 8; ++i) {
        (void)TIFFGetTagListEntry(tif, i);
    }

    uint16_t u16 = 0;
    uint32_t u32 = 0;
    char *str = NULL;

    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &u32);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &u32);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &u16);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &u16);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &u16);

    (void)TIFFGetFieldDefaulted(tif, base_tag, &u16);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 1, &u32);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 2, &str);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 10, &u16);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 11, &u32);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 12, &str);

    if (Size >= 8) {
        uint32_t extra_tag = rd32(Data, Size, 4);
        (void)TIFFUnsetField(tif, extra_tag);
        (void)TIFFGetFieldDefaulted(tif, extra_tag, &u32);
    }

    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    return 0;
}