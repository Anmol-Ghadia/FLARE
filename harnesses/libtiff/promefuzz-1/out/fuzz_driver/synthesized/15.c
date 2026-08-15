// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv24toXYZ at tif_luv.c:961:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tiffio.h"

static uint32_t read_u32(const uint8_t *p) {
    uint32_t v = 0;
    v |= (uint32_t)p[0];
    v |= (uint32_t)p[1] << 8;
    v |= (uint32_t)p[2] << 16;
    v |= (uint32_t)p[3] << 24;
    return v;
}

static int32_t read_i32(const uint8_t *p) {
    return (int32_t)read_u32(p);
}

static float make_float_from_u32(uint32_t u) {
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static float sanitize_float(float f, float fallback) {
    if (!isfinite(f))
        return fallback;
    return f;
}

static void clamp_xyz(float xyz[3]) {
    size_t i;
    for (i = 0; i < 3; i++) {
        if (!isfinite(xyz[i])) {
            xyz[i] = 0.0f;
        } else {
            if (xyz[i] < 0.0f)
                xyz[i] = 0.0f;
            if (xyz[i] > 1.0e20f)
                xyz[i] = 1.0e20f;
        }
    }
}

static int safe_em(uint32_t v) {
    return (int)(v % 33U);
}

int LLVMFuzzerTestOneInput_15(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        uint8_t buf[32];
        uint32_t u0, u1, u2, u3, u4, u5;
        int32_t a, b;
        int em1, em2;
        float xyz1[3], xyz2[3], xyz3[3];
        float out24[3] = {0.0f, 0.0f, 0.0f};
        float out32[3] = {0.0f, 0.0f, 0.0f};
        float mixed24[3] = {0.0f, 0.0f, 0.0f};
        float mixed32[3] = {0.0f, 0.0f, 0.0f};
        float X = 0.0f, Y = 0.0f, Z = 0.0f;
        uint32_t ll24, ll32, ll24_lab, ll32_lab;
        TIFFCIELabToRGB cielab;

        memset(buf, 0, sizeof(buf));
        if (Size > sizeof(buf))
            Size = sizeof(buf);
        if (Size > 0)
            memcpy(buf, Data, Size);

        u0 = read_u32(buf + 0);
        u1 = read_u32(buf + 4);
        u2 = read_u32(buf + 8);
        u3 = read_u32(buf + 12);
        u4 = read_u32(buf + 16);
        u5 = read_u32(buf + 20);

        em1 = safe_em(u3);
        em2 = safe_em(u3 >> 8);

        xyz1[0] = sanitize_float(make_float_from_u32(u0), 0.0f);
        xyz1[1] = sanitize_float(make_float_from_u32(u1), 0.0f);
        xyz1[2] = sanitize_float(make_float_from_u32(u2), 0.0f);
        clamp_xyz(xyz1);

        xyz2[0] = xyz1[0];
        xyz2[1] = xyz1[1];
        xyz2[2] = xyz1[2];

        ll24 = LogLuv24fromXYZ(xyz1, em1);
        ll32 = LogLuv32fromXYZ(xyz2, em2);

        LogLuv24toXYZ(ll24, out24);
        LogLuv32toXYZ(ll32, out32);

        LogLuv24toXYZ(u4 & 0x00FFFFFFU, mixed24);
        LogLuv32toXYZ(u5, mixed32);

        TIFFSwabFloat(&xyz1[0]);
        TIFFSwabFloat(&xyz1[1]);
        TIFFSwabFloat(&xyz1[2]);
        clamp_xyz(xyz1);

        TIFFSwabFloat(&out24[0]);
        TIFFSwabFloat(&out32[1]);

        memset(&cielab, 0, sizeof(cielab));
        cielab.range = 1500.0f;
        cielab.rstep = 1.0f;
        cielab.gstep = 1.0f;
        cielab.bstep = 1.0f;
        cielab.X0 = sanitize_float(fabsf(make_float_from_u32(u0)), 95.047f);
        cielab.Y0 = sanitize_float(fabsf(make_float_from_u32(u1)), 100.0f);
        cielab.Z0 = sanitize_float(fabsf(make_float_from_u32(u2)), 108.883f);
        if (cielab.X0 <= 0.0f) cielab.X0 = 95.047f;
        if (cielab.Y0 <= 0.0f) cielab.Y0 = 100.0f;
        if (cielab.Z0 <= 0.0f) cielab.Z0 = 108.883f;

        a = read_i32(buf + 4);
        b = read_i32(buf + 8);
        TIFFCIELabToXYZ(&cielab, u0, a, b, &X, &Y, &Z);

        xyz3[0] = sanitize_float(X, 0.0f);
        xyz3[1] = sanitize_float(Y, 0.0f);
        xyz3[2] = sanitize_float(Z, 0.0f);
        clamp_xyz(xyz3);

        ll24_lab = LogLuv24fromXYZ(xyz3, safe_em(u1));
        ll32_lab = LogLuv32fromXYZ(xyz3, safe_em(u2));
        LogLuv24toXYZ(ll24_lab, out24);
        LogLuv32toXYZ(ll32_lab, out32);

        TIFFSwabFloat(&X);
        TIFFSwabFloat(&Y);
        TIFFSwabFloat(&Z);
    }

    return 0;
}