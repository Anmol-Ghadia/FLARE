// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
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

static uint32_t pick_tag(const uint8_t *Data, size_t Size, size_t idx) {
    static const uint32_t tags[] = {
        TIFFTAG_IMAGEWIDTH,
        TIFFTAG_IMAGELENGTH,
        TIFFTAG_BITSPERSAMPLE,
        TIFFTAG_COMPRESSION,
        TIFFTAG_PHOTOMETRIC,
        TIFFTAG_FILLORDER,
        TIFFTAG_ORIENTATION,
        TIFFTAG_SAMPLESPERPIXEL,
        TIFFTAG_ROWSPERSTRIP,
        TIFFTAG_PLANARCONFIG,
        TIFFTAG_RESOLUTIONUNIT,
        TIFFTAG_XRESOLUTION,
        TIFFTAG_YRESOLUTION,
        TIFFTAG_TILEWIDTH,
        TIFFTAG_TILELENGTH,
        TIFFTAG_SUBFILETYPE,
        TIFFTAG_PAGENUMBER,
        TIFFTAG_HALFTONEHINTS,
        TIFFTAG_COLORMAP,
        TIFFTAG_EXTRASAMPLES,
        TIFFTAG_WHITEPOINT,
        TIFFTAG_PRIMARYCHROMATICITIES,
        TIFFTAG_TRANSFERFUNCTION,
        TIFFTAG_ARTIST,
        TIFFTAG_DATETIME,
        TIFFTAG_HOSTCOMPUTER,
        TIFFTAG_IMAGEDESCRIPTION,
        TIFFTAG_MAKE,
        TIFFTAG_MODEL,
        TIFFTAG_SOFTWARE,
        TIFFTAG_DOCUMENTNAME,
        TIFFTAG_PAGENAME,
        TIFFTAG_XPOSITION,
        TIFFTAG_YPOSITION
    };
    if (Size == 0) return tags[idx % (sizeof(tags) / sizeof(tags[0]))];
    return tags[(Data[idx % Size] + idx) % (sizeof(tags) / sizeof(tags[0]))];
}

static void do_getfield(TIFF *tif, uint32_t tag) {
    uint16_t u16_a = 0, u16_b = 0, u16_c = 0;
    uint32_t u32_a = 0, u32_b = 0;
    float f32_a = 0.0f, f32_b = 0.0f, f32_c = 0.0f, f32_d = 0.0f, f32_e = 0.0f, f32_f = 0.0f;
    char *str = NULL;
    uint16_t *pu16 = NULL;
    float *pf32 = NULL;

    switch (tag) {
        case TIFFTAG_IMAGEWIDTH:
        case TIFFTAG_IMAGELENGTH:
        case TIFFTAG_TILEWIDTH:
        case TIFFTAG_TILELENGTH:
        case TIFFTAG_SUBFILETYPE:
            (void)TIFFGetField(tif, tag, &u32_a);
            break;
        case TIFFTAG_BITSPERSAMPLE:
        case TIFFTAG_COMPRESSION:
        case TIFFTAG_PHOTOMETRIC:
        case TIFFTAG_FILLORDER:
        case TIFFTAG_ORIENTATION:
        case TIFFTAG_SAMPLESPERPIXEL:
        case TIFFTAG_ROWSPERSTRIP:
        case TIFFTAG_PLANARCONFIG:
        case TIFFTAG_RESOLUTIONUNIT:
            (void)TIFFGetField(tif, tag, &u16_a);
            break;
        case TIFFTAG_XRESOLUTION:
        case TIFFTAG_YRESOLUTION:
        case TIFFTAG_XPOSITION:
        case TIFFTAG_YPOSITION:
            (void)TIFFGetField(tif, tag, &f32_a);
            break;
        case TIFFTAG_PAGENUMBER:
        case TIFFTAG_HALFTONEHINTS:
            (void)TIFFGetField(tif, tag, &u16_a, &u16_b);
            break;
        case TIFFTAG_COLORMAP:
            (void)TIFFGetField(tif, tag, &pu16, &pu16, &pu16);
            break;
        case TIFFTAG_EXTRASAMPLES:
            (void)TIFFGetField(tif, tag, &u16_a, &pu16);
            break;
        case TIFFTAG_WHITEPOINT:
            (void)TIFFGetField(tif, tag, &pf32);
            break;
        case TIFFTAG_PRIMARYCHROMATICITIES:
            (void)TIFFGetField(tif, tag, &pf32);
            break;
        case TIFFTAG_TRANSFERFUNCTION:
            (void)TIFFGetField(tif, tag, &pu16, &pu16, &pu16);
            break;
        case TIFFTAG_ARTIST:
        case TIFFTAG_DATETIME:
        case TIFFTAG_HOSTCOMPUTER:
        case TIFFTAG_IMAGEDESCRIPTION:
        case TIFFTAG_MAKE:
        case TIFFTAG_MODEL:
        case TIFFTAG_SOFTWARE:
        case TIFFTAG_DOCUMENTNAME:
        case TIFFTAG_PAGENAME:
            (void)TIFFGetField(tif, tag, &str);
            break;
        default:
            (void)TIFFGetField(tif, tag, &u32_a);
            break;
    }

    (void)u16_c;
    (void)u32_b;
    (void)f32_b;
    (void)f32_c;
    (void)f32_d;
    (void)f32_e;
    (void)f32_f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    const char *mode1 = "r";
    const char *mode2 = "rm";
    if (Size > 0) {
        switch (Data[0] % 4) {
            case 0: mode1 = "r"; break;
            case 1: mode1 = "rm"; break;
            case 2: mode1 = "rc"; break;
            default: mode1 = "rh"; break;
        }
    }
    if (Size > 1) {
        switch (Data[1] % 4) {
            case 0: mode2 = "r"; break;
            case 1: mode2 = "rm"; break;
            case 2: mode2 = "rc"; break;
            default: mode2 = "rh"; break;
        }
    }

    TIFF *tif1 = TIFFOpen("./dummy_file", mode1);
    int tiled = 0;
    if (tif1 != NULL) {
        tiled = TIFFIsTiled(tif1);

        do_getfield(tif1, pick_tag(Data, Size, 0));
        do_getfield(tif1, pick_tag(Data, Size, 1));
        do_getfield(tif1, pick_tag(Data, Size, 2));
        do_getfield(tif1, pick_tag(Data, Size, 3));
        do_getfield(tif1, pick_tag(Data, Size, 4));
        do_getfield(tif1, pick_tag(Data, Size, 5));
        do_getfield(tif1, tiled ? TIFFTAG_TILEWIDTH : TIFFTAG_ROWSPERSTRIP);

        TIFFClose(tif1);
    }

    TIFF *tif2 = TIFFOpen("./dummy_file", mode2);
    if (tif2 != NULL) {
        TIFFClose(tif2);
    }

    return 0;
}