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

static uint32_t read_u32(const uint8_t *p, size_t n) {
    uint32_t v = 0;
    size_t i;
    if (n > 4) n = 4;
    for (i = 0; i < n; i++) {
        v = (v << 8) | p[i];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif = NULL;
    uint8_t *tmp = NULL;
    uint8_t *buf = NULL;
    uint8_t *outbuf = NULL;
    uint8_t *inbuf = NULL;
    tmsize_t ret;
    tmsize_t alloc_size;
    uint32_t idx1 = 0, idx2 = 0;
    size_t header = 0;

    fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        if (Size > 0) {
            tmp = (uint8_t *)malloc(Size);
            if (tmp) {
                memcpy(tmp, Data, Size);
                TIFFReverseBits(tmp, (tmsize_t)Size);
                free(tmp);
            }
        }
        (void)TIFFGetBitRevTable(0);
        (void)TIFFGetBitRevTable(1);
        return 0;
    }

    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFGetBitRevTable(0);
    (void)TIFFGetBitRevTable(1);

    if (Size > 0) {
        tmp = (uint8_t *)malloc(Size);
        if (tmp) {
            memcpy(tmp, Data, Size);
            TIFFReverseBits(tmp, (tmsize_t)Size);
            if (Size > 1) {
                TIFFReverseBits(tmp, (tmsize_t)(Size / 2));
            }
            free(tmp);
            tmp = NULL;
        }
    } else {
        TIFFReverseBits(NULL, 0);
    }

    header = Size > 8 ? 8 : Size;
    if (header >= 4) {
        idx1 = read_u32(Data, 4);
    }
    if (header >= 8) {
        idx2 = read_u32(Data + 4, 4);
    } else {
        idx2 = idx1 ^ 1U;
    }

    alloc_size = (Size == 0) ? 1 : (tmsize_t)Size;
    buf = (uint8_t *)malloc((size_t)alloc_size);
    outbuf = (uint8_t *)malloc((size_t)alloc_size);
    inbuf = (uint8_t *)malloc((size_t)alloc_size);

    if (inbuf && Size > 0) {
        memcpy(inbuf, Data, Size);
    } else if (inbuf) {
        inbuf[0] = 0;
    }

    if (buf) {
        memset(buf, 0, (size_t)alloc_size);
        ret = TIFFReadEncodedStrip(tif, idx1, buf, alloc_size);
        (void)ret;
        ret = TIFFReadEncodedStrip(tif, idx2, buf, (tmsize_t)-1);
        (void)ret;
        ret = TIFFReadEncodedStrip(tif, 0, buf, alloc_size > 16 ? 16 : alloc_size);
        (void)ret;
    }

    if (buf) {
        memset(buf, 0, (size_t)alloc_size);
        ret = TIFFReadEncodedTile(tif, idx1, buf, alloc_size);
        (void)ret;
        ret = TIFFReadEncodedTile(tif, idx2, buf, (tmsize_t)-1);
        (void)ret;
        ret = TIFFReadEncodedTile(tif, 0, buf, alloc_size > 32 ? 32 : alloc_size);
        (void)ret;
    }

    if (inbuf && outbuf) {
        memset(outbuf, 0, (size_t)alloc_size);
        (void)TIFFReadFromUserBuffer(tif, idx1, inbuf, alloc_size, outbuf, alloc_size);
        (void)TIFFReadFromUserBuffer(tif, idx2, inbuf, alloc_size > 8 ? alloc_size - 8 : alloc_size,
                                     outbuf, alloc_size);
        (void)TIFFReadFromUserBuffer(tif, 0, inbuf, alloc_size, outbuf,
                                     alloc_size > 4 ? alloc_size / 2 : alloc_size);
    }

    free(inbuf);
    free(outbuf);
    free(buf);
    TIFFClose(tif);
    return 0;
}