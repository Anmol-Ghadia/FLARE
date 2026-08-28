#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1;
    uint16_t samplesperpixel = 1;
    uint16_t bitspersample = 8;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t planarconfig = PLANARCONFIG_CONTIG;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint16_t resolutionunit = RESUNIT_INCH;
    float xres = 72.0f, yres = 72.0f;

    if (Size >= 4) {
        width = ((uint32_t)Data[0] << 8) | Data[1];
        height = ((uint32_t)Data[2] << 8) | Data[3];
        if (width == 0) width = 1;
        if (height == 0) height = 1;
    }
    if (Size >= 5) {
        samplesperpixel = (uint16_t)((Data[4] % 4) + 1);
    }
    if (Size >= 6) {
        switch (Data[5] % 4) {
            case 0: bitspersample = 1; break;
            case 1: bitspersample = 8; break;
            case 2: bitspersample = 16; break;
            default: bitspersample = 32; break;
        }
    }
    if (Size >= 7) {
        photometric = (Data[6] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    }
    if (Size >= 8) {
        compression = (Data[7] & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }
    if (Size >= 9) {
        planarconfig = (Data[8] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size >= 10) {
        orientation = (uint16_t)((Data[9] % 8) + 1);
    }
    if (Size >= 11) {
        resolutionunit = (uint16_t)((Data[10] % 3) + 1);
    }
    if (Size >= 13) {
        xres = (float)((Data[11] % 200) + 1);
        yres = (float)((Data[12] % 200) + 1);
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bitspersample);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, xres);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, yres);

    TIFFDeferStrileArrayWriting(tif);
    TIFFWriteCheck(tif, 0, "LLVMFuzzer");
    TIFFWriteDirectory(tif);
    TIFFFreeDirectory(tif);
    TIFFCreateDirectory(tif);

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}