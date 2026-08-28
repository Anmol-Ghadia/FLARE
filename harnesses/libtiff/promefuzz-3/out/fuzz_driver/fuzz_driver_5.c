// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
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
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileOffsetWithErr at tif_dirread.c:6396:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    uint32_t strile = 0;
    if (Size >= 4) {
        strile = ((uint32_t)Data[0] << 24) |
                 ((uint32_t)Data[1] << 16) |
                 ((uint32_t)Data[2] << 8) |
                 (uint32_t)Data[3];
    }

    tmsize_t inbuf_size = 0;
    tmsize_t outbuf_size = 0;
    if (Size > 4) {
        inbuf_size = (tmsize_t)((Size - 4) / 2);
        outbuf_size = (tmsize_t)(Size - 4 - (size_t)inbuf_size);
    }

    void *inbuf = NULL;
    void *outbuf = NULL;
    if (inbuf_size > 0) {
        inbuf = malloc((size_t)inbuf_size);
        if (inbuf) {
            memcpy(inbuf, Data + 4, (size_t)inbuf_size);
        }
    }
    if (outbuf_size <= 0) {
        outbuf_size = 1;
    }
    outbuf = malloc((size_t)outbuf_size);
    if (outbuf) {
        memset(outbuf, 0, (size_t)outbuf_size);
    }

    TIFF *tif1 = TIFFOpen("./dummy_file", "r");
    if (tif1) {
        if (inbuf && outbuf) {
            (void)TIFFReadFromUserBuffer(tif1, strile, inbuf, inbuf_size, outbuf, outbuf_size);
            if (inbuf_size > 1) {
                (void)TIFFReadFromUserBuffer(tif1, strile ^ 1U, inbuf, inbuf_size - 1, outbuf, outbuf_size);
            }
            (void)TIFFReadFromUserBuffer(tif1, 0, inbuf, inbuf_size, outbuf, outbuf_size);
        }
        TIFFClose(tif1);
    }

    TIFF *tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        tmsize_t enc_size = outbuf_size;
        if (outbuf) {
            (void)TIFFReadEncodedStrip(tif2, strile, outbuf, enc_size);
            if (enc_size > 1) {
                (void)TIFFReadEncodedStrip(tif2, strile ^ 1U, outbuf, enc_size - 1);
            }
            (void)TIFFReadEncodedStrip(tif2, 0, outbuf, (tmsize_t)-1);
        }
        TIFFClose(tif2);
    }

    TIFF *tif3 = TIFFOpen("./dummy_file", "r");
    if (tif3) {
        int err1 = 0;
        int err2 = 0;
        (void)TIFFGetStrileOffsetWithErr(tif3, strile, &err1);
        (void)TIFFGetStrileByteCountWithErr(tif3, strile, &err2);

        err1 = 0;
        err2 = 0;
        (void)TIFFGetStrileOffsetWithErr(tif3, strile ^ 1U, &err1);
        (void)TIFFGetStrileByteCountWithErr(tif3, strile ^ 1U, &err2);

        TIFFClose(tif3);
    }

    free(inbuf);
    free(outbuf);
    return 0;
}