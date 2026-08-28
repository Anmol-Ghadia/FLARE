#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "tiffio.h"

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

static void write_input_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static uint16_t rd16(const uint8_t *p, size_t n, size_t off)
{
    if (off + 1 >= n)
        return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off)
{
    if (off + 3 >= n)
        return 0;
    return (uint32_t)p[off] | ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) | ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    uint32_t base_tag;
    TIFF *tif;
    char ascii_buf[32];
    uint16_t short_val = 0;
    uint32_t long_val = 0;
    char *str_val = NULL;
    int i;

    write_input_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r+");
    if (tif == NULL)
        tif = TIFFOpen("./dummy_file", "w+");
    if (tif == NULL)
        return 0;

    base_tag = 65000U;
    if (Size >= 2)
        base_tag += (uint32_t)(rd16(Data, Size, 0) % 100U);

    {
        TIFFFieldInfo compat_fields[3];

        compat_fields[0].field_tag = base_tag;
        compat_fields[0].field_readcount = 1;
        compat_fields[0].field_writecount = 1;
        compat_fields[0].field_type = TIFF_SHORT;
        compat_fields[0].field_bit = FIELD_CUSTOM;
        compat_fields[0].field_oktochange = 1;
        compat_fields[0].field_passcount = 0;
        compat_fields[0].field_name = (char *)"FuzzCustomShort";

        compat_fields[1].field_tag = base_tag + 1;
        compat_fields[1].field_readcount = 1;
        compat_fields[1].field_writecount = 1;
        compat_fields[1].field_type = TIFF_LONG;
        compat_fields[1].field_bit = FIELD_CUSTOM;
        compat_fields[1].field_oktochange = 1;
        compat_fields[1].field_passcount = 0;
        compat_fields[1].field_name = (char *)"FuzzCustomLong";

        compat_fields[2].field_tag = base_tag + 2;
        compat_fields[2].field_readcount = TIFF_VARIABLE;
        compat_fields[2].field_writecount = TIFF_VARIABLE;
        compat_fields[2].field_type = TIFF_ASCII;
        compat_fields[2].field_bit = FIELD_CUSTOM;
        compat_fields[2].field_oktochange = 1;
        compat_fields[2].field_passcount = 0;
        compat_fields[2].field_name = (char *)"FuzzCustomAscii";

        (void)TIFFMergeFieldInfo(tif, compat_fields, 3);
    }

    (void)TIFFCreateDirectory(tif);

    if (Size > 0)
    {
        short_val = (uint16_t)Data[0];
        long_val = (Size >= 4) ? rd32(Data, Size, 0) : (uint32_t)Data[0];

        ascii_buf[0] = 'A' + (char)(Data[0] % 26);
        ascii_buf[1] = '\0';
        if (Size > 1)
        {
            size_t copy = Size - 1;
            if (copy > sizeof(ascii_buf) - 1)
                copy = sizeof(ascii_buf) - 1;
            for (i = 0; i < (int)copy; ++i)
            {
                unsigned char c = Data[i + 1];
                ascii_buf[i] = (char)((c >= 32 && c <= 126) ? c : 'A' + (c % 26));
            }
            ascii_buf[copy] = '\0';
        }

        (void)TIFFSetField(tif, base_tag, short_val);
        (void)TIFFSetField(tif, base_tag + 1, long_val);
        (void)TIFFSetField(tif, base_tag + 2, ascii_buf);
    }

    (void)TIFFUnsetField(tif, base_tag);
    (void)TIFFUnsetField(tif, base_tag + 1);
    (void)TIFFUnsetField(tif, base_tag + 2);

    if (Size > 0)
    {
        uint32_t dyn_tag = base_tag + (uint32_t)(Data[0] % 32U);
        (void)TIFFUnsetField(tif, dyn_tag);
    }

    for (i = -2; i < 8; ++i)
        (void)TIFFGetTagListEntry(tif, i);

    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &long_val);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &long_val);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &short_val);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &short_val);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &short_val);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &short_val);

    (void)TIFFGetFieldDefaulted(tif, base_tag, &short_val);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 1, &long_val);
    (void)TIFFGetFieldDefaulted(tif, base_tag + 2, &str_val);

    if (Size >= 8)
    {
        uint32_t extra_tag = rd32(Data, Size, 4);
        (void)TIFFUnsetField(tif, extra_tag);
        (void)TIFFGetFieldDefaulted(tif, extra_tag, &long_val);
    }

    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    return 0;
}