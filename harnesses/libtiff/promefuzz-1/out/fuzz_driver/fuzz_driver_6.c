// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Data && Size > 0) {
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

    tmsize_t inbuf_size = (tmsize_t)(Size > 4 ? Size - 4 : 0);
    void *inbuf = (void *)(uintptr_t)(Data + (Size > 4 ? 4 : 0));

    tmsize_t outsize = 0;
    if (Size > 0) {
        outsize = (tmsize_t)((Size % 4096) + 1);
    }
    void *outbuf = NULL;
    if (outsize > 0) {
        outbuf = malloc((size_t)outsize);
        if (!outbuf) {
            return 0;
        }
        memset(outbuf, 0, (size_t)outsize);
    }

    TIFF *tif1 = TIFFOpen("./dummy_file", "r");
    if (tif1) {
        (void)TIFFReadFromUserBuffer(tif1, strile, inbuf, inbuf_size, outbuf, outsize);
        TIFFClose(tif1);
    }

    TIFF *tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2) {
        tmsize_t enc_size = outsize;
        if (enc_size <= 0) {
            enc_size = 1;
        }
        void *enc_buf = malloc((size_t)enc_size);
        if (enc_buf) {
            memset(enc_buf, 0, (size_t)enc_size);
            (void)TIFFReadEncodedStrip(tif2, strile, enc_buf, enc_size);
            free(enc_buf);
        }
        TIFFClose(tif2);
    }

    TIFF *tif3 = TIFFOpen("./dummy_file", "r");
    if (tif3) {
        int err1 = 0;
        int err2 = 0;
        (void)TIFFGetStrileOffsetWithErr(tif3, strile, &err1);
        (void)TIFFGetStrileByteCountWithErr(tif3, strile, &err2);

        if (Size >= 8) {
            uint32_t strile2 = ((uint32_t)Data[Size - 4] << 24) |
                               ((uint32_t)Data[Size - 3] << 16) |
                               ((uint32_t)Data[Size - 2] << 8) |
                               (uint32_t)Data[Size - 1];
            (void)TIFFGetStrileOffsetWithErr(tif3, strile2, &err1);
            (void)TIFFGetStrileByteCountWithErr(tif3, strile2, &err2);
        }

        TIFFClose(tif3);
    }

    free(outbuf);
    return 0;
}