// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFYCbCrtoRGB at tif_color.c:181:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
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

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++];
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++] << 8;
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++] << 16;
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++] << 24;
    return v;
}

static int32_t read_i32(const uint8_t *Data, size_t Size, size_t *Off) {
    return (int32_t)read_u32(Data, Size, Off);
}

static float read_floatish(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t u = read_u32(Data, Size, Off);
    int32_t s = (int32_t)u;
    return (float)(s % 200000) / 1000.0f;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput_70(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    float xyz1[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float X = 0.0f, Y = 0.0f, Z = 0.0f;
    uint32_t R = 0, G = 0, B = 0;
    uint32_t pixel24, pixel32;
    uint32_t L;
    int32_t a, b;
    float refWhite[3];
    TIFFDisplay display;
    TIFFCIELabToRGB cielab;
    int cielab_ok = 0;

    write_dummy_file(Data, Size);

    memset(&display, 0, sizeof(display));
    memset(&cielab, 0, sizeof(cielab));

    pixel24 = read_u32(Data, Size, &off);
    pixel32 = read_u32(Data, Size, &off);
    L = read_u32(Data, Size, &off);
    a = read_i32(Data, Size, &off);
    b = read_i32(Data, Size, &off);

    refWhite[0] = read_floatish(Data, Size, &off);
    refWhite[1] = read_floatish(Data, Size, &off);
    refWhite[2] = read_floatish(Data, Size, &off);

    ((float *)&display)[0] = 0.1f + (float)((read_u32(Data, Size, &off) % 1000) + 1) / 100.0f;
    ((float *)&display)[1] = 0.1f + (float)((read_u32(Data, Size, &off) % 1000) + 1) / 100.0f;
    ((float *)&display)[2] = 0.1f + (float)((read_u32(Data, Size, &off) % 1000) + 1) / 100.0f;
    ((float *)&display)[3] = read_floatish(Data, Size, &off);
    ((float *)&display)[4] = read_floatish(Data, Size, &off);
    ((float *)&display)[5] = read_floatish(Data, Size, &off);
    ((float *)&display)[6] = read_floatish(Data, Size, &off);
    ((float *)&display)[7] = read_floatish(Data, Size, &off);
    ((float *)&display)[8] = read_floatish(Data, Size, &off);

    LogLuv24toXYZ(pixel24, xyz1);
    LogLuv32toXYZ(pixel32, xyz2);

    cielab_ok = TIFFCIELabToRGBInit(&cielab, &display, refWhite);
    if (cielab_ok) {
        TIFFCIELabToXYZ(&cielab, L, a, b, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);

        TIFFXYZToRGB(&cielab, xyz1[0], xyz1[1], xyz1[2], &R, &G, &B);
        TIFFXYZToRGB(&cielab, xyz2[0], xyz2[1], xyz2[2], &R, &G, &B);

        TIFFCIELabToXYZ(&cielab, pixel24 & 0xffu, (int32_t)((pixel24 >> 8) & 0xffu) - 128,
                        (int32_t)((pixel24 >> 16) & 0xffu) - 128, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);
    }

    if (0) {
        TIFFYCbCrToRGB ycbcr;
        TIFFYCbCrtoRGB(&ycbcr, 0, 0, 0, &R, &G, &B);
    }

    return 0;
}