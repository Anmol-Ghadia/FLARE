// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFGetBitRevTable at tif_swab.c:278:1 in tiffio.h
// TIFFGetBitRevTable at tif_swab.c:278:1 in tiffio.h
// TIFFGetBitRevTable at tif_swab.c:278:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsMSB2LSB at tif_open.c:658:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_64(const uint8_t *Data, size_t Size) {
    const unsigned char *tbl0 = TIFFGetBitRevTable(0);
    const unsigned char *tbl1 = TIFFGetBitRevTable(1);
    const unsigned char *tbl2 = TIFFGetBitRevTable(Size ? (int)(Data[0] & 1) : 0);
    volatile unsigned int sink = 0;
    if (tbl0) sink ^= tbl0[0];
    if (tbl1) sink ^= tbl1[255];
    if (tbl2 && Size) sink ^= tbl2[Data[0]];

    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) return 0;

    sink ^= (unsigned int)TIFFIsMSB2LSB(tif);

    int err = 0;
    uint64_t bc0 = TIFFGetStrileByteCountWithErr(tif, 0, &err);
    sink ^= (unsigned int)(bc0 & 0xFFU);
    sink ^= (unsigned int)(err & 0xFF);

    uint32_t idx1 = 0;
    uint32_t idx2 = 1;
    if (Size >= 4) idx1 = rd32(Data);
    if (Size >= 8) idx2 = rd32(Data + 4);

    err = 0;
    uint64_t bc1 = TIFFGetStrileByteCountWithErr(tif, idx1, &err);
    sink ^= (unsigned int)(bc1 & 0xFFU);
    sink ^= (unsigned int)(err & 0xFF);

    err = 0;
    uint64_t bc2 = TIFFGetStrileByteCountWithErr(tif, idx2, &err);
    sink ^= (unsigned int)(bc2 & 0xFFU);
    sink ^= (unsigned int)(err & 0xFF);

    tmsize_t outsize = 0;
    if (Size >= 2) {
        outsize = (tmsize_t)(rd16(Data) % 65536U);
    } else {
        outsize = (tmsize_t)(Size % 1024U);
    }
    if (outsize < 0) outsize = 0;

    void *outbuf = NULL;
    if (outsize > 0) {
        outbuf = malloc((size_t)outsize);
        if (outbuf) memset(outbuf, 0, (size_t)outsize);
    }

    void *inbuf = NULL;
    tmsize_t insize = (tmsize_t)Size;
    if (Size > 0) {
        inbuf = malloc(Size);
        if (inbuf) memcpy(inbuf, Data, Size);
    }

    if (inbuf || Size == 0) {
        (void)TIFFReadFromUserBuffer(tif, 0, inbuf, insize, outbuf, outsize);
        (void)TIFFReadFromUserBuffer(tif, idx1, inbuf, insize, outbuf, outsize);
        (void)TIFFReadFromUserBuffer(tif, idx2, inbuf, insize, outbuf, outsize);
    }

    tmsize_t readsz1 = outsize;
    tmsize_t readsz2 = (outsize > 0) ? outsize / 2 : (tmsize_t)-1;
    tmsize_t readsz3 = (outsize > 0) ? (outsize + 16) : 16;

    if (outbuf || outsize == 0) {
        (void)TIFFReadEncodedStrip(tif, 0, outbuf, readsz1);
        (void)TIFFReadEncodedStrip(tif, idx1, outbuf, readsz2);
        (void)TIFFReadEncodedStrip(tif, idx2, outbuf, readsz3);

        (void)TIFFReadEncodedTile(tif, 0, outbuf, readsz1);
        (void)TIFFReadEncodedTile(tif, idx1, outbuf, readsz2);
        (void)TIFFReadEncodedTile(tif, idx2, outbuf, readsz3);
    }

    free(inbuf);
    free(outbuf);
    TIFFClose(tif);

    (void)sink;
    return 0;
}