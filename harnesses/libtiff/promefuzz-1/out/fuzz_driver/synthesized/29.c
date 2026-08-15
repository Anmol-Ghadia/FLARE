// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tiffio.h"

static uint32_t read_u32(const uint8_t *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int32_t read_i32(const uint8_t *p)
{
    return (int32_t)read_u32(p);
}

static float read_f32(const uint8_t *p)
{
    float f = 0.0f;
    uint32_t v = read_u32(p);
    memcpy(&f, &v, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput_29(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    float xyz[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float X = 0.0f, Y = 0.0f, Z = 0.0f;
    float f0 = 0.0f, f1 = 0.0f, f2 = 0.0f;
    uint32_t u0 = 0, u1 = 0, u2 = 0, u3 = 0;
    int32_t a = 0, b = 0;
    uint32_t L = 0;
    uint32_t r = 0, g = 0, bl = 0;
    uint32_t packed32 = 0, packed24 = 0, repacked = 0;
    int em = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (Size >= 4)
        u0 = read_u32(Data);
    if (Size >= 8)
        u1 = read_u32(Data + 4);
    if (Size >= 12)
        u2 = read_u32(Data + 8);
    if (Size >= 16)
        u3 = read_u32(Data + 12);
    if (Size >= 20)
        f0 = read_f32(Data + 16);
    if (Size >= 24)
        f1 = read_f32(Data + 20);
    if (Size >= 28)
        f2 = read_f32(Data + 24);
    if (Size >= 32)
        a = read_i32(Data + 28);
    if (Size >= 36)
        b = read_i32(Data + 32);
    if (Size >= 40)
        L = read_u32(Data + 36);

    packed32 = u0;
    packed24 = u1 & 0x00FFFFFFU;
    em = (int)(u2 & 0x7);

    LogLuv32toXYZ(packed32, xyz);
    LogLuv24toXYZ(packed24, xyz2);

    repacked = LogLuv32fromXYZ(xyz, em);
    LogLuv32toXYZ(repacked, xyz2);

    xyz[0] = f0;
    xyz[1] = f1;
    xyz[2] = f2;
    repacked = LogLuv32fromXYZ(xyz, em);
    LogLuv32toXYZ(repacked, xyz);

    TIFFSwabFloat(&f0);
    TIFFSwabFloat(&f1);
    TIFFSwabFloat(&f2);

    {
        TIFFCIELabToRGB *cielab = NULL;
        TIFFDisplay display;
        float refWhite[3];
        float refBlack[3];

        memset(&display, 0, sizeof(display));
        memset(refWhite, 0, sizeof(refWhite));
        memset(refBlack, 0, sizeof(refBlack));

        if (Size >= 36)
        {
            display.d_mat[0][0] = read_f32(Data + 0);
            display.d_mat[0][1] = read_f32(Data + 4);
            display.d_mat[0][2] = read_f32(Data + 8);
            display.d_mat[1][0] = read_f32(Data + 12);
            display.d_mat[1][1] = read_f32(Data + 16);
            display.d_mat[1][2] = read_f32(Data + 20);
            display.d_mat[2][0] = read_f32(Data + 24);
            display.d_mat[2][1] = read_f32(Data + 28);
            display.d_mat[2][2] = read_f32(Data + 32);
        }
        else
        {
            display.d_mat[0][0] = 3.2406f;
            display.d_mat[0][1] = -1.5372f;
            display.d_mat[0][2] = -0.4986f;
            display.d_mat[1][0] = -0.9689f;
            display.d_mat[1][1] = 1.8758f;
            display.d_mat[1][2] = 0.0415f;
            display.d_mat[2][0] = 0.0557f;
            display.d_mat[2][1] = -0.2040f;
            display.d_mat[2][2] = 1.0570f;
        }

        display.d_YCR = 100.0f;
        display.d_YCG = 100.0f;
        display.d_YCB = 100.0f;
        display.d_Vrwr = 1.0f;
        display.d_Vrwg = 1.0f;
        display.d_Vrwb = 1.0f;
        display.d_Y0R = 0.0f;
        display.d_Y0G = 0.0f;
        display.d_Y0B = 0.0f;
        display.d_gammaR = 2.2f;
        display.d_gammaG = 2.2f;
        display.d_gammaB = 2.2f;

        if (Size >= 48)
        {
            refWhite[0] = read_f32(Data + 36);
            refWhite[1] = read_f32(Data + 40);
            refWhite[2] = read_f32(Data + 44);
        }
        else
        {
            refWhite[0] = 95.047f;
            refWhite[1] = 100.0f;
            refWhite[2] = 108.883f;
        }

        if (Size >= 60)
        {
            refBlack[0] = read_f32(Data + 48);
            refBlack[1] = read_f32(Data + 52);
            refBlack[2] = read_f32(Data + 56);
        }
        else
        {
            refBlack[0] = 0.0f;
            refBlack[1] = 0.0f;
            refBlack[2] = 0.0f;
        }

        cielab = (TIFFCIELabToRGB *)malloc(sizeof(*cielab));
        if (cielab != NULL)
        {
            if (TIFFCIELabToRGBInit(cielab, &display, refWhite) != 0)
            {
                TIFFCIELabToXYZ(cielab, L, a, b, &X, &Y, &Z);
                TIFFXYZToRGB(cielab, X, Y, Z, &r, &g, &bl);

                TIFFXYZToRGB(cielab, xyz[0], xyz[1], xyz[2], &r, &g, &bl);
                TIFFXYZToRGB(cielab, xyz2[0], xyz2[1], xyz2[2], &r, &g, &bl);

                TIFFCIELabToXYZ(cielab, u3 & 0xFFU,
                                (int32_t)(int8_t)(u3 >> 8),
                                (int32_t)(int8_t)(u3 >> 16),
                                &X, &Y, &Z);
                TIFFXYZToRGB(cielab, X, Y, Z, &r, &g, &bl);

                TIFFXYZToRGB(cielab, f0, f1, f2, &r, &g, &bl);
            }
            free(cielab);
        }
    }

    (void)r;
    (void)g;
    (void)bl;
    return 0;
}