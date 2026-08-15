// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_4(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif == NULL) {
        return 0;
    }

    tmsize_t strip_size = TIFFStripSize(tif);
    void *buf = NULL;
    if (strip_size > 0) {
        buf = _TIFFmalloc(strip_size);
    } else {
        buf = _TIFFmalloc(0);
    }

    uint32_t num_strips = TIFFNumberOfStrips(tif);

    if (buf != NULL && num_strips > 0) {
        uint32_t strip = 0;
        if (Size >= 4) {
            strip = ((uint32_t)Data[0] << 24) |
                    ((uint32_t)Data[1] << 16) |
                    ((uint32_t)Data[2] << 8) |
                    (uint32_t)Data[3];
            strip %= num_strips;
        }

        tmsize_t read_size = strip_size;
        if (Size >= 12) {
            uint64_t v = 0;
            for (size_t i = 4; i < 12; i++) {
                v = (v << 8) | Data[i];
            }
            if (strip_size > 0) {
                read_size = (tmsize_t)(v % (uint64_t)(strip_size + 1));
            }
        }

        (void)TIFFReadEncodedStrip(tif, strip, buf, read_size);

        if (num_strips > 1) {
            uint32_t strip2 = num_strips - 1;
            (void)TIFFReadEncodedStrip(tif, strip2, buf, strip_size);
        }
    } else {
        (void)TIFFReadEncodedStrip(tif, 0, buf, strip_size);
    }

    _TIFFfree(buf);
    TIFFClose(tif);
    _TIFFfree(NULL);

    return 0;
}