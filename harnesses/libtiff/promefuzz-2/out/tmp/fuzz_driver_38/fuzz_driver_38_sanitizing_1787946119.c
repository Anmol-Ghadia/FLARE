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
#include <unistd.h>
#include <fcntl.h>
#include "tiffio.h"

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8);
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t rd64(const uint8_t *p)
{
    return (uint64_t)rd32(p) | ((uint64_t)rd32(p + 4) << 32);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
    {
        tif = TIFFOpen("./dummy_file", "w");
        if (!tif)
            return 0;

        uint32_t width = 1, height = 1;
        uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
        uint16_t config = PLANARCONFIG_CONTIG;
        uint16_t compression = COMPRESSION_NONE;
        uint16_t rowsperstrip = 1;
        unsigned char pixel = (Size > 0) ? Data[0] : 0;

        (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, config);
        (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        (void)TIFFWriteScanline(tif, &pixel, 0, 0);
        TIFFWriteDirectory(tif);
        TIFFClose(tif);

        tif = TIFFOpen("./dummy_file", "r");
        if (!tif)
            return 0;
    }

    if (Size >= 8)
    {
        toff_t off1 = (toff_t)rd64(Data);
        TIFFSetWriteOffset(tif, off1);
    }
    else if (Size >= 4)
    {
        toff_t off2 = (toff_t)rd32(Data);
        TIFFSetWriteOffset(tif, off2);
    }
    else
    {
        TIFFSetWriteOffset(tif, (toff_t)0);
    }

    int oldfd = TIFFSetFileno(tif, -1);
    (void)TIFFSetFileno(tif, oldfd);

    (void)TIFFCurrentDirOffset(tif);

    uint32_t indices[8];
    size_t nindices = 0;

    indices[nindices++] = 0;
    indices[nindices++] = 1;
    indices[nindices++] = UINT32_MAX;

    if (Size >= 4)
        indices[nindices++] = rd32(Data);
    if (Size >= 8)
        indices[nindices++] = (uint32_t)rd64(Data);
    if (Size >= 2)
        indices[nindices++] = rd16(Data);
    if (Size >= 12)
        indices[nindices++] = rd32(Data + 4);
    if (Size >= 16)
        indices[nindices++] = rd32(Data + 8);

    for (size_t i = 0; i < nindices; ++i)
    {
        int err = 0;
        volatile uint64_t a = TIFFGetStrileOffset(tif, indices[i]);
        volatile uint64_t b = TIFFGetStrileOffsetWithErr(tif, indices[i], &err);
        volatile uint64_t c = TIFFGetStrileByteCountWithErr(tif, indices[i], &err);
        (void)a;
        (void)b;
        (void)c;
    }

    int err2 = 0;
    (void)TIFFGetStrileOffsetWithErr(tif, 0, NULL);
    (void)TIFFGetStrileByteCountWithErr(tif, 0, NULL);
    (void)TIFFGetStrileOffsetWithErr(tif, UINT32_MAX, &err2);
    (void)TIFFGetStrileByteCountWithErr(tif, UINT32_MAX, &err2);

    if (TIFFReadDirectory(tif))
    {
        (void)TIFFCurrentDirOffset(tif);
        (void)TIFFGetStrileOffset(tif, 0);
        (void)TIFFGetStrileOffsetWithErr(tif, 0, &err2);
        (void)TIFFGetStrileByteCountWithErr(tif, 0, &err2);
    }

    TIFFClose(tif);
    unlink("./dummy_file");
    return 0;
}