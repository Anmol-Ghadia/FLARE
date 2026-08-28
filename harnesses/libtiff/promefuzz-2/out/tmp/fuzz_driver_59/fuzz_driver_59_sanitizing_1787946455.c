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

static void SilentErrorHandler(const char *module, const char *fmt, va_list ap) {
    (void)module;
    (void)fmt;
    (void)ap;
}

static void SilentErrorHandlerExt(thandle_t fd, const char *module, const char *fmt, va_list ap) {
    (void)fd;
    (void)module;
    (void)fmt;
    (void)ap;
}

static const char *pick_string(const uint8_t *Data, size_t Size, size_t *offset) {
    static const char *fallbacks[] = {
        "",
        "mod",
        "warning",
        "error",
        "%s",
        "%d",
        "msg: %s %d",
        "%%%%",
        "libtiff"
    };
    if (*offset >= Size) {
        return fallbacks[0];
    }
    size_t idx = Data[(*offset)++] % (sizeof(fallbacks) / sizeof(fallbacks[0]));
    return fallbacks[idx];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint8_t control = 0;
    if (Size > 0) {
        control = Data[off++];
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > off) {
            (void)fwrite(Data + off, 1, Size - off, fp);
        }
        fclose(fp);
    }

    TIFFErrorHandler oldWarn = TIFFSetWarningHandler(NULL);
    TIFFErrorHandler oldErr = TIFFSetErrorHandler(NULL);
    TIFFErrorHandlerExt oldWarnExt = TIFFSetWarningHandlerExt(NULL);
    TIFFErrorHandlerExt oldErrExt = TIFFSetErrorHandlerExt(NULL);

    TIFFErrorHandler curWarn = NULL;
    TIFFErrorHandler curErr = NULL;
    TIFFErrorHandlerExt curWarnExt = NULL;
    TIFFErrorHandlerExt curErrExt = NULL;

    if (control & 0x01) {
        curWarn = TIFFSetWarningHandler(SilentErrorHandler);
    }
    if (control & 0x02) {
        curErr = TIFFSetErrorHandler(SilentErrorHandler);
    }
    if (control & 0x04) {
        curWarnExt = TIFFSetWarningHandlerExt(SilentErrorHandlerExt);
    }
    if (control & 0x08) {
        curErrExt = TIFFSetErrorHandlerExt(SilentErrorHandlerExt);
    }

    thandle_t handle1 = NULL;
    thandle_t handle2 = (thandle_t)(uintptr_t)Size;
    thandle_t handle3 = (thandle_t)(uintptr_t)(control);

    const char *module1 = pick_string(Data, Size, &off);
    const char *module2 = pick_string(Data, Size, &off);
    const char *fmt1 = "fuzz msg";
    const char *fmt2 = "num=%d str=%s";
    const char *fmt3 = "%s";
    const char *argstr = pick_string(Data, Size, &off);
    int argint = (off < Size) ? (int)Data[off] : 0;

    TIFFWarningExt(handle1, module1, fmt1);
    TIFFWarningExt(handle2, module2, fmt2, argint, argstr);
    TIFFWarningExt(handle3, NULL, fmt3, argstr);

    TIFFErrorExt(handle1, module1, fmt1);
    TIFFErrorExt(handle2, module2, fmt2, argint, argstr);
    TIFFErrorExt(handle3, NULL, fmt3, argstr);

    if (control & 0x10) {
        (void)TIFFSetWarningHandler(curWarn);
        (void)TIFFSetErrorHandler(curErr);
        (void)TIFFSetWarningHandlerExt(curWarnExt);
        (void)TIFFSetErrorHandlerExt(curErrExt);

        TIFFWarningExt(handle2, module1, "after swap %d", argint);
        TIFFErrorExt(handle3, module2, "after swap %s", argstr);
    }

    (void)TIFFSetWarningHandler(oldWarn);
    (void)TIFFSetErrorHandler(oldErr);
    (void)TIFFSetWarningHandlerExt(oldWarnExt);
    (void)TIFFSetErrorHandlerExt(oldErrExt);

    return 0;
}