#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "tiffio.h"

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_ASCII
#define TIFF_ASCII 2
#endif

static int call_TIFFVSetField_ascii(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVSetField(tif, tag, ap);
    va_end(ap);
    return ret;
}

static void write_input_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    TIFF *tif;
    uint16_t dirn = 0;
    uint32_t custom_tag;
    uint32_t tag_from_list;
    int i;

    write_input_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r+");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "w+");
    if (!tif)
        return 0;

    custom_tag = 65000u + (uint32_t)(Size ? (Data[0] % 32) : 0);

    {
        TIFFFieldInfo finfo[2];
        memset(finfo, 0, sizeof(finfo));

        finfo[0].field_tag = custom_tag;
        finfo[0].field_readcount = TIFF_VARIABLE;
        finfo[0].field_writecount = TIFF_VARIABLE;
        finfo[0].field_type = TIFF_ASCII;
        finfo[0].field_bit = 0;
        finfo[0].field_oktochange = 1;
        finfo[0].field_passcount = 0;
        finfo[0].field_name = (char *)"FuzzCustomASCII";

        finfo[1].field_tag = custom_tag + 1;
        finfo[1].field_readcount = TIFF_VARIABLE;
        finfo[1].field_writecount = TIFF_VARIABLE;
        finfo[1].field_type = TIFF_ASCII;
        finfo[1].field_bit = 0;
        finfo[1].field_oktochange = 1;
        finfo[1].field_passcount = 0;
        finfo[1].field_name = (char *)"FuzzCustomASCII2";

        (void)TIFFMergeFieldInfo(tif, finfo, 2);
    }

    if (Size > 0)
    {
        size_t slen = Size > 1024 ? 1024 : Size;
        char *str = (char *)malloc(slen + 1);
        if (str)
        {
            memcpy(str, Data, slen);
            str[slen] = '\0';
            (void)call_TIFFVSetField_ascii(tif, custom_tag, str);
            (void)call_TIFFVSetField_ascii(tif, custom_tag + 1, str);
            free(str);
        }
    }
    else
    {
        (void)call_TIFFVSetField_ascii(tif, custom_tag, "");
    }

    if (Size >= 2)
        dirn = (uint16_t)(((uint16_t)Data[0] << 8) | (uint16_t)Data[1]);

    (void)TIFFSetDirectory(tif, dirn);
    (void)TIFFSetDirectory(tif, 0);
    (void)TIFFSetDirectory(tif, 1);

    (void)TIFFUnsetField(tif, custom_tag);
    (void)TIFFUnsetField(tif, custom_tag + 1);
    (void)TIFFUnsetField(tif, 0);
    (void)TIFFUnsetField(tif, 0xffffffffU);

    for (i = -2; i < 8; i++)
    {
        tag_from_list = TIFFGetTagListEntry(tif, i);
        if (tag_from_list != (uint32_t)-1)
            (void)TIFFUnsetField(tif, tag_from_list);
    }

    TIFFFreeDirectory(tif);

    for (i = 0; i < 4; i++)
    {
        (void)TIFFSetDirectory(tif, (uint16_t)i);
        tag_from_list = TIFFGetTagListEntry(tif, i);
        if (tag_from_list != (uint32_t)-1)
            (void)TIFFUnsetField(tif, tag_from_list);
    }

    TIFFClose(tif);
    return 0;
}