// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFVSetField at tif_dir.c:887:1 in tiffio.h
// TIFFVSetField at tif_dir.c:887:1 in tiffio.h
// TIFFVSetField at tif_dir.c:887:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFGetFieldDefaulted at tif_aux.c:344:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "tiffio.h"

static void call_vsetfield_uint32(TIFF *tif, uint32_t tag, ...)
{
    va_list ap;
    va_start(ap, tag);
    (void)TIFFVSetField(tif, tag, ap);
    va_end(ap);
}

static void call_vsetfield_float(TIFF *tif, uint32_t tag, ...)
{
    va_list ap;
    va_start(ap, tag);
    (void)TIFFVSetField(tif, tag, ap);
    va_end(ap);
}

static void call_vsetfield_ascii(TIFF *tif, uint32_t tag, ...)
{
    va_list ap;
    va_start(ap, tag);
    (void)TIFFVSetField(tif, tag, ap);
    va_end(ap);
}

int LLVMFuzzerTestOneInput_61(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif)
    {
        uint64_t off = 0;
        if (Size >= 8)
        {
            off = ((uint64_t)Data[0]) |
                  ((uint64_t)Data[1] << 8) |
                  ((uint64_t)Data[2] << 16) |
                  ((uint64_t)Data[3] << 24) |
                  ((uint64_t)Data[4] << 32) |
                  ((uint64_t)Data[5] << 40) |
                  ((uint64_t)Data[6] << 48) |
                  ((uint64_t)Data[7] << 56);
        }
        else if (Size > 0)
        {
            size_t i;
            for (i = 0; i < Size; i++)
                off = (off << 8) ^ Data[i];
        }

        (void)TIFFSetSubDirectory(tif, off);
        (void)TIFFSetSubDirectory(tif, 0);
        (void)TIFFSetSubDirectory(tif, (uint64_t)1);
        (void)TIFFSetSubDirectory(tif, ~off);

        {
            uint32_t u32 = 0;
            uint16_t u16 = 0;
            float f = 0.0f;
            char *s = NULL;

            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &u32);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &u32);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &u32);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &u16);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &f);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &f);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SOFTWARE, &s);
        }

        {
            uint32_t strips = TIFFNumberOfStrips(tif);
            uint32_t width = 0, rowsperstrip = 0;
            uint16_t spp = 0;
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &width);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
            (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

            if (width > 0 && width < (1U << 24))
            {
                uint32_t *raster = (uint32_t *)malloc((size_t)width * sizeof(uint32_t));
                if (raster)
                {
                    uint32_t row0 = 0;
                    uint32_t row1 = rowsperstrip ? rowsperstrip : 0;
                    uint32_t row2 = 0;
                    if (strips > 1 && rowsperstrip != 0)
                        row2 = rowsperstrip * (strips - 1);

                    (void)TIFFReadRGBAStripExt(tif, row0, raster, 0);
                    (void)TIFFReadRGBAStripExt(tif, row0, raster, 1);
                    (void)TIFFReadRGBAStripExt(tif, row1, raster, 0);
                    (void)TIFFReadRGBAStripExt(tif, row2, raster, 1);
                    if (rowsperstrip > 1)
                        (void)TIFFReadRGBAStripExt(tif, row1 + 1, raster, (spp & 1));
                    free(raster);
                }
            }
        }

        (void)TIFFNumberOfTiles(tif);
        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r+");
    if (tif)
    {
        uint32_t v32a = (Size >= 4) ? (((uint32_t)Data[0]) | ((uint32_t)Data[1] << 8) |
                                       ((uint32_t)Data[2] << 16) | ((uint32_t)Data[3] << 24))
                                    : (uint32_t)Size;
        uint32_t v32b = (Size >= 8) ? (((uint32_t)Data[4]) | ((uint32_t)Data[5] << 8) |
                                       ((uint32_t)Data[6] << 16) | ((uint32_t)Data[7] << 24))
                                    : (v32a ^ 0x12345678U);
        float fv = (float)(v32a % 10000U) / 17.0f;

        char ascii_buf[64];
        size_t copy_len = Size < sizeof(ascii_buf) - 1 ? Size : sizeof(ascii_buf) - 1;
        if (copy_len > 0)
            memcpy(ascii_buf, Data, copy_len);
        ascii_buf[copy_len] = '\0';

        call_vsetfield_uint32(tif, TIFFTAG_IMAGEWIDTH, v32a ? v32a : 1U);
        call_vsetfield_uint32(tif, TIFFTAG_IMAGELENGTH, v32b ? v32b : 1U);
        call_vsetfield_uint32(tif, TIFFTAG_ROWSPERSTRIP, (v32a % 4096U) + 1U);
        call_vsetfield_uint32(tif, TIFFTAG_TILEWIDTH, (v32a % 1024U));
        call_vsetfield_uint32(tif, TIFFTAG_TILELENGTH, (v32b % 1024U));
        call_vsetfield_uint32(tif, TIFFTAG_BITSPERSAMPLE, (v32a % 32U) + 1U);
        call_vsetfield_uint32(tif, TIFFTAG_SAMPLESPERPIXEL, (v32b % 8U) + 1U);
        call_vsetfield_uint32(tif, TIFFTAG_PLANARCONFIG, (v32a & 1U) ? PLANARCONFIG_CONTIG : PLANARCONFIG_SEPARATE);
        call_vsetfield_uint32(tif, TIFFTAG_ORIENTATION, (v32b % 8U) + 1U);
        call_vsetfield_float(tif, TIFFTAG_XRESOLUTION, fv);
        call_vsetfield_float(tif, TIFFTAG_YRESOLUTION, fv + 1.0f);
        call_vsetfield_ascii(tif, TIFFTAG_SOFTWARE, ascii_buf);

        (void)TIFFNumberOfStrips(tif);
        (void)TIFFNumberOfTiles(tif);

        TIFFClose(tif);
    }

    remove("./dummy_file");
    return 0;
}