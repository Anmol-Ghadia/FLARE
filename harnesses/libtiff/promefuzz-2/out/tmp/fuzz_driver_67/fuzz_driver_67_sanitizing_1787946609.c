#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void init_custom_field(TIFFField *field, char *name, uint32_t tag)
{
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = 1;
    field->field_writecount = 1;
    field->field_type = TIFF_LONG;
    field->set_field_type = TIFF_SETGET_UINT32;
    field->get_field_type = TIFF_SETGET_UINT32;
    field->field_bit = FIELD_CUSTOM;
    field->field_oktochange = 1;
    field->field_passcount = 0;
    field->field_name = name;
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    write_dummy_file(Data, Size);

    toff_t fuzz_off1 = 0;
    toff_t fuzz_off2 = 0;
    toff_t fuzz_off3 = 8;
    uint64_t written_off1 = 0;
    uint64_t written_off2 = 0;

    if (Size >= sizeof(toff_t))
        memcpy(&fuzz_off1, Data, sizeof(toff_t));
    if (Size >= 2 * sizeof(toff_t))
        memcpy(&fuzz_off2, Data + sizeof(toff_t), sizeof(toff_t));
    if (Size >= 3 * sizeof(toff_t))
        memcpy(&fuzz_off3, Data + 2 * sizeof(toff_t), sizeof(toff_t));

    TIFFField custom_fields[2];
    init_custom_field(&custom_fields[0], (char *)"FuzzTagA", 65000U);
    init_custom_field(&custom_fields[1], (char *)"FuzzTagB", 65001U);

    TIFFFieldArray custom_info;
    memset(&custom_info, 0, sizeof(custom_info));
    custom_info.type = TIFFFIELD_CUSTOM;
    custom_info.allocated_size = 0;
    custom_info.count = 2;
    custom_info.fields = custom_fields;

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (tifw)
    {
        (void)TIFFCreateCustomDirectory(tifw, &custom_info);
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
                (void)TIFFCreateCustomDirectory(tifw, &custom_info);
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
        (void)TIFFReadCustomDirectory(tifr, fuzz_off1, &custom_info);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, fuzz_off2, &custom_info);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, fuzz_off3, &custom_info);
        TIFFFreeDirectory(tifr);

        if (written_off1 != 0)
        {
            (void)TIFFReadCustomDirectory(tifr, (toff_t)written_off1, &custom_info);
            TIFFFreeDirectory(tifr);
        }

        if (written_off2 != 0)
        {
            (void)TIFFReadCustomDirectory(tifr, (toff_t)written_off2, &custom_info);
            TIFFFreeDirectory(tifr);
        }

        if (Size > 0)
        {
            if ((Data[0] & 1) == 0)
                (void)TIFFCreateCustomDirectory(tifr, &custom_info);
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