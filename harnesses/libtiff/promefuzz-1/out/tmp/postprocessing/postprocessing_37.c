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

static void FuzzWarningHandlerExt(thandle_t fd, const char *module, const char *fmt,
                                  va_list ap)
{
    (void)fd;
    (void)module;
    (void)fmt;
    (void)ap;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    const char *version = TIFFGetVersion();
    (void)version;

    TIFFErrorHandlerExt oldWarnExt = TIFFSetWarningHandlerExt(FuzzWarningHandlerExt);

    char module[64];
    char fmt[128];
    char arg1[64];
    char arg2[64];

    size_t mlen = 0, flen = 0, a1len = 0, a2len = 0;
    if (Size > 0)
    {
        mlen = Data[0] % (sizeof(module) - 1);
        if (Size > 1)
            flen = Data[1] % (sizeof(fmt) - 1);
        if (Size > 2)
            a1len = Data[2] % (sizeof(arg1) - 1);
        if (Size > 3)
            a2len = Data[3] % (sizeof(arg2) - 1);
    }

    memset(module, 0, sizeof(module));
    memset(fmt, 0, sizeof(fmt));
    memset(arg1, 0, sizeof(arg1));
    memset(arg2, 0, sizeof(arg2));

    size_t off = 4;
    if (off + mlen > Size) mlen = (off < Size) ? (Size - off) : 0;
    memcpy(module, Data + off, mlen);
    module[mlen] = '\0';
    off += mlen;

    if (off + flen > Size) flen = (off < Size) ? (Size - off) : 0;
    memcpy(fmt, Data + off, flen);
    fmt[flen] = '\0';
    off += flen;

    if (off + a1len > Size) a1len = (off < Size) ? (Size - off) : 0;
    memcpy(arg1, Data + off, a1len);
    arg1[a1len] = '\0';
    off += a1len;

    if (off + a2len > Size) a2len = (off < Size) ? (Size - off) : 0;
    memcpy(arg2, Data + off, a2len);
    arg2[a2len] = '\0';

    if (module[0] == '\0')
        memcpy(module, "fuzz", 5);
    if (fmt[0] == '\0')
        memcpy(fmt, "msg: %s %d", 11);

    thandle_t h1 = (thandle_t)(uintptr_t)Size;
    thandle_t h2 = (thandle_t)(uintptr_t)(Size ? Data[0] : 0);

    TIFFWarning(module, "%s %d", arg1, (int)Size);
    TIFFWarningExt(h1, module, "%s %d", arg2, (int)(Size & 0x7fffffff));
    TIFFError(module, "%s %d", arg1, (int)(Size ^ 0x55));
    TIFFErrorExt(h2, module, "%s %d", arg2, (int)(Size ^ 0xaa));

    TIFFWarning(module, "%s", fmt);
    TIFFWarningExt(h1, module, "%s", fmt);
    TIFFError(module, "%s", fmt);
    TIFFErrorExt(h2, module, "%s", fmt);

    if (Size > 0 && (Data[0] & 1))
    {
        (void)TIFFSetWarningHandlerExt(NULL);
        TIFFWarning(module, "%s %d", arg1, 123);
        TIFFWarningExt(h1, module, "%s %d", arg2, 456);
        (void)TIFFSetWarningHandlerExt(FuzzWarningHandlerExt);
    }

    (void)TIFFSetWarningHandlerExt(oldWarnExt);
    return 0;
}