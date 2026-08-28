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

static void fuzz_warning_handler(const char *module, const char *fmt, va_list ap)
{
    (void)module;
    (void)fmt;
    (void)ap;
}

static void fuzz_warning_handler_ext(thandle_t fd, const char *module, const char *fmt, va_list ap)
{
    (void)fd;
    (void)module;
    (void)fmt;
    (void)ap;
}

static void fuzz_error_handler(const char *module, const char *fmt, va_list ap)
{
    (void)module;
    (void)fmt;
    (void)ap;
}

static void fuzz_error_handler_ext(thandle_t fd, const char *module, const char *fmt, va_list ap)
{
    (void)fd;
    (void)module;
    (void)fmt;
    (void)ap;
}

static int call_vgetfield_wrapper(TIFF *tif, uint32_t tag, ...)
{
    int ret;
    va_list ap;
    va_start(ap, tag);
    ret = TIFFVGetField(tif, tag, ap);
    va_end(ap);
    return ret;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    TIFFErrorHandler old_warn = TIFFSetWarningHandler(fuzz_warning_handler);
    TIFFErrorHandler old_err = TIFFSetErrorHandler(fuzz_error_handler);
    TIFFErrorHandlerExt old_warn_ext = TIFFSetWarningHandlerExt(fuzz_warning_handler_ext);
    TIFFErrorHandlerExt old_err_ext = TIFFSetErrorHandlerExt(fuzz_error_handler_ext);

    const char *module = "fuzz";
    char fmtbuf[128];
    size_t copylen = Size < sizeof(fmtbuf) - 1 ? Size : sizeof(fmtbuf) - 1;
    memcpy(fmtbuf, Data, copylen);
    fmtbuf[copylen] = '\0';

    TIFFWarningExt((thandle_t)(uintptr_t)Size, module, "%s", fmtbuf);
    TIFFWarningExt(NULL, module, "size=%u first=%u", (unsigned)Size,
                   (unsigned)(Size ? Data[0] : 0));
    TIFFWarningExt((thandle_t)(uintptr_t)(Size ? Data[0] : 0), NULL, "%s", fmtbuf);

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL)
    {
        uint32_t u32 = 0;
        uint16_t u16 = 0;
        float f = 0.0f;
        char *str = NULL;
        uint16_t *u16p = NULL;
        uint32_t *u32p = NULL;
        float *fpv = NULL;

        (void)call_vgetfield_wrapper(tif, TIFFTAG_IMAGEWIDTH, &u32);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_IMAGELENGTH, &u32);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_BITSPERSAMPLE, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_COMPRESSION, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_PHOTOMETRIC, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_FILLORDER, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_PLANARCONFIG, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_ORIENTATION, &u16);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_XRESOLUTION, &f);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_YRESOLUTION, &f);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_SOFTWARE, &str);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_ARTIST, &str);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_HOSTCOMPUTER, &str);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_PAGENAME, &str);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_GRAYRESPONSECURVE, &u16p);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_COLORMAP, &u16p, &u16p, &u16p);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_TRANSFERFUNCTION, &u16p);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_WHITEPOINT, &fpv);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_PRIMARYCHROMATICITIES, &fpv);
        (void)call_vgetfield_wrapper(tif, TIFFTAG_SUBIFD, &u16, &u32p);

        TIFFClose(tif);
    }

    (void)TIFFSetWarningHandler(NULL);
    (void)TIFFSetErrorHandler(NULL);
    (void)TIFFSetWarningHandlerExt(NULL);
    (void)TIFFSetErrorHandlerExt(NULL);

    (void)TIFFSetWarningHandler(old_warn);
    (void)TIFFSetErrorHandler(old_err);
    (void)TIFFSetWarningHandlerExt(old_warn_ext);
    (void)TIFFSetErrorHandlerExt(old_err_ext);

    return 0;
}