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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif1 = TIFFOpen("./dummy_file", "r");
    TIFF *tif2 = TIFFOpen("./dummy_file", "r");

    if (tif1) {
        uint32_t u32 = 0;
        uint16_t u16 = 0;
        float f = 0.0f;
        char *str = NULL;
        void *vp = NULL;
        uint16_t *u16p = NULL;
        uint32_t *u32p = NULL;
        float *fpv = NULL;
        double *dp = NULL;
        uint16_t count16 = 0;
        uint32_t count32 = 0;
        toff_t off = 0;
        const TIFFField *field = NULL;

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

        (void)TIFFGetField(tif1, TIFFTAG_GPSVERSIONID, &count16, &vp);
        (void)TIFFGetField(tif1, TIFFTAG_GPSLATITUDEREF, &str);
        (void)TIFFGetField(tif1, TIFFTAG_GPSLATITUDE, &count16, &vp);
        (void)TIFFGetField(tif1, TIFFTAG_GPSLONGITUDEREF, &str);
        (void)TIFFGetField(tif1, TIFFTAG_GPSLONGITUDE, &count16, &vp);
        (void)TIFFGetField(tif1, TIFFTAG_GPSALTITUDEREF, &u16);
        (void)TIFFGetField(tif1, TIFFTAG_GPSALTITUDE, &count16, &vp);
        (void)TIFFGetField(tif1, TIFFTAG_GPSTIMESTAMP, &count16, &vp);
        (void)TIFFGetField(tif1, TIFFTAG_GPSDATESTAMP, &str);
        (void)TIFFGetField(tif1, TIFFTAG_GPSPROCESSINGMETHOD, &count32, &vp);

        (void)TIFFSetDirectory(tif1, (uint16_t)(Size > 0 ? Data[0] : 0));
        (void)TIFFGetField(tif1, TIFFTAG_EXIFIFD, &off);
        (void)TIFFReadEXIFDirectory(tif1, off);
        (void)TIFFGetField(tif1, EXIFTAG_EXPOSURETIME, &count16, &vp);

        field = TIFFFindField(tif1, TIFFTAG_IMAGEWIDTH, TIFF_ANY);
        (void)field;

        (void)TIFFGetField(tif1, EXIFTAG_FNUMBER, &count16, &vp);
        (void)TIFFGetField(tif1, EXIFTAG_EXPOSUREPROGRAM, &u16);
        (void)TIFFGetField(tif1, EXIFTAG_SPECTRALSENSITIVITY, &str);
        (void)TIFFGetField(tif1, EXIFTAG_ISOSPEEDRATINGS, &count16, &u16p);
        (void)TIFFGetField(tif1, EXIFTAG_EXIFVERSION, &count16, &vp);
        (void)TIFFGetField(tif1, EXIFTAG_DATETIMEORIGINAL, &str);
        (void)TIFFGetField(tif1, EXIFTAG_DATETIMEDIGITIZED, &str);
        (void)TIFFGetField(tif1, EXIFTAG_SHUTTERSPEEDVALUE, &f);
        (void)TIFFGetField(tif1, EXIFTAG_APERTUREVALUE, &f);
        (void)TIFFGetField(tif1, EXIFTAG_BRIGHTNESSVALUE, &f);

        TIFFClose(tif1);
    }

    if (tif2) {
        TIFFClose(tif2);
    }

    return 0;
}