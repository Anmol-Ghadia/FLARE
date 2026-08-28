#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tiffio.h"
#include "tiffio.hxx"

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void setup_minimal_image_tags(TIFF *tif, const uint8_t *Data, size_t Size)
{
    uint32_t width = 1 + (Size > 0 ? Data[0] : 0);
    uint32_t height = 1 + (Size > 1 ? Data[1] : 0);
    uint16_t spp = 1;
    uint16_t bps = 8;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = height ? height : 1;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
}

static void write_some_pixels(TIFF *tif, const uint8_t *Data, size_t Size)
{
    unsigned char buf[256];
    uint32_t row;
    uint32_t height = 1 + (Size > 1 ? Data[1] : 0);
    uint32_t width = 1 + (Size > 0 ? Data[0] : 0);

    if (width > sizeof(buf))
        width = (uint32_t)sizeof(buf);

    memset(buf, 0, sizeof(buf));
    if (Size > 2)
    {
        size_t copy = Size - 2;
        if (copy > width)
            copy = width;
        memcpy(buf, Data + 2, copy);
    }

    for (row = 0; row < height; row++)
        (void)TIFFWriteScanline(tif, buf, row, 0);
}

static const TIFFFieldArray *make_empty_custom_field_array(void)
{
    static TIFFFieldArray arr = {
        (TIFFFieldArrayType)0,
        0,
        0,
        NULL
    };
    return &arr;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    uint64_t diroff1 = 0;
    uint64_t diroff2 = 0;
    uint64_t diroff3 = 0;
    toff_t read_off = 0;
    const TIFFFieldArray *empty_info = make_empty_custom_field_array();
    TIFF *tif;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "w8");
    if (tif != NULL)
    {
        setup_minimal_image_tags(tif, Data, Size);
        write_some_pixels(tif, Data, Size);
        (void)TIFFWriteDirectory(tif);

        (void)TIFFCreateGPSDirectory(tif);
        (void)TIFFWriteCustomDirectory(tif, &diroff1);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteCustomDirectory(tif, &diroff2);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateCustomDirectory(tif, empty_info);
        (void)TIFFWriteCustomDirectory(tif, &diroff3);
        TIFFFreeDirectory(tif);

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL)
    {
        (void)TIFFReadCustomDirectory(tif, (toff_t)diroff1, empty_info);
        TIFFFreeDirectory(tif);

        (void)TIFFReadCustomDirectory(tif, (toff_t)diroff2, empty_info);
        TIFFFreeDirectory(tif);

        if (Size >= 8)
        {
            read_off = (toff_t)(((uint64_t)Data[0]) |
                                ((uint64_t)Data[1] << 8) |
                                ((uint64_t)Data[2] << 16) |
                                ((uint64_t)Data[3] << 24) |
                                ((uint64_t)Data[4] << 32) |
                                ((uint64_t)Data[5] << 40) |
                                ((uint64_t)Data[6] << 48) |
                                ((uint64_t)Data[7] << 56));
        }

        (void)TIFFReadCustomDirectory(tif, read_off, empty_info);
        TIFFFreeDirectory(tif);

        (void)TIFFReadCustomDirectory(tif, (toff_t)diroff3, empty_info);
        TIFFFreeDirectory(tif);

        TIFFClose(tif);
    }

    remove("./dummy_file");
    return 0;
}