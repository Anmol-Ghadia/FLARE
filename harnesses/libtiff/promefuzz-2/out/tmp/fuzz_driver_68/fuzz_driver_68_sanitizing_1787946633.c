#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void set_minimal_image_tags(TIFF *tif, uint32_t w, uint32_t h)
{
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w ? w : 1U);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h ? h : 1U);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
}

static const TIFFFieldArray *make_minimal_custom_field_array(void)
{
    static TIFFFieldArray infoarray;
    static int initialized = 0;

    if (!initialized)
    {
        infoarray.type = (TIFFFieldArrayType)0;
        infoarray.allocated_size = 0;
        infoarray.count = 0;
        infoarray.fields = NULL;
        initialized = 1;
    }
    return &infoarray;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    TIFF *tif_rw = NULL;
    TIFF *tif_ro = NULL;
    uint64_t custom_off1 = 0;
    uint64_t custom_off2 = 0;
    uint32_t width = 1, height = 1;
    unsigned char pixel = 0;
    toff_t fuzz_off = 0;
    size_t i;
    const TIFFFieldArray *infoarray = make_minimal_custom_field_array();

    write_dummy_file(Data, Size);

    if (Size > 0)
        width = (uint32_t)(Data[0] % 8U) + 1U;
    if (Size > 1)
        height = (uint32_t)(Data[1] % 8U) + 1U;
    if (Size > 2)
        pixel = Data[2];

    tif_rw = TIFFOpen("./dummy_file", "w8");
    if (tif_rw != NULL)
    {
        set_minimal_image_tags(tif_rw, width, height);
        (void)TIFFWriteScanline(tif_rw, &pixel, 0, 0);

        (void)TIFFCreateGPSDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &custom_off1);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &custom_off2);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateCustomDirectory(tif_rw, infoarray);
        (void)TIFFWriteCustomDirectory(tif_rw, &custom_off1);
        TIFFFreeDirectory(tif_rw);

        TIFFClose(tif_rw);
    }

    tif_ro = TIFFOpen("./dummy_file", "r");
    if (tif_ro != NULL)
    {
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)0, infoarray);
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)8, infoarray);
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)custom_off1, infoarray);
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)custom_off2, infoarray);

        for (i = 0; i < Size; ++i)
            fuzz_off = (toff_t)((fuzz_off << 5) ^ (toff_t)Data[i] ^ (fuzz_off >> 2));

        if (Size > 0)
            (void)TIFFReadCustomDirectory(tif_ro, fuzz_off, infoarray);

        TIFFFreeDirectory(tif_ro);
        TIFFClose(tif_ro);
    }

    return 0;
}