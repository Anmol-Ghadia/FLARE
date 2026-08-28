// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetStrileOffsetWithErr at tif_dirread.c:6396:10 in tiffio.h
// TIFFGetStrileOffsetWithErr at tif_dirread.c:6396:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tiffio.h"

static uint16_t rd16le(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32le(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint32_t choose_strile(const uint8_t *Data, size_t Size, uint32_t fallback) {
    if (Size >= 4) {
        return rd32le(Data);
    }
    if (Size >= 2) {
        return rd16le(Data);
    }
    if (Size >= 1) {
        return Data[0];
    }
    return fallback;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    uint32_t strile0 = choose_strile(Data, Size, 0);
    uint32_t strile1 = (Size >= 8) ? rd32le(Data + 4) : (strile0 ^ 1U);

    TIFF *tif1 = TIFFOpen("./dummy_file", "r");
    if (tif1) {
        tmsize_t outsize = (Size > 0 && Size < 1 << 20) ? (tmsize_t)Size : (tmsize_t)4096;
        if (outsize <= 0) {
            outsize = 1;
        }
        void *outbuf = malloc((size_t)outsize);
        if (outbuf) {
            (void)TIFFReadFromUserBuffer(tif1, strile0, (void *)Data, (tmsize_t)Size, outbuf, outsize);
            free(outbuf);
        } else {
            (void)TIFFReadFromUserBuffer(tif1, strile0, (void *)Data, (tmsize_t)Size, NULL, 0);
        }
        TIFFClose(tif1);
    }

    TIFF *tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        tmsize_t bufsize = (Size > 0 && Size < 1 << 20) ? (tmsize_t)Size : (tmsize_t)8192;
        if (bufsize <= 0) {
            bufsize = 1;
        }
        void *buf = malloc((size_t)bufsize);
        if (buf) {
            (void)TIFFReadEncodedStrip(tif2, strile1, buf, (tmsize_t)-1);
            (void)TIFFReadEncodedStrip(tif2, strile0, buf, bufsize);
            free(buf);
        } else {
            (void)TIFFReadEncodedStrip(tif2, strile1, NULL, 0);
        }
        TIFFClose(tif2);
    }

    TIFF *tif3 = TIFFOpen("./dummy_file", "r");
    if (tif3) {
        int err0 = 0;
        int err1 = 0;
        (void)TIFFGetStrileOffsetWithErr(tif3, strile0, &err0);
        (void)TIFFGetStrileOffsetWithErr(tif3, strile1, NULL);
        (void)TIFFGetStrileByteCountWithErr(tif3, strile0, &err1);
        (void)TIFFGetStrileByteCountWithErr(tif3, strile1, NULL);
        TIFFClose(tif3);
    }

    remove("./dummy_file");
    return 0;
}