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
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_SETGET_UNDEFINED
#define TIFF_SETGET_UNDEFINED 0
#endif

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void setup_minimal_image_tags(TIFF *tif, const uint8_t *Data, size_t Size) {
    if (!tif)
        return;

    uint32_t width = 1 + (Size > 0 ? Data[0] : 0);
    uint32_t height = 1 + (Size > 1 ? Data[1] : 0);
    uint16_t spp = 1;
    uint16_t bps = 8;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = height ? height : 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (tifw) {
        setup_minimal_image_tags(tifw, Data, Size);

        TIFFField customField;
        memset(&customField, 0, sizeof(customField));
        customField.field_tag = 65000U;
        customField.field_readcount = TIFF_VARIABLE;
        customField.field_writecount = TIFF_VARIABLE;
        customField.field_type = TIFF_ASCII;
        customField.set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
        customField.get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
        customField.field_bit = FIELD_CUSTOM;
        customField.field_oktochange = 1;
        customField.field_passcount = 0;
        customField.field_name = (char *)"FuzzCustomASCII";
        customField.field_subfields = NULL;

        TIFFFieldArray infoarray;
        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.allocated_size = 0;
        infoarray.count = 1;
        infoarray.fields = &customField;

        if (Size > 0) {
            switch (Data[0] % 3) {
                case 0:
                    infoarray.type = (TIFFFieldArrayType)0;
                    break;
                case 1:
                    infoarray.type = (TIFFFieldArrayType)1;
                    break;
                default:
                    infoarray.type = (TIFFFieldArrayType)2;
                    break;
            }
        } else {
            infoarray.type = (TIFFFieldArrayType)0;
        }

        uint64_t diroff = 0;

        (void)TIFFCreateCustomDirectory(tifw, &infoarray);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateGPSDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateEXIFDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &diroff);
        TIFFFreeDirectory(tifw);

        if (Size > 2) {
            (void)TIFFCreateCustomDirectory(tifw, &infoarray);
            (void)TIFFWriteCustomDirectory(tifw, &diroff);
            TIFFFreeDirectory(tifw);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        TIFFField customField;
        memset(&customField, 0, sizeof(customField));
        customField.field_tag = 65000U;
        customField.field_readcount = TIFF_VARIABLE;
        customField.field_writecount = TIFF_VARIABLE;
        customField.field_type = TIFF_ASCII;
        customField.set_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
        customField.get_field_type = (TIFFSetGetFieldType)TIFF_SETGET_UNDEFINED;
        customField.field_bit = FIELD_CUSTOM;
        customField.field_oktochange = 1;
        customField.field_passcount = 0;
        customField.field_name = (char *)"FuzzCustomASCII";
        customField.field_subfields = NULL;

        TIFFFieldArray infoarray;
        memset(&infoarray, 0, sizeof(infoarray));
        infoarray.type = (TIFFFieldArrayType)0;
        infoarray.allocated_size = 0;
        infoarray.count = 1;
        infoarray.fields = &customField;

        toff_t off1 = 0;
        toff_t off2 = (Size >= sizeof(uint64_t)) ? (toff_t)(*(const uint64_t *)Data) : 8;
        toff_t off3 = (toff_t)(Size ? Data[0] : 0);

        (void)TIFFReadCustomDirectory(tifr, off1, &infoarray);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off2, &infoarray);
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off3, &infoarray);
        TIFFFreeDirectory(tifr);

        TIFFClose(tifr);
    }

    remove("./dummy_file");
    return 0;
}