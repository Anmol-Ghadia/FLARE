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

#include <tiffio.h>

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

#ifndef TIFF_ANY
#define TIFF_ANY 0
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            0x49, 0x49, 0x2A, 0x00,
            0x08, 0x00, 0x00, 0x00,
            0x00, 0x00
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

static void init_custom_field(TIFFField *field, uint32_t tag, char *name) {
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = TIFF_VARIABLE;
    field->field_writecount = TIFF_VARIABLE;
    field->field_type = TIFF_ASCII;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->field_bit = TIFF_ANY;
    field->field_oktochange = 1;
    field->field_passcount = 1;
    field->field_name = name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r+");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w+");
    }
    if (!tif) {
        return 0;
    }

    TIFFField field_storage[2];
    char name1[] = "FuzzCustomTag1";
    char name2[] = "FuzzCustomTag2";
    init_custom_field(&field_storage[0], 65000U, name1);
    init_custom_field(&field_storage[1], 65001U, name2);

    TIFFFieldArray infoarray;
    memset(&infoarray, 0, sizeof(infoarray));
    infoarray.type = (TIFFFieldArrayType)(Size ? (Data[0] % 4) : 0);
    infoarray.allocated_size = (uint32_t)sizeof(field_storage);
    infoarray.count = (Size > 1 && (Data[1] & 1)) ? 2U : 1U;
    infoarray.fields = field_storage;

    (void)TIFFGetTagListCount(tif);

    if (Size > 2) {
        switch (Data[2] % 4) {
            case 0:
                (void)TIFFCreateEXIFDirectory(tif);
                break;
            case 1:
                (void)TIFFCreateCustomDirectory(tif, &infoarray);
                break;
            case 2:
                (void)TIFFCreateEXIFDirectory(tif);
                (void)TIFFCreateCustomDirectory(tif, &infoarray);
                break;
            default:
                (void)TIFFCreateCustomDirectory(tif, &infoarray);
                (void)TIFFCreateEXIFDirectory(tif);
                break;
        }
    } else {
        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFCreateCustomDirectory(tif, &infoarray);
    }

    (void)TIFFGetTagListCount(tif);

    toff_t off1 = 0;
    toff_t off2 = 8;
    toff_t off3 = (toff_t)Size;

    if (Size >= sizeof(toff_t)) {
        memcpy(&off3, Data, sizeof(toff_t));
    } else if (Size > 0) {
        for (size_t i = 0; i < Size; ++i) {
            off3 = (off3 << 8) ^ Data[i];
        }
    }

    (void)TIFFReadEXIFDirectory(tif, off1);
    (void)TIFFReadEXIFDirectory(tif, off2);
    (void)TIFFReadEXIFDirectory(tif, off3);

    (void)TIFFReadCustomDirectory(tif, off1, &infoarray);
    (void)TIFFReadCustomDirectory(tif, off2, &infoarray);
    (void)TIFFReadCustomDirectory(tif, off3, &infoarray);

    if (Size > 3 && (Data[3] & 1)) {
        TIFFFreeDirectory(tif);
        (void)TIFFGetTagListCount(tif);
        (void)TIFFCreateCustomDirectory(tif, &infoarray);
        (void)TIFFReadCustomDirectory(tif, off3, &infoarray);
    }

    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    return 0;
}