#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    (void)TIFFIsTiled(tif);

    uint32_t u32 = 0;
    uint16_t u16 = 0;
    float f = 0.0f;
    char *str = NULL;
    uint16_t *u16p = NULL;
    float *fp0 = NULL;
    float *fp1 = NULL;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &u32);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &u32);
    (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &u16);
    (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
    (void)TIFFGetField(tif, TIFFTAG_XRESOLUTION, &f);
    (void)TIFFGetField(tif, TIFFTAG_SOFTWARE, &str);
    (void)TIFFGetField(tif, TIFFTAG_COLORMAP, &u16p, &u16p, &u16p);

    TIFFClose(tif);

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        (void)TIFFGetField(tif, TIFFTAG_WHITEPOINT, &fp0, &fp1);
        TIFFClose(tif);
    }

    return 0;
}