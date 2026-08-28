#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

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
    if (strip_size < 0) {
        strip_size = 0;
    }

    void *buf1 = _TIFFmalloc(strip_size);

    uint32_t num_strips = TIFFNumberOfStrips(tif);

    if (buf1 != NULL) {
        if (num_strips == 0) {
            (void)TIFFReadEncodedStrip(tif, 0, buf1, strip_size);
        } else {
            uint32_t strip = 0;
            if (Size > 0) {
                strip = ((uint32_t)Data[0]) % num_strips;
            }
            (void)TIFFReadEncodedStrip(tif, strip, buf1, strip_size);
            (void)TIFFReadEncodedStrip(tif, strip, buf1, (tmsize_t)-1);
        }
    } else {
        if (num_strips > 0) {
            (void)TIFFReadEncodedStrip(tif, 0, NULL, strip_size);
        }
    }

    _TIFFfree(buf1);

    TIFFClose(tif);

    _TIFFfree(NULL);

    return 0;
}