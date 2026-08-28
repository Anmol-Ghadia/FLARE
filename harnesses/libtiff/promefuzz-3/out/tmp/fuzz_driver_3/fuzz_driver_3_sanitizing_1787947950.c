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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    tmsize_t strip_size = TIFFStripSize(tif);

    void *buf1 = NULL;
    if (strip_size > 0) {
        buf1 = _TIFFmalloc(strip_size);
    } else {
        buf1 = _TIFFmalloc(0);
    }

    uint32_t nstrips = TIFFNumberOfStrips(tif);

    if (buf1 && nstrips > 0) {
        uint32_t strip = 0;
        if (Size >= 4) {
            strip = ((uint32_t)Data[0] << 24) |
                    ((uint32_t)Data[1] << 16) |
                    ((uint32_t)Data[2] << 8) |
                    (uint32_t)Data[3];
            strip %= nstrips;
        }

        tmsize_t read_size = strip_size;
        if (Size >= 12) {
            uint64_t v = 0;
            memcpy(&v, Data + 4, (Size - 4) >= sizeof(v) ? sizeof(v) : (Size - 4));
            if (strip_size > 0) {
                read_size = (tmsize_t)(v % ((uint64_t)strip_size + 1));
            }
        }

        (void)TIFFReadEncodedStrip(tif, strip, buf1, read_size);

        if (nstrips > 1) {
            uint32_t strip2 = (strip + 1) % nstrips;
            (void)TIFFReadEncodedStrip(tif, strip2, buf1, strip_size);
        }

        (void)TIFFReadEncodedStrip(tif, strip, buf1, (tmsize_t)-1);
    } else {
        (void)TIFFReadEncodedStrip(tif, 0, buf1, strip_size);
    }

    _TIFFfree(buf1);

    TIFFClose(tif);

    _TIFFfree(NULL);

    return 0;
}