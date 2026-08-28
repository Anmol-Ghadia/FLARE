#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint64_t read_u64_le(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    size_t i;
    size_t lim = n < 8 ? n : 8;
    for (i = 0; i < lim; i++) {
        v |= ((uint64_t)p[i]) << (8 * i);
    }
    return v;
}

static void init_field(TIFFField *field, uint32_t tag, char *name) {
    field->field_tag = tag;
    field->field_readcount = 1;
    field->field_writecount = 1;
    field->field_type = TIFF_LONG;
    field->reserved = 0;
    field->set_field_type = TIFF_SETGET_UINT32;
    field->get_field_type = TIFF_SETGET_UINT32;
    field->field_bit = 65;
    field->field_oktochange = 1;
    field->field_passcount = 0;
    field->field_name = name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    uint64_t diroff = 0;
    uint64_t out_diroff = 0;
    int mode_selector = 0;
    const char *open_mode = "r";
    TIFFTagMethods *tag_methods;

    TIFFField custom_fields[2];
    TIFFFieldArray custom_array;

    write_input_file(Data, Size);

    if (Size > 0) {
        mode_selector = Data[0] % 4;
    }

    switch (mode_selector) {
        case 0:
            open_mode = "r";
            break;
        case 1:
            open_mode = "rM";
            break;
        case 2:
            open_mode = "w";
            break;
        default:
            open_mode = "w8";
            break;
    }

    tif = TIFFOpen("./dummy_file", open_mode);
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r+");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w8");
    }
    if (!tif) {
        return 0;
    }

    init_field(&custom_fields[0], 65000, (char *)"FuzzTagA");
    init_field(&custom_fields[1], 65001, (char *)"FuzzTagB");

    custom_array.type = (TIFFFieldArrayType)0;
    custom_array.allocated_size = 0;
    custom_array.count = 2;
    custom_array.fields = custom_fields;

    tag_methods = TIFFAccessTagMethods(tif);
    (void)tag_methods;

    if (Size > 1) {
        diroff = read_u64_le(Data + 1, Size - 1);
    }

    (void)TIFFReadEXIFDirectory(tif, (toff_t)diroff);
    (void)TIFFReadCustomDirectory(tif, (toff_t)diroff, &custom_array);
    TIFFFreeDirectory(tif);

    if (mode_selector >= 2) {
        (void)TIFFWriteCustomDirectory(tif, &out_diroff);
        (void)TIFFReadEXIFDirectory(tif, (toff_t)out_diroff);
        (void)TIFFReadCustomDirectory(tif, (toff_t)out_diroff, &custom_array);
        TIFFFreeDirectory(tif);
    }

    if (Size > 16) {
        uint64_t diroff2 = read_u64_le(Data + (Size - 8), 8);
        (void)TIFFReadEXIFDirectory(tif, (toff_t)diroff2);
        (void)TIFFReadCustomDirectory(tif, (toff_t)diroff2, &custom_array);
        TIFFFreeDirectory(tif);
    }

    TIFFCleanup(tif);
    return 0;
}