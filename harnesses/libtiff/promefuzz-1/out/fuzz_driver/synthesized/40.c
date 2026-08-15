// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetErrorHandler at tif_error.c:33:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFSetErrorHandler at tif_error.c:33:1 in tiffio.h
// TIFFSetWarningHandler at tif_warning.c:33:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFSetErrorHandler at tif_error.c:33:1 in tiffio.h
// TIFFSetWarningHandler at tif_warning.c:33:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFSetErrorHandler at tif_error.c:33:1 in tiffio.h
// TIFFSetWarningHandler at tif_warning.c:33:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFSetErrorHandler at tif_error.c:33:1 in tiffio.h
// TIFFSetWarningHandler at tif_warning.c:33:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFSetErrorHandler at tif_error.c:33:1 in tiffio.h
// TIFFSetWarningHandler at tif_warning.c:33:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFSetWarningHandler at tif_warning.c:33:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "tiffio.h"

static void fuzz_error_handler(const char *module, const char *fmt, va_list ap)
{
    (void)module;
    (void)fmt;
    (void)ap;
}

static void fuzz_error_handler_ext(thandle_t fd, const char *module, const char *fmt,
                                   va_list ap)
{
    (void)fd;
    (void)module;
    (void)fmt;
    (void)ap;
}

static char *make_cstring(const uint8_t *data, size_t len)
{
    char *s = (char *)malloc(len + 1);
    if (s == NULL)
        return NULL;
    if (len > 0)
        memcpy(s, data, len);
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput_40(const uint8_t *Data, size_t Size)
{
    TIFFErrorHandler old_err;
    TIFFErrorHandler old_warn;
    TIFFErrorHandlerExt old_err_ext;
    TIFFErrorHandlerExt old_warn_ext;
    char *module = NULL;
    char *fmt = NULL;
    size_t split1, split2;
    uint8_t mode;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (Size == 0)
    {
        old_err = TIFFSetErrorHandler(NULL);
        old_warn = TIFFSetWarningHandler(NULL);
        old_err_ext = TIFFSetErrorHandlerExt(NULL);
        old_warn_ext = TIFFSetWarningHandlerExt(NULL);

        TIFFWarning("mod", "empty");
        TIFFError("mod", "empty");

        TIFFSetErrorHandler(old_err);
        TIFFSetWarningHandler(old_warn);
        TIFFSetErrorHandlerExt(old_err_ext);
        TIFFSetWarningHandlerExt(old_warn_ext);
        return 0;
    }

    mode = Data[0];
    split1 = 1 + (Size > 1 ? (size_t)(Data[0] % Size) : 0);
    if (split1 > Size)
        split1 = Size;
    split2 = split1 + ((Size > split1) ? (size_t)(Data[split1 - 1] % (Size - split1 + 1)) : 0);
    if (split2 > Size)
        split2 = Size;

    module = make_cstring(Data + 1, split1 > 1 ? split1 - 1 : 0);
    fmt = make_cstring(Data + split1, split2 > split1 ? split2 - split1 : 0);

    old_err = TIFFSetErrorHandler((mode & 0x01) ? fuzz_error_handler : NULL);
    old_warn = TIFFSetWarningHandler((mode & 0x02) ? fuzz_error_handler : NULL);
    old_err_ext = TIFFSetErrorHandlerExt((mode & 0x04) ? fuzz_error_handler_ext : NULL);
    old_warn_ext = TIFFSetWarningHandlerExt((mode & 0x08) ? fuzz_error_handler_ext : NULL);

    TIFFWarning(module ? module : "", "%s", fmt ? fmt : "");
    TIFFError(module ? module : "", "%s", fmt ? fmt : "");

    if (mode & 0x10)
    {
        TIFFSetWarningHandler(fuzz_error_handler);
        TIFFWarning(module ? module : "", "num=%d str=%s", (int)Size, fmt ? fmt : "");
    }

    if (mode & 0x20)
    {
        TIFFSetErrorHandler(fuzz_error_handler);
        TIFFError(module ? module : "", "byte=%u len=%d", (unsigned)(Data[0]), (int)Size);
    }

    if (mode & 0x40)
    {
        TIFFSetWarningHandlerExt(fuzz_error_handler_ext);
        TIFFSetErrorHandlerExt(fuzz_error_handler_ext);
        TIFFWarning(module ? module : "", "literal warning");
        TIFFError(module ? module : "", "literal error");
    }

    if (mode & 0x80)
    {
        TIFFSetErrorHandler(NULL);
        TIFFSetWarningHandler(NULL);
        TIFFSetErrorHandlerExt(NULL);
        TIFFSetWarningHandlerExt(NULL);
        TIFFWarning(module ? module : "", "%s", fmt ? fmt : "");
        TIFFError(module ? module : "", "%s", fmt ? fmt : "");
    }

    TIFFSetErrorHandler(old_err);
    TIFFSetWarningHandler(old_warn);
    TIFFSetErrorHandlerExt(old_err_ext);
    TIFFSetWarningHandlerExt(old_warn_ext);

    free(module);
    free(fmt);

    return 0;
}