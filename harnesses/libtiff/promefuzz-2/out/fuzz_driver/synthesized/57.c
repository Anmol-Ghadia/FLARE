// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFVGetField at tif_dir.c:1288:1 in tiffio.h
// TIFFVGetFieldDefaulted at tif_aux.c:208:1 in tiffio.h
// TIFFVSetField at tif_dir.c:887:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "tiffio.h"

static int call_TIFFVGetField(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVGetField(tif, tag, ap);
    va_end(ap);
    return ret;
}

static int call_TIFFVGetFieldDefaulted(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVGetFieldDefaulted(tif, tag, ap);
    va_end(ap);
    return ret;
}

static int call_TIFFVSetField(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVSetField(tif, tag, ap);
    va_end(ap);
    return ret;
}

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_57(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    TIFF *tif = NULL;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r+");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    {
        uint64_t subdir = 0;
        if (Size >= 8)
            subdir = ((uint64_t)rd32(Data) << 32) | rd32(Data + 4);
        else if (Size >= 4)
            subdir = rd32(Data);
        (void)TIFFSetSubDirectory(tif, subdir);
    }

    {
        uint32_t width32 = 0, length32 = 0;
        uint16_t shortv = 0, spp = 0, bps = 0, compression = 0, photometric = 0;
        float xres = 0.0f, yres = 0.0f;
        char *artist = NULL;
        char *software = NULL;
        char *datetime = NULL;
        uint16_t extrasamples_count = 0;
        uint16_t *extrasamples = NULL;

        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &width32);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &length32);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &xres);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &yres);

        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &width32);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &length32);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &shortv);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &shortv);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &shortv);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &shortv);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &xres);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &yres);

        (void)call_TIFFVGetField(tif, TIFFTAG_ARTIST, &artist);
        (void)call_TIFFVGetField(tif, TIFFTAG_SOFTWARE, &software);
        (void)call_TIFFVGetField(tif, TIFFTAG_DATETIME, &datetime);
        (void)call_TIFFVGetField(tif, TIFFTAG_EXTRASAMPLES, &extrasamples_count, &extrasamples);
    }

    if (Size > 0)
    {
        uint8_t selector = Data[0] % 8;

        switch (selector)
        {
            case 0:
            {
                uint32_t v = (Size >= 5) ? rd32(Data + 1) : (uint32_t)Size;
                (void)call_TIFFVSetField(tif, TIFFTAG_IMAGEWIDTH, v);
                break;
            }
            case 1:
            {
                uint32_t v = (Size >= 5) ? rd32(Data + 1) : (uint32_t)(Size * 2U + 1U);
                (void)call_TIFFVSetField(tif, TIFFTAG_IMAGELENGTH, v);
                break;
            }
            case 2:
            {
                uint16_t v = (Size >= 3) ? rd16(Data + 1) : (uint16_t)8;
                if (v == 0) v = 1;
                (void)call_TIFFVSetField(tif, TIFFTAG_BITSPERSAMPLE, (int)v);
                break;
            }
            case 3:
            {
                uint16_t v = (Size >= 3) ? rd16(Data + 1) : (uint16_t)1;
                if (v == 0) v = 1;
                (void)call_TIFFVSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (int)v);
                break;
            }
            case 4:
            {
                uint16_t v = (Size >= 3) ? rd16(Data + 1) : (uint16_t)1;
                (void)call_TIFFVSetField(tif, TIFFTAG_COMPRESSION, (int)v);
                break;
            }
            case 5:
            {
                uint16_t v = (Size >= 3) ? rd16(Data + 1) : (uint16_t)2;
                (void)call_TIFFVSetField(tif, TIFFTAG_PHOTOMETRIC, (int)v);
                break;
            }
            case 6:
            {
                float fv = 72.0f;
                if (Size >= 5)
                {
                    uint32_t raw = rd32(Data + 1);
                    fv = (float)((raw % 10000U) + 1U) / 10.0f;
                }
                (void)call_TIFFVSetField(tif, TIFFTAG_XRESOLUTION, (double)fv);
                (void)call_TIFFVSetField(tif, TIFFTAG_YRESOLUTION, (double)(fv + 1.0f));
                break;
            }
            case 7:
            {
                char buf[64];
                size_t n = Size - 1;
                if (n > sizeof(buf) - 1)
                    n = sizeof(buf) - 1;
                memcpy(buf, Data + 1, n);
                buf[n] = '\0';
                (void)call_TIFFVSetField(tif, TIFFTAG_SOFTWARE, buf);
                (void)call_TIFFVSetField(tif, TIFFTAG_ARTIST, buf);
                break;
            }
        }
    }

    TIFFFreeDirectory(tif);

    {
        uint32_t width32 = 0;
        uint16_t compression = 0;
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &width32);
        (void)call_TIFFVGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
    }

    TIFFClose(tif);
    return 0;
}