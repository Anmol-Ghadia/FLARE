// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif1 = TIFFOpen("./dummy_file", "r");
    TIFF *tif2 = TIFFOpen("./dummy_file", "r");

    if (!tif1) {
        if (tif2) TIFFClose(tif2);
        return 0;
    }

    uint32_t longv = 0;
    uint16_t shortv = 0;
    float floatv = 0.0f;
    char *strv = NULL;
    void *voidp = NULL;
    uint16_t *short_arr = NULL;
    uint32_t count32 = 0;
    uint16_t count16 = 0;
    toff_t diroff1 = 0, diroff2 = 0;
    uint16_t dirindex = 0;

    if (Size > 0) {
        dirindex = (uint16_t)Data[0];
    }
    if (Size >= sizeof(toff_t)) {
        memcpy(&diroff1, Data, sizeof(toff_t));
    } else {
        for (size_t i = 0; i < Size; i++) {
            diroff1 = (diroff1 << 8) ^ Data[i];
        }
    }
    if (Size >= 2 * sizeof(toff_t)) {
        memcpy(&diroff2, Data + sizeof(toff_t), sizeof(toff_t));
    } else {
        diroff2 = diroff1 ^ (toff_t)Size;
    }

    TIFFGetField(tif1, TIFFTAG_IMAGEWIDTH, &longv);
    TIFFGetField(tif1, TIFFTAG_IMAGELENGTH, &longv);
    TIFFGetField(tif1, TIFFTAG_BITSPERSAMPLE, &shortv);
    TIFFGetField(tif1, TIFFTAG_SAMPLESPERPIXEL, &shortv);
    TIFFGetField(tif1, TIFFTAG_COMPRESSION, &shortv);
    TIFFGetField(tif1, TIFFTAG_PHOTOMETRIC, &shortv);
    TIFFGetField(tif1, TIFFTAG_FILLORDER, &shortv);
    TIFFGetField(tif1, TIFFTAG_ORIENTATION, &shortv);
    TIFFGetField(tif1, TIFFTAG_PLANARCONFIG, &shortv);
    TIFFReadGPSDirectory(tif1, diroff1);
    TIFFGetField(tif1, TIFFTAG_XRESOLUTION, &floatv);
    TIFFGetField(tif1, TIFFTAG_YRESOLUTION, &floatv);
    TIFFGetField(tif1, TIFFTAG_RESOLUTIONUNIT, &shortv);
    TIFFGetField(tif1, TIFFTAG_ROWSPERSTRIP, &longv);
    TIFFGetField(tif1, TIFFTAG_SUBFILETYPE, &longv);
    TIFFGetField(tif1, TIFFTAG_MINSAMPLEVALUE, &shortv);
    TIFFGetField(tif1, TIFFTAG_MAXSAMPLEVALUE, &shortv);
    TIFFGetField(tif1, TIFFTAG_SOFTWARE, &strv);
    TIFFGetField(tif1, TIFFTAG_DATETIME, &strv);
    TIFFGetField(tif1, TIFFTAG_ARTIST, &strv);
    TIFFSetDirectory(tif1, dirindex);
    TIFFGetField(tif1, TIFFTAG_PAGENUMBER, &count16, &shortv);
    TIFFReadEXIFDirectory(tif1, diroff2);
    TIFFGetField(tif1, TIFFTAG_EXIFIFD, &diroff1);
    (void)TIFFFindField(tif1, (uint32_t)(Size > 1 ? Data[1] : TIFFTAG_IMAGEWIDTH), TIFF_ANY);
    TIFFGetField(tif1, TIFFTAG_PRIMARYCHROMATICITIES, &voidp);
    TIFFGetField(tif1, TIFFTAG_WHITEPOINT, &voidp);
    TIFFGetField(tif1, TIFFTAG_HALFTONEHINTS, &shortv, &count16);
    TIFFGetField(tif1, TIFFTAG_COLORMAP, &short_arr, &short_arr, &short_arr);
    TIFFGetField(tif1, TIFFTAG_TILEWIDTH, &longv);
    TIFFGetField(tif1, TIFFTAG_TILELENGTH, &longv);
    TIFFGetField(tif1, TIFFTAG_INKSET, &shortv);
    TIFFGetField(tif1, TIFFTAG_DOTRANGE, &shortv, &count16);
    TIFFGetField(tif1, TIFFTAG_TARGETPRINTER, &strv);
    TIFFGetField(tif1, TIFFTAG_EXTRASAMPLES, &shortv, &short_arr);

    TIFFClose(tif1);
    if (tif2) TIFFClose(tif2);

    return 0;
}