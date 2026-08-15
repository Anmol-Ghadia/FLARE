#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

extern void LogLuv24toXYZ(uint32_t, float *);
extern void LogLuv32toXYZ(uint32_t, float *);
extern int TIFFCIELabToRGBInit(TIFFCIELabToRGB *, const TIFFDisplay *, float *);
extern void TIFFCIELabToXYZ(TIFFCIELabToRGB *, uint32_t, int32_t, int32_t,
                            float *, float *, float *);
extern void TIFFXYZToRGB(TIFFCIELabToRGB *, float, float, float,
                         uint32_t *, uint32_t *, uint32_t *);
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
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
        TIFFCIELabToXYZ(&cielab, L, a, b, &X, &Y, &Z);

        uint32_t r = 0, g = 0, bl = 0;
        TIFFXYZToRGB(&cielab, X, Y, Z, &r, &g, &bl);
        TIFFXYZToRGB(&cielab, xyz24[0], xyz24[1], xyz24[2], &r, &g, &bl);
        TIFFXYZToRGB(&cielab, xyz32[0], xyz32[1], xyz32[2], &r, &g, &bl);

        TIFFXYZToRGB(&cielab,
                     rdfloat(Data, Size, &off),
                     rdfloat(Data, Size, &off),
                     rdfloat(Data, Size, &off),
                     &r, &g, &bl);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        TIFFYCbCrToRGB *ycbcr = TIFFYCbCrToRGBInit(tif);
        if (ycbcr != NULL) {
            uint32_t r = 0, g = 0, bl = 0;

            uint32_t Yv = rd32(Data, Size, &off);
            int32_t Cb = rd32s(Data, Size, &off);
            int32_t Cr = rd32s(Data, Size, &off);

            TIFFYCbCrtoRGB(ycbcr, Yv, Cb, Cr, &r, &g, &bl);
            TIFFYCbCrtoRGB(ycbcr, (uint32_t)(xyz24[1] * 255.0f), (int32_t)r, (int32_t)g, &r, &g, &bl);
            TIFFYCbCrtoRGB(ycbcr, (uint32_t)(xyz32[1] * 255.0f), (int32_t)bl, (int32_t)r, &r, &g, &bl);

            TIFFfree(ycbcr);
        }
        TIFFClose(tif);
    }

    return 0;
}