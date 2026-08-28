#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tiffio.h"

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void init_custom_field(TIFFField *field) {
    memset(field, 0, sizeof(*field));
    field->field_tag = 65000;
    field->field_readcount = TIFF_VARIABLE;
    field->field_writecount = TIFF_VARIABLE;
    field->field_type = TIFF_ASCII;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->field_bit = 65;
    field->field_oktochange = 1;
    field->field_passcount = 0;
    field->field_name = (char *)"FuzzCustomASCII";
    field->field_subfields = NULL;
}

static void init_custom_field_array(TIFFFieldArray *fa, TIFFField *field) {
    memset(fa, 0, sizeof(*fa));
    fa->type = tfiatOther;
    fa->allocated_size = 0;
    fa->count = 1;
    fa->fields = field;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    TIFF *tif_read = NULL;
    uint64_t custom_off = 0;
    TIFFField custom_field;
    TIFFFieldArray custom_array;
    char ascii_buf[64];
    size_t copy_len;
    toff_t read_off = 0;

    write_input_file(Data, Size);

    init_custom_field(&custom_field);
    init_custom_field_array(&custom_array, &custom_field);

    tif = TIFFOpen("./dummy_file", "w8");
    if (tif) {
        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteDirectory(tif);

        (void)TIFFCreateCustomDirectory(tif, &custom_array);

        copy_len = Size < (sizeof(ascii_buf) - 1) ? Size : (sizeof(ascii_buf) - 1);
        if (copy_len > 0) {
            memcpy(ascii_buf, Data, copy_len);
        }
        ascii_buf[copy_len] = '\0';

        if (copy_len > 0) {
            (void)TIFFSetField(tif, 65000, ascii_buf);
        } else {
            (void)TIFFSetField(tif, 65000, "A");
        }

        (void)TIFFWriteCustomDirectory(tif, &custom_off);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteDirectory(tif);

        TIFFClose(tif);
        tif = NULL;
    }

    tif_read = TIFFOpen("./dummy_file", "r");
    if (tif_read) {
        read_off = (toff_t)custom_off;
        (void)TIFFReadCustomDirectory(tif_read, read_off, &custom_array);
        TIFFFreeDirectory(tif_read);

        if (Size >= sizeof(uint64_t)) {
            uint64_t fuzz_off = 0;
            memcpy(&fuzz_off, Data, sizeof(uint64_t));
            (void)TIFFReadCustomDirectory(tif_read, (toff_t)fuzz_off, &custom_array);
            TIFFFreeDirectory(tif_read);
        }

        (void)TIFFReadCustomDirectory(tif_read, 0, &custom_array);
        TIFFFreeDirectory(tif_read);

        TIFFClose(tif_read);
        tif_read = NULL;
    }

    return 0;
}