// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static uint32_t read_u32(const uint8_t *p)
{
    return ((uint32_t)p[0]) | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int32_t read_s32(const uint8_t *p)
{
    return (int32_t)read_u32(p);
}

static float read_f32(const uint8_t *p)
{
    float f;
    uint32_t u = read_u32(p);
    memcpy(&f, &u, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    uint32_t encoded24 = 0, encoded32 = 0;
    float xyz24[3] = {0.0f, 0.0f, 0.0f};
    float xyz32[3] = {0.0f, 0.0f, 0.0f};
    float X = 0.0f, Y = 0.0f, Z = 0.0f;
    uint32_t R = 0, G = 0, B = 0;
    uint32_t L = 0, Yv = 0;
    int32_t a = 0, b = 0;
    float refWhite[3] = {0.3457f, 0.3585f, 1.0f};
    TIFFDisplay display;
    TIFFCIELabToRGB cielab;
    int have_cielab = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(&display, 0, sizeof(display));
    memset(&cielab, 0, sizeof(cielab));

    if (Size >= 4)
        encoded24 = read_u32(Data);
    if (Size >= 8)
        encoded32 = read_u32(Data + 4);
    if (Size >= 12)
        L = read_u32(Data + 8);
    if (Size >= 16)
        a = read_s32(Data + 12);
    if (Size >= 20)
        b = read_s32(Data + 16);
    if (Size >= 24)
        refWhite[0] = read_f32(Data + 20);
    if (Size >= 28)
        refWhite[1] = read_f32(Data + 24);
    if (Size >= 32)
        refWhite[2] = read_f32(Data + 28);

    display.d_mat[0][0] = 3.2410f;
    display.d_mat[0][1] = -1.5374f;
    display.d_mat[0][2] = -0.4986f;
    display.d_mat[1][0] = -0.9692f;
    display.d_mat[1][1] = 1.8760f;
    display.d_mat[1][2] = 0.0416f;
    display.d_mat[2][0] = 0.0556f;
    display.d_mat[2][1] = -0.2040f;
    display.d_mat[2][2] = 1.0570f;
    display.d_YCR = 100.0f;
    display.d_YCG = 100.0f;
    display.d_YCB = 100.0f;
    display.d_Vrwr = 255;
    display.d_Vrwg = 255;
    display.d_Vrwb = 255;
    display.d_Y0R = 1.0f;
    display.d_Y0G = 1.0f;
    display.d_Y0B = 1.0f;
    display.d_gammaR = 2.2f;
    display.d_gammaG = 2.2f;
    display.d_gammaB = 2.2f;

    if (Size >= 36)
        display.d_mat[0][0] = read_f32(Data + 32);
    if (Size >= 40)
        display.d_mat[0][1] = read_f32(Data + 36);
    if (Size >= 44)
        display.d_mat[0][2] = read_f32(Data + 40);
    if (Size >= 48)
        display.d_mat[1][0] = read_f32(Data + 44);
    if (Size >= 52)
        display.d_mat[1][1] = read_f32(Data + 48);
    if (Size >= 56)
        display.d_mat[1][2] = read_f32(Data + 52);
    if (Size >= 60)
        display.d_mat[2][0] = read_f32(Data + 56);
    if (Size >= 64)
        display.d_mat[2][1] = read_f32(Data + 60);
    if (Size >= 68)
        display.d_mat[2][2] = read_f32(Data + 64);
    if (Size >= 72)
        display.d_YCR = read_f32(Data + 68);
    if (Size >= 76)
        display.d_YCG = read_f32(Data + 72);
    if (Size >= 80)
        display.d_YCB = read_f32(Data + 76);
    if (Size >= 84)
        display.d_Vrwr = read_u32(Data + 80);
    if (Size >= 88)
        display.d_Vrwg = read_u32(Data + 84);
    if (Size >= 92)
        display.d_Vrwb = read_u32(Data + 88);
    if (Size >= 96)
        display.d_Y0R = read_f32(Data + 92);
    if (Size >= 100)
        display.d_Y0G = read_f32(Data + 96);
    if (Size >= 104)
        display.d_Y0B = read_f32(Data + 100);
    if (Size >= 108)
        display.d_gammaR = read_f32(Data + 104);
    if (Size >= 112)
        display.d_gammaG = read_f32(Data + 108);
    if (Size >= 116)
        display.d_gammaB = read_f32(Data + 112);

    LogLuv24toXYZ(encoded24, xyz24);
    LogLuv32toXYZ(encoded32, xyz32);

    if (TIFFCIELabToRGBInit(&cielab, &display, refWhite))
        have_cielab = 1;

    if (have_cielab)
    {
        TIFFCIELabToXYZ(&cielab, L, a, b, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);

        TIFFXYZToRGB(&cielab, xyz24[0], xyz24[1], xyz24[2], &R, &G, &B);
        TIFFXYZToRGB(&cielab, xyz32[0], xyz32[1], xyz32[2], &R, &G, &B);

        TIFFCIELabToXYZ(&cielab, 0, 0, 0, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);

        TIFFCIELabToXYZ(&cielab, 100, -128, 127, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);
    }

    return 0;
}