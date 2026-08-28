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

static void fuzz_getters(TIFF *tif, const uint8_t *Data, size_t Size)
{
    uint32_t u32 = 0;
    uint16_t u16 = 0;
    float f = 0.0f;
    char *str = NULL;
    uint16_t *u16p = NULL;
    uint32_t *u32p = NULL;
    float *fp = NULL;
    uint16_t extra = 0;
    uint16_t sampleformat = 0;
    uint16_t bitspersample = 0;
    uint16_t compression = 0;
    uint16_t photometric = 0;
    uint16_t orientation = 0;
    uint16_t planarconfig = 0;
    uint16_t resunit = 0;
    uint16_t fillorder = 0;
    uint16_t xposition_dummy = 0;
    uint16_t yposition_dummy = 0;
    uint32_t rowsperstrip = 0;
    uint32_t imagewidth = 0;
    uint32_t imagelength = 0;
    uint32_t tilewidth = 0;
    uint32_t tilelength = 0;

    (void)xposition_dummy;
    (void)yposition_dummy;

    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &imagelength);
    TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
    TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
    TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &fillorder);
    TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orientation);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
    TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);
    TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &resunit);
    TIFFGetFieldDefaulted(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    TIFFGetFieldDefaulted(tif, TIFFTAG_TILELENGTH, &tilelength);
    TIFFGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &f);
    TIFFGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &f);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SOFTWARE, &str);
    TIFFGetFieldDefaulted(tif, TIFFTAG_DATETIME, &str);
    TIFFGetFieldDefaulted(tif, TIFFTAG_ARTIST, &str);
    TIFFGetFieldDefaulted(tif, TIFFTAG_HOSTCOMPUTER, &str);
    TIFFGetFieldDefaulted(tif, TIFFTAG_DOCUMENTNAME, &str);
    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEDESCRIPTION, &str);
    TIFFGetFieldDefaulted(tif, TIFFTAG_MINSAMPLEVALUE, &u16);
    TIFFGetFieldDefaulted(tif, TIFFTAG_MAXSAMPLEVALUE, &u16);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);

    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &u32);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &u32);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &u32);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &f);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &f);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_SOFTWARE, &str);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_IMAGEDESCRIPTION, &str);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_DOCUMENTNAME, &str);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_ARTIST, &str);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_DATETIME, &str);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_HOSTCOMPUTER, &str);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_MINSAMPLEVALUE, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_MAXSAMPLEVALUE, &u16);
    call_TIFFVGetFieldDefaulted(tif, TIFFTAG_SAMPLEFORMAT, &u16);

    call_TIFFVGetField(tif, TIFFTAG_IMAGEWIDTH, &u32);
    call_TIFFVGetField(tif, TIFFTAG_IMAGELENGTH, &u32);
    call_TIFFVGetField(tif, TIFFTAG_BITSPERSAMPLE, &u16);
    call_TIFFVGetField(tif, TIFFTAG_COMPRESSION, &u16);
    call_TIFFVGetField(tif, TIFFTAG_PHOTOMETRIC, &u16);
    call_TIFFVGetField(tif, TIFFTAG_FILLORDER, &u16);
    call_TIFFVGetField(tif, TIFFTAG_ORIENTATION, &u16);
    call_TIFFVGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
    call_TIFFVGetField(tif, TIFFTAG_ROWSPERSTRIP, &u32);
    call_TIFFVGetField(tif, TIFFTAG_PLANARCONFIG, &u16);
    call_TIFFVGetField(tif, TIFFTAG_RESOLUTIONUNIT, &u16);
    call_TIFFVGetField(tif, TIFFTAG_XRESOLUTION, &f);
    call_TIFFVGetField(tif, TIFFTAG_YRESOLUTION, &f);
    call_TIFFVGetField(tif, TIFFTAG_SOFTWARE, &str);
    call_TIFFVGetField(tif, TIFFTAG_IMAGEDESCRIPTION, &str);
    call_TIFFVGetField(tif, TIFFTAG_DOCUMENTNAME, &str);
    call_TIFFVGetField(tif, TIFFTAG_ARTIST, &str);
    call_TIFFVGetField(tif, TIFFTAG_DATETIME, &str);
    call_TIFFVGetField(tif, TIFFTAG_HOSTCOMPUTER, &str);
    call_TIFFVGetField(tif, TIFFTAG_MINSAMPLEVALUE, &u16);
    call_TIFFVGetField(tif, TIFFTAG_MAXSAMPLEVALUE, &u16);
    call_TIFFVGetField(tif, TIFFTAG_SAMPLEFORMAT, &u16);

    if (Size > 0) {
        switch (Data[0] % 4) {
            case 0:
                TIFFGetFieldDefaulted(tif, TIFFTAG_COLORMAP, &u16p, &u16p, &u16p);
                call_TIFFVGetFieldDefaulted(tif, TIFFTAG_COLORMAP, &u16p, &u16p, &u16p);
                call_TIFFVGetField(tif, TIFFTAG_COLORMAP, &u16p, &u16p, &u16p);
                break;
            case 1:
                TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extra, &u16p);
                call_TIFFVGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extra, &u16p);
                call_TIFFVGetField(tif, TIFFTAG_EXTRASAMPLES, &extra, &u16p);
                break;
            case 2:
                TIFFGetFieldDefaulted(tif, TIFFTAG_STRIPOFFSETS, &u32p);
                TIFFGetFieldDefaulted(tif, TIFFTAG_STRIPBYTECOUNTS, &u32p);
                call_TIFFVGetFieldDefaulted(tif, TIFFTAG_STRIPOFFSETS, &u32p);
                call_TIFFVGetFieldDefaulted(tif, TIFFTAG_STRIPBYTECOUNTS, &u32p);
                call_TIFFVGetField(tif, TIFFTAG_STRIPOFFSETS, &u32p);
                call_TIFFVGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &u32p);
                break;
            default:
                TIFFGetFieldDefaulted(tif, TIFFTAG_TRANSFERFUNCTION, &u16p, &u16p, &u16p);
                call_TIFFVGetFieldDefaulted(tif, TIFFTAG_TRANSFERFUNCTION, &u16p, &u16p, &u16p);
                call_TIFFVGetField(tif, TIFFTAG_TRANSFERFUNCTION, &u16p, &u16p, &u16p);
                break;
        }
    }

    if (Size > 1) {
        if (Data[1] & 1) {
            TIFFGetFieldDefaulted(tif, TIFFTAG_WHITEPOINT, &fp, &fp);
            call_TIFFVGetFieldDefaulted(tif, TIFFTAG_WHITEPOINT, &fp, &fp);
            call_TIFFVGetField(tif, TIFFTAG_WHITEPOINT, &fp, &fp);
        } else {
            TIFFGetFieldDefaulted(tif, TIFFTAG_PRIMARYCHROMATICITIES, &fp, &fp, &fp, &fp, &fp, &fp);
            call_TIFFVGetFieldDefaulted(tif, TIFFTAG_PRIMARYCHROMATICITIES, &fp, &fp, &fp, &fp, &fp, &fp);
            call_TIFFVGetField(tif, TIFFTAG_PRIMARYCHROMATICITIES, &fp, &fp, &fp, &fp, &fp, &fp);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    TIFF *tif;
    uint16_t dirn;
    uint32_t tag;
    size_t i;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r+");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    fuzz_getters(tif, Data, Size);

    if (Size >= 2) {
        dirn = (uint16_t)((((uint16_t)Data[0]) << 8) | Data[1]);
        TIFFSetDirectory(tif, dirn);
        TIFFSetDirectory(tif, (uint16_t)(dirn % 8));
        TIFFSetDirectory(tif, 0);
    } else {
        TIFFSetDirectory(tif, 0);
        TIFFSetDirectory(tif, 1);
    }

    for (i = 0; i < Size && i < 16; ++i) {
        if (i + 3 < Size) {
            tag = ((uint32_t)Data[i] << 24) |
                  ((uint32_t)Data[i + 1] << 16) |
                  ((uint32_t)Data[i + 2] << 8) |
                  (uint32_t)Data[i + 3];
        } else {
            tag = (uint32_t)Data[i];
        }
        TIFFUnsetField(tif, tag);
    }

    TIFFUnsetField(tif, TIFFTAG_IMAGEWIDTH);
    TIFFUnsetField(tif, TIFFTAG_IMAGELENGTH);
    TIFFUnsetField(tif, TIFFTAG_BITSPERSAMPLE);
    TIFFUnsetField(tif, TIFFTAG_COMPRESSION);
    TIFFUnsetField(tif, TIFFTAG_PHOTOMETRIC);
    TIFFUnsetField(tif, TIFFTAG_ROWSPERSTRIP);
    TIFFUnsetField(tif, TIFFTAG_SOFTWARE);
    TIFFUnsetField(tif, TIFFTAG_IMAGEDESCRIPTION);

    fuzz_getters(tif, Data, Size);

    TIFFCheckpointDirectory(tif);

    if (Size > 2) {
        TIFFSetDirectory(tif, (uint16_t)(Data[2] % 4));
        TIFFCheckpointDirectory(tif);
        fuzz_getters(tif, Data, Size);
    }

    TIFFClose(tif);
    return 0;
}