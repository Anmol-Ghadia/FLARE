// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFStripSize64 at tif_strip.c:186:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFStripSize64 at tif_strip.c:186:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFVStripSize64 at tif_strip.c:76:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void set_basic_tags(TIFF *tif, const uint8_t *Data, size_t Size)
{
    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = 1;

    if (Size > 0)
        width = (uint32_t)Data[0] + 1;
    if (Size > 1)
        height = (uint32_t)Data[1] + 1;
    if (Size > 2)
        spp = (uint16_t)((Data[2] % 4) + 1);
    if (Size > 3)
    {
        switch (Data[3] % 4)
        {
            case 0:
                bps = 1;
                break;
            case 1:
                bps = 4;
                break;
            case 2:
                bps = 8;
                break;
            default:
                bps = 16;
                break;
        }
    }
    if (Size > 4)
        photometric =
            (Data[4] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    if (Size > 5)
        planar = (Data[5] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    if (Size > 6)
        rowsperstrip = (uint32_t)Data[6] + 1;
    if (Size > 7)
    {
        switch (Data[7] % 3)
        {
            case 0:
                compression = COMPRESSION_NONE;
                break;
            case 1:
                compression = COMPRESSION_PACKBITS;
                break;
            default:
                compression = COMPRESSION_LZW;
                break;
        }
    }

    if (photometric == PHOTOMETRIC_RGB && spp < 3)
        spp = 3;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    if (compression == COMPRESSION_LZW)
        TIFFSetField(tif, TIFFTAG_PREDICTOR, 1);
}

static tmsize_t choose_buf_size(uint64_t preferred, uint64_t fallback)
{
    const uint64_t kMax = 1U << 20;
    if (preferred > 0 && preferred < kMax)
        return (tmsize_t)preferred;
    if (fallback > 0 && fallback < kMax)
        return (tmsize_t)fallback;
    return (tmsize_t)4096;
}

static void exercise_read_apis(TIFF *tif, const uint8_t *Data, size_t Size)
{
    uint64_t strip_size64;
    uint32_t nstrips;
    uint32_t i;

    if (!tif)
        return;

    strip_size64 = TIFFStripSize64(tif);
    nstrips = TIFFNumberOfStrips(tif);

    (void)TIFFVStripSize64(tif, (uint32_t)-1);
    (void)TIFFVStripSize64(tif, 0);
    (void)TIFFVStripSize64(tif, 1);
    if (Size > 0)
        (void)TIFFVStripSize64(tif, (uint32_t)Data[0]);

    for (i = 0; i < nstrips && i < 8; i++)
    {
        int err = 0;
        uint64_t bc = TIFFGetStrileByteCountWithErr(tif, i, &err);
        tmsize_t outsize = choose_buf_size(err ? 0 : bc, strip_size64);
        void *outbuf = malloc((size_t)outsize);

        if (!outbuf)
            continue;

        (void)TIFFReadEncodedStrip(tif, i, outbuf, outsize);

        /* Guard TIFFReadFromUserBuffer against division by zero in
         * TIFFStartStrip() when stripsperimage is 0. Only call it when
         * TIFF reports at least one strip. */
        if (Size > 0 && nstrips > 0)
        {
            (void)TIFFReadFromUserBuffer(tif, i, (void *)Data, (tmsize_t)Size,
                                         outbuf, outsize);
        }

        free(outbuf);
    }

    if (nstrips > 0 && Size > 0)
    {
        uint8_t stackbuf[256];
        (void)TIFFReadFromUserBuffer(tif, 0, (void *)Data, (tmsize_t)Size,
                                     stackbuf, (tmsize_t)sizeof(stackbuf));
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    TIFF *tif;
    TIFF *tw;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (tif)
    {
        exercise_read_apis(tif, Data, Size);
        TIFFClose(tif);
    }

    tw = TIFFOpen("./dummy_file", "w");
    if (tw)
    {
        set_basic_tags(tw, Data, Size);

        /* Safe metadata-only exploration on a writer handle. Avoid
         * TIFFReadFromUserBuffer/TIFFReadEncodedStrip on write-only TIFF. */
        (void)TIFFStripSize64(tw);
        (void)TIFFNumberOfStrips(tw);
        (void)TIFFVStripSize64(tw, (uint32_t)-1);
        (void)TIFFVStripSize64(tw, 0);
        (void)TIFFVStripSize64(tw, 1);
        if (Size > 1)
            (void)TIFFVStripSize64(tw, (uint32_t)Data[1]);

        {
            uint32_t nstrips = TIFFNumberOfStrips(tw);
            uint32_t i;
            for (i = 0; i < nstrips && i < 4; i++)
            {
                int err = 0;
                (void)TIFFGetStrileByteCountWithErr(tw, i, &err);
            }
        }

        TIFFClose(tw);
    }

    return 0;
}