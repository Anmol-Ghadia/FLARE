#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

static const char *make_cstring(const uint8_t *data, size_t len)
{
    char *s = (char *)malloc(len + 1);
    if (!s)
        return NULL;
    if (len > 0)
        memcpy(s, data, len);
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    TIFFErrorHandler old_err;
    TIFFErrorHandler old_warn;
    TIFFErrorHandlerExt old_err_ext;
    TIFFErrorHandlerExt old_warn_ext;
    const char *module = NULL;
    const char *fmt = NULL;
    size_t split1, split2;
    uint8_t mode;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            fwrite(Data, 1, Size, fp);
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

    if (!module)
        module = "";
    if (!fmt)
        fmt = "%s";

    old_err = TIFFSetErrorHandler((mode & 0x01) ? fuzz_error_handler : NULL);
    old_warn = TIFFSetWarningHandler((mode & 0x02) ? fuzz_error_handler : NULL);
    old_err_ext = TIFFSetErrorHandlerExt((mode & 0x04) ? fuzz_error_handler_ext : NULL);
    old_warn_ext = TIFFSetWarningHandlerExt((mode & 0x08) ? fuzz_error_handler_ext : NULL);

    TIFFWarning(module, "%s", fmt);
    TIFFError(module, "%s", fmt);

    if (mode & 0x10)
    {
        TIFFSetWarningHandler(fuzz_error_handler);
        TIFFWarning(module, "num=%d str=%s", (int)Size, fmt);
    }

    if (mode & 0x20)
    {
        TIFFSetErrorHandler(fuzz_error_handler);
        TIFFError(module, "byte=%u len=%d", (unsigned)(Data[0]), (int)Size);
    }

    if (mode & 0x40)
    {
        TIFFSetWarningHandlerExt(fuzz_error_handler_ext);
        TIFFSetErrorHandlerExt(fuzz_error_handler_ext);
        TIFFWarning(module, "literal warning");
        TIFFError(module, "literal error");
    }

    if (mode & 0x80)
    {
        TIFFSetErrorHandler(NULL);
        TIFFSetWarningHandler(NULL);
        TIFFSetErrorHandlerExt(NULL);
        TIFFSetWarningHandlerExt(NULL);
        TIFFWarning(module, "%s", fmt);
        TIFFError(module, "%s", fmt);
    }

    TIFFSetErrorHandler(old_err);
    TIFFSetWarningHandler(old_warn);
    TIFFSetErrorHandlerExt(old_err_ext);
    TIFFSetWarningHandlerExt(old_warn_ext);

    if (module && module[0] != '\0')
        free((void *)module);
    if (fmt && fmt[0] != '\0')
        free((void *)fmt);

    return 0;
}