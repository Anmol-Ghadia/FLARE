// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "tiffio.h"

static uint32_t read_u32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float read_f32(const uint8_t *p) {
    uint32_t u = read_u32(p);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static float sanitize_float(float f, float fallback) {
    if (!isfinite(f))
        return fallback;
    if (f > 1.0e6f)
        return 1.0e6f;
    if (f < -1.0e6f)
        return -1.0e6f;
    return f;
}

static void make_safe_xyz(float dst[3], const float src[3]) {
    dst[0] = sanitize_float(src[0], 0.0f);
    dst[1] = sanitize_float(src[1], 0.0f);
    dst[2] = sanitize_float(src[2], 0.0f);

    if (dst[0] < 0.0f) dst[0] = 0.0f;
    if (dst[1] < 0.0f) dst[1] = 0.0f;
    if (dst[2] < 0.0f) dst[2] = 0.0f;

    if (dst[0] == 0.0f && dst[1] == 0.0f && dst[2] == 0.0f) {
        dst[0] = 0.1f;
        dst[1] = 0.1f;
        dst[2] = 0.1f;
    }
}

int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    uint32_t packed32 = 0;
    uint32_t packed24 = 0;
    float xyz[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float xyz3[3] = {0.0f, 0.0f, 0.0f};
    float safe_xyz[3] = {0.1f, 0.1f, 0.1f};
    uint8_t rgb24[3] = {0, 0, 0};
    int em = 0;

    if (Size >= 4)
        packed32 = read_u32(Data);
    if (Size >= 8)
        packed24 = read_u32(Data + 4) & 0x00FFFFFFU;
    else
        packed24 = packed32 & 0x00FFFFFFU;

    if (Size >= 20) {
        xyz[0] = read_f32(Data + 8);
        xyz[1] = read_f32(Data + 12);
        xyz[2] = read_f32(Data + 16);
    } else {
        xyz[0] = (float)((packed32 >> 0) & 0xFF) / 255.0f;
        xyz[1] = (float)((packed32 >> 8) & 0xFF) / 255.0f;
        xyz[2] = (float)((packed32 >> 16) & 0xFF) / 255.0f;
    }

    if (Size > 20)
        em = (int)(Data[20] & 1);

    make_safe_xyz(safe_xyz, xyz);

    LogLuv32toXYZ(packed32, xyz2);
    XYZtoRGB24(xyz2, rgb24);

    {
        uint32_t enc32 = LogLuv32fromXYZ(safe_xyz, em);
        float dec32[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb_from_dec32[3] = {0, 0, 0};
        LogLuv32toXYZ(enc32, dec32);
        XYZtoRGB24(dec32, rgb_from_dec32);
    }

    LogLuv24toXYZ(packed24, xyz3);
    XYZtoRGB24(xyz3, rgb24);

    {
        uint32_t enc24 = LogLuv24fromXYZ(safe_xyz, em);
        float dec24[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb_from_dec24[3] = {0, 0, 0};
        LogLuv24toXYZ(enc24, dec24);
        XYZtoRGB24(dec24, rgb_from_dec24);
    }

    {
        float varied_xyz[3];
        float varied_safe_xyz[3];

        varied_xyz[0] = isfinite(xyz[0]) ? fabsf(xyz[0]) : 0.1f;
        varied_xyz[1] = isfinite(xyz[1]) ? fabsf(xyz[1]) * 2.0f : 0.1f;
        varied_xyz[2] = isfinite(xyz[2]) ? fabsf(xyz[2]) + 0.01f : 0.1f;

        make_safe_xyz(varied_safe_xyz, varied_xyz);

        {
            uint32_t enc32b = LogLuv32fromXYZ(varied_safe_xyz, em ^ 1);
            float out1[3] = {0.0f, 0.0f, 0.0f};
            uint8_t rgb1[3] = {0, 0, 0};
            LogLuv32toXYZ(enc32b, out1);
            XYZtoRGB24(out1, rgb1);
        }

        {
            uint32_t enc24b = LogLuv24fromXYZ(varied_safe_xyz, em ^ 1);
            float out2[3] = {0.0f, 0.0f, 0.0f};
            uint8_t rgb2[3] = {0, 0, 0};
            LogLuv24toXYZ(enc24b, out2);
            XYZtoRGB24(out2, rgb2);
        }
    }

    return 0;
}