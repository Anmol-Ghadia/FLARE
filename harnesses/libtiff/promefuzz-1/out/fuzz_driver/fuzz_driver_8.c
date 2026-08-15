// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
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
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1;
    uint16_t samplesperpixel = 1, bitspersample = 8, photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE, planarconfig = PLANARCONFIG_CONTIG;
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
        bitspersample = (uint16_t)((Data[4] % 32) + 1);
    }
    if (Size >= 6) {
        samplesperpixel = (uint16_t)((Data[5] % 4) + 1);
    }
    if (Size >= 7) {
        photometric = (Data[6] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        if (samplesperpixel >= 3 && (Data[6] & 2)) {
            photometric = PHOTOMETRIC_RGB;
        }
    }
    if (Size >= 8) {
        orientation = (uint16_t)((Data[7] % 8) + 1);
    }
    if (Size >= 10) {
        xres = (float)((Data[8] % 200) + 1);
        yres = (float)((Data[9] % 200) + 1);
    }

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bitspersample);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    (void)TIFFSetField(tif, TIFFTAG_XRESOLUTION, xres);
    (void)TIFFSetField(tif, TIFFTAG_YRESOLUTION, yres);

    (void)TIFFDeferStrileArrayWriting(tif);
    (void)TIFFWriteCheck(tif, 0, "fuzz");
    (void)TIFFWriteDirectory(tif);
    TIFFFreeDirectory(tif);
    (void)TIFFCreateDirectory(tif);

    TIFFClose(tif);
    return 0;
}