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

#include "tiffio.h"

static void fuzz_error_handler(const char *module, const char *fmt, va_list ap)
{
    (void)module;
    (void)fmt;
    va_list aq;
    va_copy(aq, ap);
    (void)vfprintf(stderr, fmt ? fmt : "", aq);
    va_end(aq);
}

static const char *pick_string(const uint8_t *Data, size_t Size, size_t *offset)
{
    static char buf[256];
    size_t i = 0;

    if (*offset >= Size)
        return NULL;

    while (*offset < Size && i + 1 < sizeof(buf))
    {
        uint8_t c = Data[*offset];
        (*offset)++;
        if (c == 0)
            break;
        if (c < 32 || c > 126)
            c = (uint8_t)('A' + (c % 26));
        buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return buf;
}

static unsigned consume_u32(const uint8_t *Data, size_t Size, size_t *offset)
{
    unsigned v = 0;
    size_t i;
    for (i = 0; i < 4; i++)
    {
        v <<= 8;
        if (*offset < Size)
            v |= Data[(*offset)++];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    size_t offset = 0;
    FILE *fp;
    const char *version;
    const char *module1;
    const char *module2;
    const char *fmt1;
    const char *fmt2;
    const char *fmt3;
    const char *fmt4;
    TIFFErrorHandler old_handler;
    thandle_t handle1;
    thandle_t handle2;
    unsigned a, b;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    version = TIFFGetVersion();
    if (version == NULL)
        return 0;

    old_handler = TIFFSetErrorHandler(fuzz_error_handler);

    module1 = pick_string(Data, Size, &offset);
    module2 = pick_string(Data, Size, &offset);
    fmt1 = pick_string(Data, Size, &offset);
    fmt2 = pick_string(Data, Size, &offset);
    fmt3 = pick_string(Data, Size, &offset);
    fmt4 = pick_string(Data, Size, &offset);

    if (fmt1 == NULL || fmt1[0] == '\0')
        fmt1 = "err:%u:%s";
    if (fmt2 == NULL || fmt2[0] == '\0')
        fmt2 = "warn:%d:%s";
    if (fmt3 == NULL || fmt3[0] == '\0')
        fmt3 = "ext-err:%u:%s";
    if (fmt4 == NULL || fmt4[0] == '\0')
        fmt4 = "ext-warn:%d:%s";

    a = consume_u32(Data, Size, &offset);
    b = consume_u32(Data, Size, &offset);

    handle1 = (thandle_t)(uintptr_t)a;
    handle2 = (thandle_t)(uintptr_t)b;

    TIFFError(module1, "%s", fmt1);
    TIFFWarning(module2, "%s", fmt2);

    TIFFError(module1, "num=%u str=%s", a, fmt1);
    TIFFWarning(module2, "num=%d str=%s", (int)b, fmt2);

    TIFFErrorExt(handle1, module1, "%s", fmt3);
    TIFFWarningExt(handle2, module2, "%s", fmt4);

    TIFFErrorExt(handle1, module1, "h=%p a=%u s=%s", handle1, a, fmt3);
    TIFFWarningExt(handle2, module2, "h=%p b=%d s=%s", handle2, (int)b, fmt4);

    (void)TIFFSetErrorHandler(NULL);
    TIFFError(module1, "disabled handler test %u", a);
    TIFFErrorExt(handle1, module2, "disabled ext handler test %u", b);

    (void)TIFFSetErrorHandler(fuzz_error_handler);
    TIFFError("TIFFGetVersion", "version=%s", version);
    TIFFWarning("TIFFGetVersion", "version=%s", version);

    (void)TIFFSetErrorHandler(old_handler);
    return 0;
}