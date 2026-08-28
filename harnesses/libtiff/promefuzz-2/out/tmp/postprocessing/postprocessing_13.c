#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tiffio.h"

static void FuzzWarningHandler(const char *module, const char *fmt, va_list ap) {
    (void)module;
    (void)fmt;
    (void)ap;
}

static void FuzzErrorHandlerExt(thandle_t fd, const char *module, const char *fmt, va_list ap) {
    (void)fd;
    (void)module;
    (void)fmt;
    (void)ap;
}

static size_t bounded_strlen(const char *s, size_t maxlen) {
    size_t i;
    for (i = 0; i < maxlen; ++i) {
        if (s[i] == '\0')
            break;
    }
    return i;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFFErrorHandler oldWarn;
    TIFFErrorHandlerExt oldErrExt;
    char module[64];
    char fmt[128];
    char arg1[64];
    char arg2[64];
    size_t copy1, copy2, copy3, copy4;
    thandle_t handle1;
    thandle_t handle2;
    int num1, num2;
    unsigned int unum;
    long lnum;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(module, 0, sizeof(module));
    memset(fmt, 0, sizeof(fmt));
    memset(arg1, 0, sizeof(arg1));
    memset(arg2, 0, sizeof(arg2));

    copy1 = Size < sizeof(module) - 1 ? Size : sizeof(module) - 1;
    if (copy1 > 0)
        memcpy(module, Data, copy1);

    copy2 = 0;
    if (Size > copy1) {
        size_t remain = Size - copy1;
        copy2 = remain < sizeof(fmt) - 1 ? remain : sizeof(fmt) - 1;
        if (copy2 > 0)
            memcpy(fmt, Data + copy1, copy2);
    }

    copy3 = 0;
    if (Size > copy1 + copy2) {
        size_t remain = Size - copy1 - copy2;
        copy3 = remain < sizeof(arg1) - 1 ? remain : sizeof(arg1) - 1;
        if (copy3 > 0)
            memcpy(arg1, Data + copy1 + copy2, copy3);
    }

    copy4 = 0;
    if (Size > copy1 + copy2 + copy3) {
        size_t remain = Size - copy1 - copy2 - copy3;
        copy4 = remain < sizeof(arg2) - 1 ? remain : sizeof(arg2) - 1;
        if (copy4 > 0)
            memcpy(arg2, Data + copy1 + copy2 + copy3, copy4);
    }

    module[sizeof(module) - 1] = '\0';
    fmt[sizeof(fmt) - 1] = '\0';
    arg1[sizeof(arg1) - 1] = '\0';
    arg2[sizeof(arg2) - 1] = '\0';

    if (bounded_strlen(fmt, sizeof(fmt)) == 0) {
        memcpy(fmt, "msg=%s num=%d hex=%x str=%s", sizeof("msg=%s num=%d hex=%x str=%s"));
    }

    num1 = 0;
    num2 = 0;
    unum = 0;
    lnum = 0;
    if (Size > 0) num1 = (int)Data[0];
    if (Size > 1) num2 = (int)Data[1];
    if (Size > 3) unum = ((unsigned int)Data[0] << 8) | (unsigned int)Data[1];
    if (Size > 7) {
        lnum = ((long)Data[0] << 24) |
               ((long)Data[1] << 16) |
               ((long)Data[2] << 8) |
               (long)Data[3];
    }

    handle1 = (thandle_t)(uintptr_t)Size;
    handle2 = (thandle_t)(uintptr_t)(Size ? Data[0] : 0);

    oldWarn = TIFFSetWarningHandler(FuzzWarningHandler);
    oldErrExt = TIFFSetErrorHandlerExt(FuzzErrorHandlerExt);

    TIFFWarning(module[0] ? module : NULL, "%s", arg1);
    TIFFError(module[0] ? module : NULL, "%s", arg2);
    TIFFWarningExt(handle1, module[0] ? module : NULL, "%s", arg1);
    TIFFErrorExt(handle2, module[0] ? module : NULL, "%s", arg2);

    TIFFWarning(module[0] ? module : NULL, "n=%d u=%u l=%ld s=%s", num1, unum, lnum, arg1);
    TIFFError(module[0] ? module : NULL, "n=%d u=%u l=%ld s=%s", num2, unum, lnum, arg2);
    TIFFWarningExt(handle1, module[0] ? module : NULL, "h=%p n=%d s=%s", handle1, num1, arg1);
    TIFFErrorExt(handle2, module[0] ? module : NULL, "h=%p n=%d s=%s", handle2, num2, arg2);

    TIFFSetWarningHandler(NULL);
    TIFFSetErrorHandlerExt(NULL);

    TIFFWarning(module[0] ? module : NULL, "%s", fmt);
    TIFFError(module[0] ? module : NULL, "%s", fmt);
    TIFFWarningExt(handle1, module[0] ? module : NULL, "%s", fmt);
    TIFFErrorExt(handle2, module[0] ? module : NULL, "%s", fmt);

    TIFFSetWarningHandler(oldWarn);
    TIFFSetErrorHandlerExt(oldErrExt);

    TIFFWarning(module[0] ? module : NULL, "%s", arg1);
    TIFFError(module[0] ? module : NULL, "%s", arg2);
    TIFFWarningExt(handle1, module[0] ? module : NULL, "%s", arg1);
    TIFFErrorExt(handle2, module[0] ? module : NULL, "%s", arg2);

    return 0;
}