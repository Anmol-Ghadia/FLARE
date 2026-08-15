// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFVSetField at tif_dir.c:887:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFVGetField at tif_dir.c:1288:1 in tiffio.h
// TIFFVGetFieldDefaulted at tif_aux.c:208:1 in tiffio.h
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

static void call_vsetfield(TIFF *tif, uint32_t tag, ...)
{
    va_list ap;
    va_start(ap, tag);
    (void)TIFFVSetField(tif, tag, ap);
    va_end(ap);
}

static void call_vgetfield(TIFF *tif, uint32_t tag, ...)
{
    va_list ap;
    va_start(ap, tag);
    (void)TIFFVGetField(tif, tag, ap);
    va_end(ap);
}

static void call_vgetfield_defaulted(TIFF *tif, uint32_t tag, ...)
{
    va_list ap;
    va_start(ap, tag);
    (void)TIFFVGetFieldDefaulted(tif, tag, ap);
    va_end(ap);
}

static uint16_t rd16(const uint8_t *p, size_t n, size_t *off)
{
    uint16_t v = 0;
    if (*off < n) v |= (uint16_t)p[(*off)++];
    if (*off < n) v |= (uint16_t)p[(*off)++] << 8;
    return v;
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t *off)
{
    uint32_t v = 0;
    if (*off < n) v |= (uint32_t)p[(*off)++];
    if (*off < n) v |= (uint32_t)p[(*off)++] << 8;
    if (*off < n) v |= (uint32_t)p[(*off)++] << 16;
    if (*off < n) v |= (uint32_t)p[(*off)++] << 24;
    return v;
}

static float rdfloat(const uint8_t *p, size_t n, size_t *off)
{
    uint32_t u = rd32(p, n, off);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void fuzz_on_tiff(TIFF *tif, const uint8_t *Data, size_t Size)
{
    size_t off = 0;
    uint16_t shortv1 = rd16(Data, Size, &off);
    uint16_t shortv2 = rd16(Data, Size, &off);
    uint32_t longv1 = rd32(Data, Size, &off);
    uint32_t longv2 = rd32(Data, Size, &off);
    float floatv = rdfloat(Data, Size, &off);
    char asciibuf[32];
    uint16_t get16 = 0;
    uint32_t get32 = 0;
    float getf = 0.0f;
    char *getstr = NULL;
    uint16_t *get16p = NULL;
    uint32_t *get32p = NULL;

    memset(asciibuf, 0, sizeof(asciibuf));
    if (off < Size) {
        size_t rem = Size - off;
        size_t copy = rem < sizeof(asciibuf) - 1 ? rem : sizeof(asciibuf) - 1;
        memcpy(asciibuf, Data + off, copy);
    } else {
        strcpy(asciibuf, "x");
    }

    {
        TIFFFieldInfo finfo[2];
        memset(finfo, 0, sizeof(finfo));

        finfo[0].field_tag = 65000 + (longv1 % 100);
        finfo[0].field_readcount = 1;
        finfo[0].field_writecount = 1;
        finfo[0].field_type = TIFF_LONG;
        finfo[0].field_bit = FIELD_CUSTOM;
        finfo[0].field_oktochange = 1;
        finfo[0].field_passcount = 0;
        finfo[0].field_name = (char *)"FuzzCustomLong";

        finfo[1].field_tag = 65100 + (longv2 % 100);
        finfo[1].field_readcount = TIFF_VARIABLE;
        finfo[1].field_writecount = TIFF_VARIABLE;
        finfo[1].field_type = TIFF_ASCII;
        finfo[1].field_bit = FIELD_CUSTOM;
        finfo[1].field_oktochange = 1;
        finfo[1].field_passcount = 0;
        finfo[1].field_name = (char *)"FuzzCustomAscii";

        (void)TIFFMergeFieldInfo(tif, finfo, 2);

        call_vsetfield(tif, finfo[0].field_tag, longv1);
        call_vsetfield(tif, finfo[1].field_tag, asciibuf);

        call_vgetfield(tif, finfo[0].field_tag, &get32);
        call_vgetfield_defaulted(tif, finfo[0].field_tag, &get32);
        call_vgetfield(tif, finfo[1].field_tag, &getstr);
        call_vgetfield_defaulted(tif, finfo[1].field_tag, &getstr);

        (void)TIFFUnsetField(tif, finfo[0].field_tag);
        (void)TIFFUnsetField(tif, finfo[1].field_tag);
    }

    call_vsetfield(tif, TIFFTAG_IMAGEWIDTH, longv1 ? longv1 : 1U);
    call_vsetfield(tif, TIFFTAG_IMAGELENGTH, longv2 ? longv2 : 1U);
    call_vsetfield(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)((shortv1 % 32) + 1));
    call_vsetfield(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)((shortv2 % 4) + 1));
    call_vsetfield(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE);
    call_vsetfield(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)(shortv1 & 1 ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB));
    call_vsetfield(tif, TIFFTAG_ORIENTATION, (uint16_t)((shortv2 % 8) + 1));
    call_vsetfield(tif, TIFFTAG_PLANARCONFIG, (uint16_t)(shortv1 & 1 ? PLANARCONFIG_CONTIG : PLANARCONFIG_SEPARATE));
    call_vsetfield(tif, TIFFTAG_FILLORDER, (uint16_t)(shortv2 & 1 ? FILLORDER_MSB2LSB : FILLORDER_LSB2MSB));
    call_vsetfield(tif, TIFFTAG_ROWSPERSTRIP, longv1 ? longv1 : 1U);
    call_vsetfield(tif, TIFFTAG_XRESOLUTION, floatv);
    call_vsetfield(tif, TIFFTAG_YRESOLUTION, floatv == 0.0f ? 1.0f : floatv);
    call_vsetfield(tif, TIFFTAG_RESOLUTIONUNIT, (uint16_t)((shortv1 % 3) + 1));
    call_vsetfield(tif, TIFFTAG_SOFTWARE, asciibuf);
    call_vsetfield(tif, TIFFTAG_DOCUMENTNAME, asciibuf);

    call_vgetfield(tif, TIFFTAG_IMAGEWIDTH, &get32);
    call_vgetfield_defaulted(tif, TIFFTAG_IMAGEWIDTH, &get32);
    call_vgetfield(tif, TIFFTAG_IMAGELENGTH, &get32);
    call_vgetfield_defaulted(tif, TIFFTAG_IMAGELENGTH, &get32);
    call_vgetfield(tif, TIFFTAG_BITSPERSAMPLE, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_BITSPERSAMPLE, &get16);
    call_vgetfield(tif, TIFFTAG_SAMPLESPERPIXEL, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &get16);
    call_vgetfield(tif, TIFFTAG_COMPRESSION, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_COMPRESSION, &get16);
    call_vgetfield(tif, TIFFTAG_PHOTOMETRIC, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_PHOTOMETRIC, &get16);
    call_vgetfield(tif, TIFFTAG_ORIENTATION, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_ORIENTATION, &get16);
    call_vgetfield(tif, TIFFTAG_PLANARCONFIG, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_PLANARCONFIG, &get16);
    call_vgetfield(tif, TIFFTAG_FILLORDER, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_FILLORDER, &get16);
    call_vgetfield(tif, TIFFTAG_ROWSPERSTRIP, &get32);
    call_vgetfield_defaulted(tif, TIFFTAG_ROWSPERSTRIP, &get32);
    call_vgetfield(tif, TIFFTAG_XRESOLUTION, &getf);
    call_vgetfield_defaulted(tif, TIFFTAG_XRESOLUTION, &getf);
    call_vgetfield(tif, TIFFTAG_YRESOLUTION, &getf);
    call_vgetfield_defaulted(tif, TIFFTAG_YRESOLUTION, &getf);
    call_vgetfield(tif, TIFFTAG_RESOLUTIONUNIT, &get16);
    call_vgetfield_defaulted(tif, TIFFTAG_RESOLUTIONUNIT, &get16);
    call_vgetfield(tif, TIFFTAG_SOFTWARE, &getstr);
    call_vgetfield_defaulted(tif, TIFFTAG_SOFTWARE, &getstr);
    call_vgetfield(tif, TIFFTAG_DOCUMENTNAME, &getstr);
    call_vgetfield_defaulted(tif, TIFFTAG_DOCUMENTNAME, &getstr);

    call_vgetfield(tif, TIFFTAG_COLORMAP, &get16p, &get16p, &get16p);
    call_vgetfield_defaulted(tif, TIFFTAG_COLORMAP, &get16p, &get16p, &get16p);
    call_vgetfield(tif, TIFFTAG_STRIPOFFSETS, &get32p);
    call_vgetfield_defaulted(tif, TIFFTAG_STRIPOFFSETS, &get32p);
    call_vgetfield(tif, TIFFTAG_STRIPBYTECOUNTS, &get32p);
    call_vgetfield_defaulted(tif, TIFFTAG_STRIPBYTECOUNTS, &get32p);

    (void)TIFFUnsetField(tif, TIFFTAG_SOFTWARE);
    (void)TIFFUnsetField(tif, TIFFTAG_DOCUMENTNAME);
    (void)TIFFUnsetField(tif, TIFFTAG_XRESOLUTION);
    (void)TIFFUnsetField(tif, TIFFTAG_YRESOLUTION);
    (void)TIFFUnsetField(tif, TIFFTAG_IMAGEWIDTH);
    (void)TIFFUnsetField(tif, TIFFTAG_IMAGELENGTH);
    (void)TIFFUnsetField(tif, 0);
    (void)TIFFUnsetField(tif, 0xffffffffU);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    TIFF *tif;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "w");
    if (tif) {
        fuzz_on_tiff(tif, Data, Size);
        TIFFCleanup(tif);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        fuzz_on_tiff(tif, Data, Size);
        TIFFCleanup(tif);
    }

    return 0;
}