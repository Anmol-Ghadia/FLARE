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

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

#ifndef TIFF_SETGET_UINT32
#define TIFF_SETGET_UINT32 4
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void init_custom_field(TIFFField *field, uint32_t tag, const char *name) {
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = 1;
    field->field_writecount = 1;
    field->field_type = TIFF_LONG;
    field->reserved = 0;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT32;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UINT32;
    field->field_bit = 65;
    field->field_oktochange = 1;
    field->field_passcount = 0;
    field->field_name = (char *)name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (tifw) {
        TIFFField fields[2];
        TIFFFieldArray infoarray;
        uint64_t diroff = 0;
        uint32_t v0 = 0, v1 = 0;

        init_custom_field(&fields[0], 65000U, "FuzzCustomTag0");
        init_custom_field(&fields[1], 65001U, "FuzzCustomTag1");

        infoarray.type = (TIFFFieldArrayType)(Size ? (Data[0] % 4) : 0);
        infoarray.allocated_size = 0;
        infoarray.count = 2;
        infoarray.fields = fields;

        (void)TIFFCreateCustomDirectory(tifw, &infoarray);

        if (Size >= 4) {
            v0 = ((uint32_t)Data[0] << 24) |
                 ((uint32_t)Data[1] << 16) |
                 ((uint32_t)Data[2] << 8) |
                 (uint32_t)Data[3];
            TIFFSetField(tifw, 65000U, v0);
        }
        if (Size >= 8) {
            v1 = ((uint32_t)Data[4] << 24) |
                 ((uint32_t)Data[5] << 16) |
                 ((uint32_t)Data[6] << 8) |
                 (uint32_t)Data[7];
            TIFFSetField(tifw, 65001U, v1);
        }

        (void)TIFFWriteCustomDirectory(tifw, &diroff);

        TIFFFreeDirectory(tifw);

        (void)TIFFCreateGPSDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateEXIFDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        TIFFField fields[2];
        TIFFFieldArray infoarray;
        toff_t off1 = 0, off2 = 0, off3 = 0;

        init_custom_field(&fields[0], 65000U, "FuzzCustomTag0");
        init_custom_field(&fields[1], 65001U, "FuzzCustomTag1");

        infoarray.type = (TIFFFieldArrayType)(Size > 1 ? (Data[1] % 4) : 0);
        infoarray.allocated_size = 0;
        infoarray.count = 2;
        infoarray.fields = fields;

        if (Size >= sizeof(toff_t)) {
            memcpy(&off1, Data, sizeof(toff_t));
        }
        if (Size >= 2 * sizeof(toff_t)) {
            memcpy(&off2, Data + sizeof(toff_t), sizeof(toff_t));
        } else {
            off2 = 8;
        }
        if (Size >= 3 * sizeof(toff_t)) {
            memcpy(&off3, Data + 2 * sizeof(toff_t), sizeof(toff_t));
        } else {
            off3 = (toff_t)0xFFFFFFFFu;
        }

        (void)TIFFReadCustomDirectory(tifr, off1, &infoarray);
        TIFFFreeDirectory(tifr);

        (void)TIFFCreateGPSDirectory(tifr);
        TIFFFreeDirectory(tifr);

        (void)TIFFCreateEXIFDirectory(tifr);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off2, &infoarray);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off3, &infoarray);
        TIFFFreeDirectory(tifr);

        TIFFClose(tifr);
    }

    return 0;
}