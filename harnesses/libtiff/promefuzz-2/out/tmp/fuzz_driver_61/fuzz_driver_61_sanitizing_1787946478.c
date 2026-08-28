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
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I', 42,0, 8,0,0,0, 0,0
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

static void init_custom_field(TIFFField *field, uint32_t tag, TIFFDataType type, const char *name) {
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = 1;
    field->field_writecount = 1;
    field->field_type = type;
    field->reserved = 0;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->field_bit = FIELD_CUSTOM;
    field->field_oktochange = 1;
    field->field_passcount = 0;
    field->field_name = (char *)name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif_rw = TIFFOpen("./dummy_file", "w8");
    if (tif_rw) {
        TIFFField fields[2];
        TIFFFieldArray infoarray;
        uint64_t diroff = 0;

        init_custom_field(&fields[0], 65000, TIFF_LONG, "FuzzCustomLong");
        init_custom_field(&fields[1], 65001, TIFF_ASCII, "FuzzCustomAscii");

        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.type = (TIFFFieldArrayType)0;
        infoarray.allocated_size = 0;
        infoarray.count = 2;
        infoarray.fields = fields;

        (void)TIFFCreateCustomDirectory(tif_rw, &infoarray);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateGPSDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        if (Size >= 8) {
            uint64_t fuzz_off = 0;
            memcpy(&fuzz_off, Data, 8);
            (void)TIFFCreateCustomDirectory(tif_rw, &infoarray);
            (void)TIFFReadCustomDirectory(tif_rw, (toff_t)fuzz_off, &infoarray);
            TIFFFreeDirectory(tif_rw);
        }

        TIFFClose(tif_rw);
    }

    TIFF *tif_ro = TIFFOpen("./dummy_file", "r");
    if (tif_ro) {
        TIFFField fields[1];
        TIFFFieldArray infoarray;
        toff_t off = 0;

        init_custom_field(&fields[0], 65010, TIFF_UNDEFINED, "FuzzReadOnlyField");

        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.type = (TIFFFieldArrayType)0;
        infoarray.allocated_size = 0;
        infoarray.count = 1;
        infoarray.fields = fields;

        if (Size >= sizeof(toff_t)) {
            memcpy(&off, Data, sizeof(toff_t));
        }

        (void)TIFFReadCustomDirectory(tif_ro, off, &infoarray);
        TIFFFreeDirectory(tif_ro);

        (void)TIFFCreateGPSDirectory(tif_ro);
        TIFFFreeDirectory(tif_ro);

        (void)TIFFCreateEXIFDirectory(tif_ro);
        TIFFFreeDirectory(tif_ro);

        (void)TIFFCreateCustomDirectory(tif_ro, &infoarray);
        TIFFFreeDirectory(tif_ro);

        TIFFClose(tif_ro);
    }

    return 0;
}