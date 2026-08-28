#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static int call_TIFFVSetField_wrapper(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVSetField(tif, tag, ap);
    va_end(ap);
    return ret;
}

static int call_TIFFVGetField_wrapper(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVGetField(tif, tag, ap);
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
    static const uint32_t known_tags[] = {
        TIFFTAG_IMAGEWIDTH,
        TIFFTAG_IMAGELENGTH,
        TIFFTAG_BITSPERSAMPLE,
        TIFFTAG_COMPRESSION,
        TIFFTAG_PHOTOMETRIC,
        TIFFTAG_FILLORDER,
        TIFFTAG_ORIENTATION,
        TIFFTAG_SAMPLESPERPIXEL,
        TIFFTAG_ROWSPERSTRIP,
        TIFFTAG_PLANARCONFIG,
        TIFFTAG_RESOLUTIONUNIT,
        TIFFTAG_XRESOLUTION,
        TIFFTAG_YRESOLUTION,
        TIFFTAG_SOFTWARE,
        TIFFTAG_DATETIME,
        TIFFTAG_ARTIST,
        TIFFTAG_HOSTCOMPUTER,
        TIFFTAG_SUBFILETYPE,
        TIFFTAG_PAGENUMBER,
        TIFFTAG_HALFTONEHINTS,
        TIFFTAG_COLORMAP,
        TIFFTAG_INKSET,
        TIFFTAG_DOTRANGE,
        TIFFTAG_TARGETPRINTER,
        TIFFTAG_EXTRASAMPLES,
        TIFFTAG_SAMPLEFORMAT,
        TIFFTAG_SMINSAMPLEVALUE,
        TIFFTAG_SMAXSAMPLEVALUE,
        TIFFTAG_PREDICTOR,
        TIFFTAG_TILEWIDTH,
        TIFFTAG_TILELENGTH
    };

    uint32_t width = 0, length = 0, rowsperstrip = 0;
    uint16_t shortv = 0, shortv2 = 0, shortv3 = 0;
    float floatv = 0.0f;
    char *strv = NULL;
    uint16_t *short_array = NULL;
    uint32_t custom_tag = 0;
    uint32_t tag = 0;
    size_t i;
    TIFF *tif;

    write_input_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r+");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "w+");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "w8");
    if (!tif)
        return 0;

    if (Size >= 1)
    {
        width = (uint32_t)Data[0] + 1U;
        length = (Size >= 2) ? ((uint32_t)Data[1] + 1U) : 1U;
    }
    else
    {
        width = 1;
        length = 1;
    }

    shortv = (Size >= 3) ? (uint16_t)((Data[2] % 16) + 1) : 1;
    shortv2 = (Size >= 4) ? (uint16_t)((Data[3] % 4) + 1) : 1;
    shortv3 = (Size >= 5) ? (uint16_t)(Data[4] % 3) : 0;
    rowsperstrip = (Size >= 6) ? ((uint32_t)Data[5] + 1U) : 1U;
    floatv = (Size >= 7) ? ((float)(Data[6] + 1) / 3.0f) : 72.0f;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)1);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_MINISBLACK);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT);
    (void)TIFFSetField(tif, TIFFTAG_XRESOLUTION, 72.0);
    (void)TIFFSetField(tif, TIFFTAG_YRESOLUTION, 72.0);
    (void)TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, (uint16_t)RESUNIT_INCH);

    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_IMAGELENGTH, length);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_ORIENTATION, shortv);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_FILLORDER, shortv2);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)1);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_RESOLUTIONUNIT, shortv3);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_XRESOLUTION, (double)floatv);
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_YRESOLUTION, (double)(floatv + 1.0f));
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_SOFTWARE, "fuzz");
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_ARTIST, "a");
    (void)call_TIFFVSetField_wrapper(tif, TIFFTAG_HOSTCOMPUTER, "host");

    (void)TIFFCheckpointDirectory(tif);

    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &length);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &shortv);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &shortv2);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &shortv3);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &floatv);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &floatv);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SOFTWARE, &strv);

    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_IMAGELENGTH, &length);
    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_BITSPERSAMPLE, &shortv);
    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_SAMPLESPERPIXEL, &shortv2);
    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_ORIENTATION, &shortv3);
    (void)call_TIFFVGetField_wrapper(tif, TIFFTAG_SOFTWARE, &strv);

    for (i = 0; i < sizeof(known_tags) / sizeof(known_tags[0]); i++)
    {
        (void)TIFFUnsetField(tif, known_tags[i]);
        (void)TIFFGetFieldDefaulted(tif, known_tags[i], &width);
    }

    custom_tag = (Size >= 11)
                     ? ((uint32_t)Data[7] << 24) | ((uint32_t)Data[8] << 16) |
                           ((uint32_t)Data[9] << 8) | (uint32_t)Data[10]
                     : 65000U;
    (void)TIFFUnsetField(tif, custom_tag);
    (void)TIFFUnsetField(tif, 0U);
    (void)TIFFUnsetField(tif, 0xFFFFFFFFU);

    for (i = 0; i < 8; i++)
    {
        uint32_t entry = TIFFGetTagListEntry(tif, (int)i);
        if (entry != (uint32_t)-1)
        {
            tag = entry;
            (void)TIFFUnsetField(tif, tag);
        }
    }

    if (Size >= 12)
    {
        int idx = (int)(Data[11] % 16);
        tag = TIFFGetTagListEntry(tif, idx);
        if (tag != (uint32_t)-1)
        {
            (void)TIFFGetFieldDefaulted(tif, tag, &width);
            (void)TIFFUnsetField(tif, tag);
        }
    }

    (void)TIFFGetTagListEntry(tif, -1);
    (void)TIFFGetTagListEntry(tif, 0);
    (void)TIFFGetTagListEntry(tif, 1);
    (void)TIFFGetTagListEntry(tif, 1024);

    (void)TIFFCheckpointDirectory(tif);

    TIFFClose(tif);
    (void)short_array;
    return 0;
}