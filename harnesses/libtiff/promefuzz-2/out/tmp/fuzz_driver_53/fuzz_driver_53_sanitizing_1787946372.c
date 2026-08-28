#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tiffio.h"

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_VARIABLE2
#define TIFF_VARIABLE2 -3
#endif

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

static void noop_extender(TIFF *tif)
{
    (void)tif;
}

static char *dup_cstr(const char *s)
{
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    if (!p)
        return NULL;
    memcpy(p, s, n + 1);
    return p;
}

static void init_custom_field(TIFFField *field, uint32_t tag, TIFFDataType type,
                              short readcount, short writecount,
                              unsigned char passcount, const char *name)
{
    memset(field, 0, sizeof(*field));
    field->field_tag = tag;
    field->field_readcount = readcount;
    field->field_writecount = writecount;
    field->field_type = type;
    field->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field->field_bit = FIELD_CUSTOM;
    field->field_oktochange = 1;
    field->field_passcount = passcount;
    field->field_name = dup_cstr(name);
    field->field_subfields = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFFExtendProc old_ext = TIFFSetTagExtender(noop_extender);

    TIFF *tif_rw = TIFFOpen("./dummy_file", "w8");
    if (tif_rw)
    {
        TIFFField fields[3];
        TIFFFieldArray infoarray;
        uint64_t diroff = 0;
        uint16_t short_val = 0;
        uint32_t long_val = 0;
        char ascii_buf[32];
        size_t copy_n = Size < sizeof(ascii_buf) - 1 ? Size : sizeof(ascii_buf) - 1;

        memset(ascii_buf, 0, sizeof(ascii_buf));
        if (copy_n > 0)
            memcpy(ascii_buf, Data, copy_n);
        ascii_buf[copy_n] = '\0';

        init_custom_field(&fields[0], 65000, TIFF_SHORT, 1, 1, 0, "FuzzShort");
        init_custom_field(&fields[1], 65001, TIFF_LONG, 1, 1, 0, "FuzzLong");
        init_custom_field(&fields[2], 65002, TIFF_ASCII, TIFF_VARIABLE, TIFF_VARIABLE, 0, "FuzzAscii");

        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.type = (TIFFFieldArrayType)(Size ? (Data[0] % 4) : 0);
        infoarray.allocated_size = 0;
        infoarray.count = 3;
        infoarray.fields = fields;

        (void)TIFFCreateCustomDirectory(tif_rw, &infoarray);

        if (Size >= 2)
            short_val = (uint16_t)(Data[0] | ((uint16_t)Data[1] << 8));
        if (Size >= 6)
            long_val = (uint32_t)Data[2] |
                       ((uint32_t)Data[3] << 8) |
                       ((uint32_t)Data[4] << 16) |
                       ((uint32_t)Data[5] << 24);
        else if (Size > 0)
            long_val = (uint32_t)Size;

        if (Size % 3 == 0)
            TIFFSetField(tif_rw, 65000, short_val);
        if (Size % 3 == 1)
            TIFFSetField(tif_rw, 65001, long_val);
        if (Size % 3 == 2)
            TIFFSetField(tif_rw, 65002, ascii_buf);

        TIFFSetField(tif_rw, 65000, short_val);
        TIFFSetField(tif_rw, 65001, long_val);
        TIFFSetField(tif_rw, 65002, ascii_buf);

        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);

        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        free(fields[0].field_name);
        free(fields[1].field_name);
        free(fields[2].field_name);

        TIFFClose(tif_rw);
    }

    TIFF *tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r)
    {
        TIFFField fields2[2];
        TIFFFieldArray infoarray2;
        toff_t off1 = 0;
        toff_t off2 = (toff_t)(Size > 0 ? Data[0] : 1);
        toff_t off3 = (toff_t)(Size > 8 ? (*(const uint64_t *)Data) : Size);

        init_custom_field(&fields2[0], 65000, TIFF_SHORT, 1, 1, 0, "ReadShort");
        init_custom_field(&fields2[1], 65002, TIFF_ASCII, TIFF_VARIABLE, TIFF_VARIABLE, 0, "ReadAscii");

        memset(&infoarray2, 0, sizeof(infoarray2));
        infoarray2.type = (TIFFFieldArrayType)(Size > 1 ? (Data[1] % 4) : 0);
        infoarray2.allocated_size = 0;
        infoarray2.count = 2;
        infoarray2.fields = fields2;

        (void)TIFFReadCustomDirectory(tif_r, off1, &infoarray2);
        (void)TIFFReadCustomDirectory(tif_r, off2, &infoarray2);
        (void)TIFFReadCustomDirectory(tif_r, off3, &infoarray2);

        TIFFFreeDirectory(tif_r);

        free(fields2[0].field_name);
        free(fields2[1].field_name);

        TIFFClose(tif_r);
    }

    (void)TIFFSetTagExtender(old_ext);
    return 0;
}