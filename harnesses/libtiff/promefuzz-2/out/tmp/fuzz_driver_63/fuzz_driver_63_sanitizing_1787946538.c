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

#ifndef TIFF_SETGET_ASCII
#define TIFF_SETGET_ASCII 1
#endif

#ifndef TIFF_SETGET_UINT8
#define TIFF_SETGET_UINT8 2
#endif

#ifndef TIFF_SETGET_UINT16
#define TIFF_SETGET_UINT16 3
#endif

#ifndef TIFF_SETGET_UINT32
#define TIFF_SETGET_UINT32 4
#endif

#ifndef TIFF_SETGET_UINT64
#define TIFF_SETGET_UINT64 5
#endif

#ifndef TIFF_SETGET_FLOAT
#define TIFF_SETGET_FLOAT 9
#endif

#ifndef TIFF_SETGET_DOUBLE
#define TIFF_SETGET_DOUBLE 10
#endif

#ifndef TIFF_SETGET_C0_UINT8
#define TIFF_SETGET_C0_UINT8 16
#endif

#ifndef TIFF_SETGET_C0_ASCII
#define TIFF_SETGET_C0_ASCII 17
#endif

#ifndef TIFF_SETGET_C16_UINT8
#define TIFF_SETGET_C16_UINT8 18
#endif

#ifndef TIFF_SETGET_C16_ASCII
#define TIFF_SETGET_C16_ASCII 19
#endif

#ifndef TIFF_SETGET_C32_UINT8
#define TIFF_SETGET_C32_UINT8 20
#endif

#ifndef TIFF_SETGET_C32_ASCII
#define TIFF_SETGET_C32_ASCII 21
#endif

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

#ifndef TIFF_ANY
#define TIFF_ANY 0
#endif

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

static uint64_t rd64(const uint8_t *p, size_t n, size_t off) {
    uint64_t lo = rd32(p, n, off);
    uint64_t hi = rd32(p, n, off + 4);
    return lo | (hi << 32);
}

static void fill_field(TIFFField *f, const uint8_t *Data, size_t Size, size_t base, uint32_t tag) {
    memset(f, 0, sizeof(*f));
    f->field_tag = tag;

    switch (Size ? Data[base % Size] % 6 : 0) {
        case 0:
            f->field_readcount = 1;
            f->field_writecount = 1;
            f->field_type = TIFF_BYTE;
            f->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT8;
            f->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT8;
            break;
        case 1:
            f->field_readcount = 1;
            f->field_writecount = 1;
            f->field_type = TIFF_SHORT;
            f->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT16;
            f->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT16;
            break;
        case 2:
            f->field_readcount = 1;
            f->field_writecount = 1;
            f->field_type = TIFF_LONG;
            f->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT32;
            f->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT32;
            break;
        case 3:
            f->field_readcount = 1;
            f->field_writecount = 1;
            f->field_type = TIFF_LONG8;
            f->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT64;
            f->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT64;
            break;
        case 4:
            f->field_readcount = 1;
            f->field_writecount = 1;
            f->field_type = TIFF_ASCII;
            f->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_ASCII;
            f->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_ASCII;
            break;
        default:
            f->field_readcount = TIFF_VARIABLE;
            f->field_writecount = TIFF_VARIABLE;
            f->field_type = TIFF_UNDEFINED;
            f->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_C32_UINT8;
            f->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_C32_UINT8;
            f->field_passcount = 1;
            break;
    }

    f->reserved = 0;
    f->field_bit = FIELD_CUSTOM;
    f->field_oktochange = 1;
    f->field_name = (char *)"FuzzCustomField";
    f->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (tifw) {
        TIFFField fields[3];
        TIFFFieldArray infoarray;
        uint64_t diroff = 0;
        uint32_t count = (Size > 0) ? ((uint32_t)(Data[0] % 3) + 1U) : 1U;
        uint32_t i;

        memset(fields, 0, sizeof(fields));
        for (i = 0; i < count; i++) {
            fill_field(&fields[i], Data, Size, 1 + i * 7, 65000U + i);
        }

        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.type = (TIFFFieldArrayType)((Size > 1) ? (Data[1] % 4) : 0);
        infoarray.allocated_size = 0;
        infoarray.count = count;
        infoarray.fields = fields;

        (void)TIFFCreateCustomDirectory(tifw, &infoarray);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateGPSDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateEXIFDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        if ((Size > 2) && (Data[2] & 1)) {
            (void)TIFFCreateCustomDirectory(tifw, &infoarray);
            (void)TIFFWriteCustomDirectory(tifw, &diroff);
            TIFFFreeDirectory(tifw);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        TIFFField fields[3];
        TIFFFieldArray infoarray;
        uint32_t count = (Size > 3) ? ((uint32_t)(Data[3] % 3) + 1U) : 1U;
        uint32_t i;
        toff_t off1 = (toff_t)rd64(Data, Size, 8);
        toff_t off2 = (toff_t)rd32(Data, Size, 16);
        toff_t off3 = (toff_t)0;

        memset(fields, 0, sizeof(fields));
        for (i = 0; i < count; i++) {
            fill_field(&fields[i], Data, Size, 24 + i * 5, 65100U + i);
        }

        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.type = (TIFFFieldArrayType)((Size > 4) ? (Data[4] % 4) : 0);
        infoarray.allocated_size = 0;
        infoarray.count = count;
        infoarray.fields = fields;

        (void)TIFFReadCustomDirectory(tifr, off1, &infoarray);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off2, &infoarray);
        TIFFFreeDirectory(tifr);

        (void)TIFFCreateGPSDirectory(tifr);
        TIFFFreeDirectory(tifr);

        (void)TIFFCreateEXIFDirectory(tifr);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off3, &infoarray);
        TIFFFreeDirectory(tifr);

        TIFFClose(tifr);
    }

    remove("./dummy_file");
    return 0;
}