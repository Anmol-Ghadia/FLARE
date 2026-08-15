// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiffio.h"

extern void LogLuv24toXYZ(uint32_t, float *);
extern void LogLuv32toXYZ(uint32_t, float *);
extern int TIFFCIELabToRGBInit(TIFFCIELabToRGB *, const TIFFDisplay *, float *);
extern void TIFFCIELabToXYZ(TIFFCIELabToRGB *, uint32_t, int32_t, int32_t,
                            float *, float *, float *);
extern void TIFFXYZToRGB(TIFFCIELabToRGB *, float, float, float,
                         uint32_t *, uint32_t *, uint32_t *);
extern int TIFFYCbCrToRGBInit(TIFFYCbCrToRGB *, float *, float *);
extern void TIFFYCbCrtoRGB(TIFFYCbCrToRGB *, uint32_t, int32_t, int32_t,
                           uint32_t *, uint32_t *, uint32_t *);

static uint32_t rd32(const uint8_t *p, size_t size, size_t *off) {
    uint32_t v = 0;
    if (*off < size) v |= (uint32_t)p[(*off)++];
    if (*off < size) v |= (uint32_t)p[(*off)++] << 8;
    if (*off < size) v |= (uint32_t)p[(*off)++] << 16;
    if (*off < size) v |= (uint32_t)p[(*off)++] << 24;
    return v;
}

static int32_t rd32s(const uint8_t *p, size_t size, size_t *off) {
    return (int32_t)rd32(p, size, off);
}

static float rdfloat(const uint8_t *p, size_t size, size_t *off) {
    union {
        uint32_t u;
        float f;
    } v;
    v.u = rd32(p, size, off);
    return v.f;
}

int LLVMFuzzerTestOneInput_69(const uint8_t *Data, size_t Size) {
    size_t off = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    uint32_t pix24 = rd32(Data, Size, &off);
    uint32_t pix32 = rd32(Data, Size, &off);

    float xyz24[3] = {0.0f, 0.0f, 0.0f};
    float xyz32[3] = {0.0f, 0.0f, 0.0f};

    LogLuv24toXYZ(pix24, xyz24);
    LogLuv32toXYZ(pix32, xyz32);

    TIFFDisplay display;
    memset(&display, 0, sizeof(display));

    float refWhite[3];
    refWhite[0] = rdfloat(Data, Size, &off);
    refWhite[1] = rdfloat(Data, Size, &off);
    refWhite[2] = rdfloat(Data, Size, &off);

    TIFFCIELabToRGB cielab;
    memset(&cielab, 0, sizeof(cielab));

    if (TIFFCIELabToRGBInit(&cielab, &display, refWhite)) {
        uint32_t L = rd32(Data, Size, &off);
        int32_t a = rd32s(Data, Size, &off);
        int32_t b = rd32s(Data, Size, &off);

        float X = 0.0f, Y = 0.0f, Z = 0.0f;
        uint32_t r = 0, g = 0, bl = 0;

        TIFFCIELabToXYZ(&cielab, L, a, b, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &r, &g, &bl);
        TIFFXYZToRGB(&cielab, xyz24[0], xyz24[1], xyz24[2], &r, &g, &bl);
        TIFFXYZToRGB(&cielab, xyz32[0], xyz32[1], xyz32[2], &r, &g, &bl);
        TIFFXYZToRGB(&cielab,
                     rdfloat(Data, Size, &off),
                     rdfloat(Data, Size, &off),
                     rdfloat(Data, Size, &off),
                     &r, &g, &bl);
    }

    return 0;
}