// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFSetupStrips at tif_write.c:523:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFReadBufferSetup at tif_read.c:1279:1 in tiffio.h
// TIFFReadBufferSetup at tif_read.c:1279:1 in tiffio.h
// TIFFReadBufferSetup at tif_read.c:1279:1 in tiffio.h
// TIFFSetupStrips at tif_write.c:523:1 in tiffio.h
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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static tmsize_t choose_size_from_data(const uint8_t *Data, size_t Size, size_t off) {
    if (Size <= off) return 0;
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && off + i < Size; i++) {
        v = (v << 8) ^ Data[off + i];
    }
    return (tmsize_t)(v % 65536);
}

int LLVMFuzzerTestOneInput_67(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        /* Exercise TIFFDataWidth even if file is not a valid TIFF */
        int dt;
        for (dt = -4; dt < 32; dt++) {
            (void)TIFFDataWidth((TIFFDataType)dt);
        }
        return 0;
    }

    char errbuf[1024];
    memset(errbuf, 0, sizeof(errbuf));
    (void)TIFFRGBAImageOK(tif, errbuf);

    /* Exercise TIFFDataWidth across a broad range of enum-like values */
    {
        int dt;
        for (dt = -4; dt < 64; dt++) {
            (void)TIFFDataWidth((TIFFDataType)dt);
        }
    }

    /* Try different read buffer setup modes */
    {
        tmsize_t sz1 = choose_size_from_data(Data, Size, 0);
        tmsize_t sz2 = choose_size_from_data(Data, Size, 8);
        tmsize_t sz3 = choose_size_from_data(Data, Size, 16);

        (void)TIFFReadBufferSetup(tif, NULL, sz1);

        if (sz2 > 0) {
            void *buf = malloc((size_t)sz2);
            if (buf) {
                memset(buf, 0xA5, (size_t)sz2);
                (void)TIFFReadBufferSetup(tif, buf, sz2);
                free(buf);
            }
        }

        (void)TIFFReadBufferSetup(tif, NULL, sz3);
    }

    /* Attempt to initialize strip/tile bookkeeping */
    (void)TIFFSetupStrips(tif);

    /* Try reading from user-supplied buffers with varied parameters */
    {
        uint32_t strile = 0;
        if (Size >= 4) {
            strile = ((uint32_t)Data[0] << 24) |
                     ((uint32_t)Data[1] << 16) |
                     ((uint32_t)Data[2] << 8) |
                     (uint32_t)Data[3];
        }

        tmsize_t insize1 = (tmsize_t)(Size > 4096 ? 4096 : Size);
        tmsize_t outsize1 = choose_size_from_data(Data, Size, 24) % 8192;
        tmsize_t outsize2 = choose_size_from_data(Data, Size, 32) % 8192;

        void *out1 = NULL;
        void *out2 = NULL;

        if (outsize1 > 0) out1 = malloc((size_t)outsize1);
        if (outsize2 > 0) out2 = malloc((size_t)outsize2);

        if (out1) {
            memset(out1, 0, (size_t)outsize1);
            (void)TIFFReadFromUserBuffer(tif, strile, (void *)Data, insize1, out1, outsize1);
        }

        if (out2) {
            memset(out2, 0x5A, (size_t)outsize2);
            (void)TIFFReadFromUserBuffer(tif, strile ^ 1U, (void *)Data, (tmsize_t)Size, out2, outsize2);
        }

        if (out1) free(out1);
        if (out2) free(out2);
    }

    /* Iterate directories to explore parser state transitions */
    {
        int i;
        for (i = 0; i < 16; i++) {
            if (!TIFFReadDirectory(tif))
                break;
            memset(errbuf, 0, sizeof(errbuf));
            (void)TIFFRGBAImageOK(tif, errbuf);
            (void)TIFFSetupStrips(tif);
        }
    }

    TIFFClose(tif);
    return 0;
}