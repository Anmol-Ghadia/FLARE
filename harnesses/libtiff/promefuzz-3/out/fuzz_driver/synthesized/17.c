// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFWarningExt at tif_warning.c:65:1 in tiffio.h
// TIFFErrorExt at tif_error.c:65:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFWarningExt at tif_warning.c:65:1 in tiffio.h
// TIFFErrorExt at tif_error.c:65:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFWarningExt at tif_warning.c:65:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFErrorExt at tif_error.c:65:1 in tiffio.h
// TIFFSetWarningHandlerExt at tif_warning.c:41:1 in tiffio.h
// TIFFSetErrorHandlerExt at tif_error.c:41:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static void FuzzWarningHandlerExt(thandle_t fd, const char *module, const char *fmt, va_list ap) {
    (void)fd;
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

static size_t consume_u8(const uint8_t *Data, size_t Size, size_t *offset) {
    if (*offset >= Size) return 0;
    return Data[(*offset)++];
}

static size_t make_cstring(const uint8_t *Data, size_t Size, size_t *offset, char *out, size_t outsz) {
    size_t i = 0;
    if (outsz == 0) return 0;
    while (*offset < Size && i + 1 < outsz) {
        uint8_t c = Data[(*offset)++];
        if (c == 0) break;
        if (c == '%')
            out[i++] = '_';
        else if (c >= 32 && c <= 126)
            out[i++] = (char)c;
        else
            out[i++] = (char)('A' + (c % 26));
    }
    out[i] = '\0';
    return i;
}

int LLVMFuzzerTestOneInput_17(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    char module1[64], module2[64];
    char msg1[128], msg2[128];
    FILE *fp;
    thandle_t handle;
    TIFFErrorHandlerExt oldWarnExt, oldErrExt;
    int mode;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    make_cstring(Data, Size, &offset, module1, sizeof(module1));
    make_cstring(Data, Size, &offset, module2, sizeof(module2));
    make_cstring(Data, Size, &offset, msg1, sizeof(msg1));
    make_cstring(Data, Size, &offset, msg2, sizeof(msg2));

    if (module1[0] == '\0') strcpy(module1, "mod1");
    if (module2[0] == '\0') strcpy(module2, "mod2");
    if (msg1[0] == '\0') strcpy(msg1, "message1");
    if (msg2[0] == '\0') strcpy(msg2, "message2");

    handle = (thandle_t)(uintptr_t)consume_u8(Data, Size, &offset);

    oldWarnExt = TIFFSetWarningHandlerExt(FuzzWarningHandlerExt);
    oldErrExt = TIFFSetErrorHandlerExt(FuzzErrorHandlerExt);

    TIFFWarning(module1, "%s", msg1);
    TIFFError(module1, "%s", msg1);
    TIFFWarningExt(handle, module1, "%s", msg1);
    TIFFErrorExt(handle, module1, "%s", msg1);

    mode = (int)consume_u8(Data, Size, &offset) % 4;
    switch (mode) {
        case 0:
            TIFFSetWarningHandlerExt(NULL);
            TIFFWarning(module2, "%s", msg2);
            TIFFWarningExt(handle, module2, "%s", msg2);
            break;
        case 1:
            TIFFSetErrorHandlerExt(NULL);
            TIFFError(module2, "%s", msg2);
            TIFFErrorExt(handle, module2, "%s", msg2);
            break;
        case 2:
            TIFFSetWarningHandlerExt(FuzzWarningHandlerExt);
            TIFFSetErrorHandlerExt(FuzzErrorHandlerExt);
            TIFFWarning(module2, "%s %d", msg2, (int)consume_u8(Data, Size, &offset));
            TIFFError(module2, "%s %d", msg2, (int)consume_u8(Data, Size, &offset));
            break;
        case 3:
            TIFFWarningExt((thandle_t)(uintptr_t)consume_u8(Data, Size, &offset),
                           module2, "%s %u", msg2, (unsigned)consume_u8(Data, Size, &offset));
            TIFFErrorExt((thandle_t)(uintptr_t)consume_u8(Data, Size, &offset),
                         module2, "%s %u", msg2, (unsigned)consume_u8(Data, Size, &offset));
            break;
    }

    TIFFSetWarningHandlerExt(oldWarnExt);
    TIFFSetErrorHandlerExt(oldErrExt);

    return 0;
}