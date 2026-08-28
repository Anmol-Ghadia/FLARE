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

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE (-1)
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static TIFFFieldArray make_custom_field_array(TIFFField *field_storage) {
    TIFFFieldArray arr;
    memset(&arr, 0, sizeof(arr));

    memset(field_storage, 0, sizeof(*field_storage));
    field_storage->field_tag = 65000U;
    field_storage->field_readcount = TIFF_VARIABLE;
    field_storage->field_writecount = TIFF_VARIABLE;
    field_storage->field_type = TIFF_ASCII;
    field_storage->reserved = 0;
    field_storage->set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field_storage->get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
    field_storage->field_bit = 65;
    field_storage->field_oktochange = 1;
    field_storage->field_passcount = 0;
    field_storage->field_name = (char *)"FuzzCustomASCII";
    field_storage->field_subfields = NULL;

    arr.type = (TIFFFieldArrayType)0;
    arr.allocated_size = 0;
    arr.count = 1;
    arr.fields = field_storage;
    return arr;
}

static void set_minimal_image_tags(TIFF *tif, uint32_t w, uint32_t h) {
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w ? w : 1);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h ? h : 1);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif_rw = NULL;
    TIFF *tif_ro = NULL;
    uint64_t diroff = 0;
    TIFFField custom_field;
    TIFFFieldArray custom_array;
    uint32_t width, height;
    unsigned char pixel = 0;

    write_dummy_file(Data, Size);

    custom_array = make_custom_field_array(&custom_field);

    width = (Size > 0) ? ((uint32_t)Data[0] % 8U) + 1U : 1U;
    height = (Size > 1) ? ((uint32_t)Data[1] % 8U) + 1U : 1U;
    pixel = (Size > 2) ? Data[2] : 0;

    tif_rw = TIFFOpen("./dummy_file", "w8");
    if (tif_rw) {
        set_minimal_image_tags(tif_rw, width, height);
        (void)TIFFWriteScanline(tif_rw, &pixel, 0, 0);

        (void)TIFFCreateGPSDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateCustomDirectory(tif_rw, &custom_array);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        if (Size & 1U) {
            (void)TIFFCreateGPSDirectory(tif_rw);
            (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
            TIFFFreeDirectory(tif_rw);
        } else {
            (void)TIFFCreateEXIFDirectory(tif_rw);
            (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
            TIFFFreeDirectory(tif_rw);
        }

        TIFFClose(tif_rw);
    }

    tif_ro = TIFFOpen("./dummy_file", "r");
    if (tif_ro) {
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)0, &custom_array);
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)8, &custom_array);
        (void)TIFFReadCustomDirectory(tif_ro, (toff_t)diroff, &custom_array);

        if (Size >= sizeof(uint64_t)) {
            uint64_t fuzz_off = 0;
            memcpy(&fuzz_off, Data, sizeof(uint64_t));
            (void)TIFFReadCustomDirectory(tif_ro, (toff_t)fuzz_off, &custom_array);
        } else if (Size > 0) {
            uint64_t fuzz_off = 0;
            size_t i;
            for (i = 0; i < Size; ++i) {
                fuzz_off = (fuzz_off << 8) ^ Data[i];
            }
            (void)TIFFReadCustomDirectory(tif_ro, (toff_t)fuzz_off, &custom_array);
        }

        TIFFFreeDirectory(tif_ro);
        TIFFClose(tif_ro);
    }

    return 0;
}