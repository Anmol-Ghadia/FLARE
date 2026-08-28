// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// TIFFYCbCrToRGBInit at tif_color.c:234:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <tiffio.h>

#ifndef TIFFroundup_32
#define TIFFroundup_32(x, y) ((((x) + ((y)-1)) / (y)) * (y))
#endif

static uint8_t get_u8(const uint8_t *Data, size_t Size, size_t *Off) {
    if (*Off >= Size) return 0;
    return Data[(*Off)++];
}

static uint32_t get_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | get_u8(Data, Size, Off);
    }
    return v;
}

static int32_t get_s32(const uint8_t *Data, size_t Size, size_t *Off) {
    return (int32_t)get_u32(Data, Size, Off);
}

static float get_floatish(const uint8_t *Data, size_t Size, size_t *Off) {
    union {
        uint32_t u;
        float f;
    } v;
    v.u = get_u32(Data, Size, Off);
    return v.f;
}

static float sanitize_positive_float(float f, float defval) {
    if (!isfinite(f) || f <= 0.0f)
        return defval;
    return f;
}

static float sanitize_unit_float(float f, float defval) {
    if (!isfinite(f))
        return defval;
    if (f < 0.0f) return 0.0f;
    if (f > 1.0f) return 1.0f;
    return f;
}

int LLVMFuzzerTestOneInput_54(const uint8_t *Data, size_t Size) {
    size_t off = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    TIFFDisplay display;
    memset(&display, 0, sizeof(display));

    display.d_mat[0][0] = sanitize_positive_float(get_floatish(Data, Size, &off), 3.2410f);
    display.d_mat[0][1] = sanitize_positive_float(get_floatish(Data, Size, &off), -1.5374f);
    display.d_mat[0][2] = sanitize_positive_float(get_floatish(Data, Size, &off), -0.4986f);
    display.d_mat[1][0] = sanitize_positive_float(get_floatish(Data, Size, &off), -0.9692f);
    display.d_mat[1][1] = sanitize_positive_float(get_floatish(Data, Size, &off), 1.8760f);
    display.d_mat[1][2] = sanitize_positive_float(get_floatish(Data, Size, &off), 0.0416f);
    display.d_mat[2][0] = sanitize_positive_float(get_floatish(Data, Size, &off), 0.0556f);
    display.d_mat[2][1] = sanitize_positive_float(get_floatish(Data, Size, &off), -0.2040f);
    display.d_mat[2][2] = sanitize_positive_float(get_floatish(Data, Size, &off), 1.0570f);

    display.d_YCR = sanitize_positive_float(get_floatish(Data, Size, &off), 100.0f);
    display.d_YCG = sanitize_positive_float(get_floatish(Data, Size, &off), 100.0f);
    display.d_YCB = sanitize_positive_float(get_floatish(Data, Size, &off), 100.0f);

    display.d_Vrwr = sanitize_positive_float(get_floatish(Data, Size, &off), 255.0f);
    display.d_Vrwg = sanitize_positive_float(get_floatish(Data, Size, &off), 255.0f);
    display.d_Vrwb = sanitize_positive_float(get_floatish(Data, Size, &off), 255.0f);

    display.d_Y0R = sanitize_positive_float(get_floatish(Data, Size, &off), 0.0f);
    display.d_Y0G = sanitize_positive_float(get_floatish(Data, Size, &off), 0.0f);
    display.d_Y0B = sanitize_positive_float(get_floatish(Data, Size, &off), 0.0f);

    display.d_gammaR = sanitize_positive_float(get_floatish(Data, Size, &off), 2.2f);
    display.d_gammaG = sanitize_positive_float(get_floatish(Data, Size, &off), 2.2f);
    display.d_gammaB = sanitize_positive_float(get_floatish(Data, Size, &off), 2.2f);

    float refWhite[3];
    refWhite[0] = sanitize_positive_float(get_floatish(Data, Size, &off), 95.047f);
    refWhite[1] = sanitize_positive_float(get_floatish(Data, Size, &off), 100.0f);
    refWhite[2] = sanitize_positive_float(get_floatish(Data, Size, &off), 108.883f);

    TIFFCIELabToRGB cielab;
    memset(&cielab, 0, sizeof(cielab));

    if (TIFFCIELabToRGBInit(&cielab, &display, refWhite)) {
        for (int i = 0; i < 4; i++) {
            uint32_t l = get_u32(Data, Size, &off);
            int32_t a = get_s32(Data, Size, &off);
            int32_t b = get_s32(Data, Size, &off);

            float X = 0.0f, Y = 0.0f, Z = 0.0f;
            TIFFCIELabToXYZ(&cielab, l, a, b, &X, &Y, &Z);

            if (isfinite(X) && isfinite(Y) && isfinite(Z)) {
                float xyz[3];
                xyz[0] = X;
                xyz[1] = Y;
                xyz[2] = Z;

                uint8_t rgb24[3] = {0, 0, 0};
                XYZtoRGB24(xyz, rgb24);

                uint32_t r = 0, g = 0, bl = 0;
                TIFFXYZToRGB(&cielab, X, Y, Z, &r, &g, &bl);

                (void)LogLuv24fromXYZ(xyz, (int)(get_u8(Data, Size, &off) & 1));

                xyz[0] = get_floatish(Data, Size, &off);
                xyz[1] = get_floatish(Data, Size, &off);
                xyz[2] = get_floatish(Data, Size, &off);

                if (!isfinite(xyz[0])) xyz[0] = 0.0f;
                if (!isfinite(xyz[1])) xyz[1] = 0.0f;
                if (!isfinite(xyz[2])) xyz[2] = 0.0f;

                XYZtoRGB24(xyz, rgb24);
                TIFFXYZToRGB(&cielab, xyz[0], xyz[1], xyz[2], &r, &g, &bl);
                (void)LogLuv24fromXYZ(xyz, (int)(get_u8(Data, Size, &off) & 3));
            }
        }
    }

    {
        float luma[3];
        float refBlackWhite[6];

        luma[0] = sanitize_unit_float(get_floatish(Data, Size, &off), 0.299f);
        luma[1] = sanitize_unit_float(get_floatish(Data, Size, &off), 0.587f);
        luma[2] = sanitize_unit_float(get_floatish(Data, Size, &off), 0.114f);

        refBlackWhite[0] = isfinite(get_floatish(Data, Size, &off)) ? get_floatish(Data, Size, &off) : 0.0f;
        refBlackWhite[1] = isfinite(get_floatish(Data, Size, &off)) ? get_floatish(Data, Size, &off) : 255.0f;
        refBlackWhite[2] = isfinite(get_floatish(Data, Size, &off)) ? get_floatish(Data, Size, &off) : 128.0f;
        refBlackWhite[3] = isfinite(get_floatish(Data, Size, &off)) ? get_floatish(Data, Size, &off) : 255.0f;
        refBlackWhite[4] = isfinite(get_floatish(Data, Size, &off)) ? get_floatish(Data, Size, &off) : 128.0f;
        refBlackWhite[5] = isfinite(get_floatish(Data, Size, &off)) ? get_floatish(Data, Size, &off) : 255.0f;

        size_t ycbcr_alloc = TIFFroundup_32(sizeof(TIFFYCbCrToRGB), sizeof(long)) + 65536;
        void *buf = malloc(ycbcr_alloc);
        if (buf) {
            memset(buf, 0, ycbcr_alloc);
            (void)TIFFYCbCrToRGBInit((TIFFYCbCrToRGB *)buf, luma, refBlackWhite);
            free(buf);
        }
    }

    return 0;
}