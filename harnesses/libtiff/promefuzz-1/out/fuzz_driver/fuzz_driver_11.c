// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
    const char *filename = "./dummy_file";
    FILE *fp = fopen(filename, "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif1 = TIFFOpen(filename, "r");
    if (tif1 != NULL) {
        (void)TIFFIsTiled(tif1);

        uint32_t u32 = 0;
        uint16_t u16 = 0;
        float f = 0.0f;
        char *str = NULL;

        (void)TIFFGetField(tif1, TIFFTAG_IMAGEWIDTH, &u32);
        (void)TIFFGetField(tif1, TIFFTAG_IMAGELENGTH, &u32);
        (void)TIFFGetField(tif1, TIFFTAG_BITSPERSAMPLE, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_SAMPLESPERPIXEL, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_COMPRESSION, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_XRESOLUTION, &f);
        (void)TIFFGetField(tif1, TIFFTAG_SOFTWARE, &str);

        TIFFClose(tif1);
    }

    TIFF *tif2 = TIFFOpen(filename, "r");
    if (tif2 != NULL) {
        TIFFClose(tif2);
    }

    return 0;
}