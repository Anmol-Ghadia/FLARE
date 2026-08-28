// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint32_t u32 = 0;
    uint16_t u16 = 0;
    float f = 0.0f;
    char *str = NULL;
    void *vp = NULL;
    uint16_t *u16p = NULL;
    toff_t off = 0;
    const TIFFField *field = NULL;

    write_dummy_file(Data, Size);

    TIFF *tif1 = TIFFOpen("./dummy_file", "r");
    TIFF *tif2 = TIFFOpen("./dummy_file", "r");

    if (tif1 != NULL) {
        (void)TIFFGetField(tif1, TIFFTAG_IMAGEWIDTH, &u32);
        (void)TIFFGetField(tif1, TIFFTAG_IMAGELENGTH, &u32);
        (void)TIFFGetField(tif1, TIFFTAG_BITSPERSAMPLE, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_SAMPLESPERPIXEL, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_COMPRESSION, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_PHOTOMETRIC, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_ORIENTATION, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_XRESOLUTION, &f);
        (void)TIFFGetField(tif1, TIFFTAG_YRESOLUTION, &f);

        (void)TIFFGetField(tif1, TIFFTAG_GPSIFD, &off);
        (void)TIFFReadGPSDirectory(tif1, off);

        (void)TIFFGetField(tif1, TIFFTAG_GPSIFD, &off);
        (void)TIFFGetField(tif1, TIFFTAG_EXIFIFD, &off);
        (void)TIFFGetField(tif1, TIFFTAG_XMLPACKET, &str);
        (void)TIFFGetField(tif1, TIFFTAG_ARTIST, &str);
        (void)TIFFGetField(tif1, TIFFTAG_COPYRIGHT, &str);
        (void)TIFFGetField(tif1, TIFFTAG_DATETIME, &str);
        (void)TIFFGetField(tif1, TIFFTAG_DOCUMENTNAME, &str);
        (void)TIFFGetField(tif1, TIFFTAG_HOSTCOMPUTER, &str);
        (void)TIFFGetField(tif1, TIFFTAG_IMAGEDESCRIPTION, &str);
        (void)TIFFGetField(tif1, TIFFTAG_MAKE, &str);

        (void)TIFFSetDirectory(tif1, (uint16_t)(Size > 0 ? Data[0] : 0));
        (void)TIFFGetField(tif1, TIFFTAG_EXIFIFD, &off);
        (void)TIFFReadEXIFDirectory(tif1, off);
        (void)TIFFGetField(tif1, EXIFTAG_EXPOSURETIME, &f);

        field = TIFFFindField(tif1, TIFFTAG_IMAGEWIDTH, TIFF_ANY);
        (void)field;

        (void)TIFFGetField(tif1, EXIFTAG_FNUMBER, &f);
        (void)TIFFGetField(tif1, EXIFTAG_EXPOSUREPROGRAM, &u16);
        (void)TIFFGetField(tif1, EXIFTAG_SPECTRALSENSITIVITY, &str);
        (void)TIFFGetField(tif1, EXIFTAG_ISOSPEEDRATINGS, &u16p);
        (void)TIFFGetField(tif1, EXIFTAG_EXIFVERSION, &vp);
        (void)TIFFGetField(tif1, EXIFTAG_DATETIMEORIGINAL, &str);
        (void)TIFFGetField(tif1, EXIFTAG_DATETIMEDIGITIZED, &str);
        (void)TIFFGetField(tif1, EXIFTAG_SHUTTERSPEEDVALUE, &f);
        (void)TIFFGetField(tif1, EXIFTAG_APERTUREVALUE, &f);
        (void)TIFFGetField(tif1, EXIFTAG_BRIGHTNESSVALUE, &f);

        TIFFClose(tif1);
    }

    if (tif2 != NULL) {
        TIFFClose(tif2);
    }

    return 0;
}