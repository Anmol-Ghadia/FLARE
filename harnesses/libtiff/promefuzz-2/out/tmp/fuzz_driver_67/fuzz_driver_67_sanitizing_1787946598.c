#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

#ifndef TIFF_BYTE
#define TIFF_BYTE 1
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void init_custom_field(TIFFField *field, uint32_t tag, char *name)
{
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = TIFF_VARIABLE;
    field->field_writecount = TIFF_VARIABLE;
    field->field_type = (TIFFDataType)TIFF_BYTE;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->field_bit = 0;
    field->field_oktochange = 1;
    field->field_passcount = 1;
    field->field_name = name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    write_dummy_file(Data, Size);

    TIFFField custom_fields[2];
    TIFFFieldArray custom_array;
    uint64_t written_off1 = 0;
    uint64_t written_off2 = 0;
    toff_t read_off1 = 0;
    toff_t read_off2 = 0;
    toff_t read_off3 = 8;

    init_custom_field(&custom_fields[0], 65000U, (char *)"FuzzCustomTag0");
    init_custom_field(&custom_fields[1], 65001U, (char *)"FuzzCustomTag1");

    memset(&custom_array, 0, sizeof(custom_array));
    custom_array.type = (TIFFFieldArrayType)0;
    custom_array.allocated_size = 0;
    custom_array.count = 2;
    custom_array.fields = custom_fields;

    if (Size >= sizeof(toff_t))
        memcpy(&read_off1, Data, sizeof(toff_t));
    if (Size >= 2 * sizeof(toff_t))
        memcpy(&read_off2, Data + sizeof(toff_t), sizeof(toff_t));
    if (Size >= 3 * sizeof(toff_t))
        memcpy(&read_off3, Data + 2 * sizeof(toff_t), sizeof(toff_t));

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (tifw)
    {
        (void)TIFFCreateCustomDirectory(tifw, &custom_array);
        (void)TIFFWriteCustomDirectory(tifw, &written_off1);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateGPSDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &written_off2);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateEXIFDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &written_off2);
        TIFFFreeDirectory(tifw);

        if (Size > 0)
        {
            if ((Data[0] & 1) == 0)
                (void)TIFFCreateCustomDirectory(tifw, &custom_array);
            else if ((Data[0] & 2) == 0)
                (void)TIFFCreateGPSDirectory(tifw);
            else
                (void)TIFFCreateEXIFDirectory(tifw);

            (void)TIFFWriteCustomDirectory(tifw, &written_off2);
            TIFFFreeDirectory(tifw);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr)
    {
        (void)TIFFReadCustomDirectory(tifr, read_off1, &custom_array);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, (toff_t)written_off1, &custom_array);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, read_off2, &custom_array);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, read_off3, &custom_array);
        TIFFFreeDirectory(tifr);

        if (written_off2 != 0)
        {
            (void)TIFFReadCustomDirectory(tifr, (toff_t)written_off2, &custom_array);
            TIFFFreeDirectory(tifr);
        }

        if (Size > 0)
        {
            if ((Data[0] & 1) == 0)
                (void)TIFFCreateCustomDirectory(tifr, &custom_array);
            else if ((Data[0] & 2) == 0)
                (void)TIFFCreateGPSDirectory(tifr);
            else
                (void)TIFFCreateEXIFDirectory(tifr);

            TIFFFreeDirectory(tifr);
        }

        TIFFClose(tifr);
    }

    return 0;
}