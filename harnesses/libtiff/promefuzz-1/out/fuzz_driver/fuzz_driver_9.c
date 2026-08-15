// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
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
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif == NULL) {
        return 0;
    }

    uint32_t u32 = 0;
    uint16_t u16 = 0;
    uint16_t *pu16 = NULL;
    uint32_t *pu32 = NULL;
    float *pf = NULL;
    char *pchar = NULL;
    void *pvoid = NULL;
    uint16_t spp = 0, bps = 0;
    uint32_t width = 0, length = 0;
    toff_t gps_off = 0, exif_off = 0;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &length);
    (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    (void)TIFFGetField(tif, TIFFTAG_COMPRESSION, &u16);
    (void)TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &u16);
    (void)TIFFGetField(tif, TIFFTAG_FILLORDER, &u16);
    (void)TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &u16);
    (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &u32);

    (void)TIFFGetField(tif, TIFFTAG_GPSIFD, &gps_off);
    (void)TIFFReadGPSDirectory(tif, gps_off);

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &length);
    (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    (void)TIFFGetField(tif, TIFFTAG_COMPRESSION, &u16);
    (void)TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &u16);
    (void)TIFFGetField(tif, TIFFTAG_FILLORDER, &u16);
    (void)TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &u16);
    (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &u32);
    (void)TIFFGetField(tif, TIFFTAG_EXIFIFD, &exif_off);

    (void)TIFFSetDirectory(tif, (uint16_t)(Size > 0 ? Data[0] : 0));
    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);

    (void)TIFFReadEXIFDirectory(tif, exif_off);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &length);

    (void)TIFFFindField(tif, TIFFTAG_IMAGEWIDTH, TIFF_ANY);

    (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    (void)TIFFGetField(tif, TIFFTAG_COMPRESSION, &u16);
    (void)TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &u16);
    (void)TIFFGetField(tif, TIFFTAG_FILLORDER, &u16);
    (void)TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &u16);
    (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &u32);
    (void)TIFFGetField(tif, TIFFTAG_XRESOLUTION, &pf);
    (void)TIFFGetField(tif, TIFFTAG_SOFTWARE, &pchar);
    (void)TIFFGetField(tif, TIFFTAG_COLORMAP, &pu16, &pu16, &pu16);

    TIFFClose(tif);

    tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL) {
        TIFFClose(tif);
    }

    (void)pu32;
    (void)pvoid;
    return 0;
}