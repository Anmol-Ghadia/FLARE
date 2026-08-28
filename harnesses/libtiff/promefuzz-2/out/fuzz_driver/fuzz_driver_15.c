// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// _TIFFmemcmp at tif_unix.c:350:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFErrorExt at tif_error.c:65:1 in tiffio.h
// TIFFWarningExt at tif_warning.c:65:1 in tiffio.h
// TIFFError at tif_error.c:49:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFErrorExt at tif_error.c:65:1 in tiffio.h
// TIFFWarningExt at tif_warning.c:65:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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

static size_t min_size(size_t a, size_t b) {
    return a < b ? a : b;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    {
        size_t half = Size / 2;
        const void *p1 = Data;
        const void *p2 = Data + half;
        tmsize_t cmp_len = (tmsize_t)min_size(half, Size - half);
        (void)_TIFFmemcmp(p1, p2, cmp_len);

        if (Size > 0) {
            (void)_TIFFmemcmp(Data, Data, (tmsize_t)Size);
        }

        if (Size >= 2) {
            (void)_TIFFmemcmp(Data, Data + 1, (tmsize_t)(Size - 1));
        }
    }

    {
        char module[64];
        char message[256];
        size_t mod_len = min_size(Size, sizeof(module) - 1);
        size_t msg_len = min_size(Size, sizeof(message) - 1);

        if (mod_len > 0) {
            memcpy(module, Data, mod_len);
        }
        module[mod_len] = '\0';

        if (msg_len > 0) {
            memcpy(message, Data, msg_len);
        }
        message[msg_len] = '\0';

        TIFFError(module[0] ? module : NULL, "%s", message);
        TIFFWarning(module[0] ? module : NULL, "%s", message);
        TIFFErrorExt((thandle_t)(uintptr_t)Size, module[0] ? module : NULL, "%s", message);
        TIFFWarningExt((thandle_t)(uintptr_t)Data, module[0] ? module : NULL, "%s", message);

        if (Size >= 4) {
            unsigned v0 = Data[0];
            unsigned v1 = Data[1];
            unsigned v2 = Data[2];
            unsigned v3 = Data[3];
            TIFFError("fuzz", "vals:%u:%u:%u:%u", v0, v1, v2, v3);
            TIFFWarning("fuzz", "mix:%u-%u", v0, v1);
            TIFFErrorExt((thandle_t)(uintptr_t)v2, "fuzz_ext", "triple:%u:%u:%u", v0, v1, v2);
            TIFFWarningExt((thandle_t)(uintptr_t)v3, "fuzz_warn_ext", "single:%u", v3);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "r");
        if (tif != NULL) {
            char emsg[1024];
            memset(emsg, 0, sizeof(emsg));
            (void)TIFFRGBAImageOK(tif, emsg);

            memset(emsg, 'A', sizeof(emsg) - 1);
            emsg[sizeof(emsg) - 1] = '\0';
            (void)TIFFRGBAImageOK(tif, emsg);

            TIFFClose(tif);
        }
    }

    remove("./dummy_file");
    return 0;
}