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

#ifndef TIFF_NOTYPE
#define TIFF_NOTYPE 0
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

static void init_field(TIFFField *field, uint32_t tag, char *name) {
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = 1;
    field->field_writecount = 1;
    field->field_type = (TIFFDataType)TIFF_NOTYPE;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->field_bit = 0;
    field->field_oktochange = 1;
    field->field_passcount = 0;
    field->field_name = name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "w8");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (!tif) {
        return 0;
    }

    TIFFTagMethods *methods = TIFFAccessTagMethods(tif);
    (void)methods;

    (void)TIFFCreateDirectory(tif);
    (void)TIFFAccessTagMethods(tif);

    (void)TIFFCreateEXIFDirectory(tif);
    (void)TIFFAccessTagMethods(tif);

    TIFFField fields[2];
    char name1[] = "FuzzTagA";
    char name2[] = "FuzzTagB";
    init_field(&fields[0], 65000u, name1);
    init_field(&fields[1], 65001u, name2);

    TIFFFieldArray infoarray;
    memset(&infoarray, 0, sizeof(infoarray));
    infoarray.type = (TIFFFieldArrayType)TIFF_ANY;
    infoarray.allocated_size = 0;
    infoarray.count = (Size > 0) ? ((Data[0] & 1) ? 2u : 1u) : 2u;
    infoarray.fields = fields;

    (void)TIFFCreateCustomDirectory(tif, &infoarray);
    (void)TIFFAccessTagMethods(tif);

    if (Size > 1) {
        size_t iterations = (Data[1] % 8) + 1;
        for (size_t i = 0; i < iterations; ++i) {
            uint8_t op = Data[(2 + i) % Size];
            switch (op % 5) {
                case 0:
                    (void)TIFFCreateDirectory(tif);
                    break;
                case 1:
                    (void)TIFFCreateEXIFDirectory(tif);
                    break;
                case 2:
                    (void)TIFFCreateCustomDirectory(tif, &infoarray);
                    break;
                case 3:
                    (void)TIFFWriteDirectory(tif);
                    break;
                case 4:
                    TIFFFreeDirectory(tif);
                    break;
            }
            (void)TIFFAccessTagMethods(tif);
        }
    } else {
        (void)TIFFWriteDirectory(tif);
        TIFFFreeDirectory(tif);
    }

    TIFFClose(tif);
    return 0;
}