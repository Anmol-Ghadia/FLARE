// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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

static long read_long_le(const uint8_t *p, size_t n) {
    long v = 0;
    size_t i;
    size_t lim = n < sizeof(long) ? n : sizeof(long);
    for (i = 0; i < lim; i++) {
        v |= ((long)p[i]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif = NULL;
    long flags = 0;
    uint16_t dirn = 0;
    int mode_selector = 0;

    fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    if (Size > 0) {
        mode_selector = Data[0] % 3;
    }

    if (mode_selector == 0) {
        tif = TIFFOpen("./dummy_file", "r");
    } else if (mode_selector == 1) {
        tif = TIFFOpen("./dummy_file", "r+");
        if (!tif) {
            tif = TIFFOpen("./dummy_file", "w");
        }
    } else {
        tif = TIFFOpen("./dummy_file", "w");
    }

    if (!tif) {
        return 0;
    }

    if (Size >= 9) {
        flags = read_long_le(Data + 1, Size - 1);
    } else if (Size > 1) {
        flags = read_long_le(Data + 1, Size - 1);
    }

    if (Size >= 3) {
        dirn = (uint16_t)(Data[1] | ((uint16_t)Data[2] << 8));
    } else if (Size >= 2) {
        dirn = Data[1];
    }

    (void)TIFFCreateDirectory(tif);
    (void)TIFFDeferStrileArrayWriting(tif);
    TIFFPrintDirectory(tif, stdout, flags);
    (void)TIFFSetDirectory(tif, dirn);
    TIFFPrintDirectory(tif, stdout, ~flags);
    TIFFFreeDirectory(tif);

    (void)TIFFCreateDirectory(tif);
    (void)TIFFDeferStrileArrayWriting(tif);
    (void)TIFFSetDirectory(tif, (uint16_t)(dirn ^ 0xFFFFu));
    TIFFPrintDirectory(tif, stdout, 0);
    TIFFFreeDirectory(tif);

    TIFFClose(tif);
    return 0;
}