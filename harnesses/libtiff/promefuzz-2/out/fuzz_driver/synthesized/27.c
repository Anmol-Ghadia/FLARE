// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv24fromXYZ at tif_luv.c:989:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tiffio.h"

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

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

static double read_f64(const uint8_t *p) {
    uint64_t u = ((uint64_t)p[0]) |
                 ((uint64_t)p[1] << 8) |
                 ((uint64_t)p[2] << 16) |
                 ((uint64_t)p[3] << 24) |
                 ((uint64_t)p[4] << 32) |
                 ((uint64_t)p[5] << 40) |
                 ((uint64_t)p[6] << 48) |
                 ((uint64_t)p[7] << 56);
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

static int pick_mode_safe(uint8_t b) {
    return (b & 1) ? 1 : 0;
}

static float finite_or_default(float v, float def) {
    return isfinite(v) ? v : def;
}

static double finite_or_default_d(double v, double def) {
    return isfinite(v) ? v : def;
}

static void sanitize_xyz(float xyz[3]) {
    xyz[0] = finite_or_default(xyz[0], 0.5f);
    xyz[1] = finite_or_default(xyz[1], 0.5f);
    xyz[2] = finite_or_default(xyz[2], 0.5f);

    if (xyz[0] < 0.0f) xyz[0] = -xyz[0];
    if (xyz[1] < 0.0f) xyz[1] = -xyz[1];
    if (xyz[2] < 0.0f) xyz[2] = -xyz[2];

    if (xyz[0] > 1.0e6f) xyz[0] = 1.0e6f;
    if (xyz[1] > 1.0e6f) xyz[1] = 1.0e6f;
    if (xyz[2] > 1.0e6f) xyz[2] = 1.0e6f;
}

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    double y64 = 0.0;
    double y64b = 1.0;
    float xyz[3] = {0.5f, 0.5f, 0.5f};
    int em0 = 0, em1 = 1, em2 = 0;

    if (Size >= 8)
        y64 = read_f64(Data);
    if (Size >= 16)
        y64b = read_f64(Data + 8);
    if (Size >= 4)
        xyz[0] = read_f32(Data);
    if (Size >= 8)
        xyz[1] = read_f32(Data + 4);
    if (Size >= 12)
        xyz[2] = read_f32(Data + 8);
    if (Size >= 13)
        em0 = pick_mode_safe(Data[12]);
    if (Size >= 14)
        em1 = pick_mode_safe(Data[13]);
    if (Size >= 15)
        em2 = pick_mode_safe(Data[14]);

    y64 = finite_or_default_d(y64, 0.0);
    y64b = finite_or_default_d(y64b, 1.0);
    sanitize_xyz(xyz);

    {
        volatile int l16a = LogL16fromY(y64, em0);
        volatile int l16b = LogL16fromY(y64b, em1);
        volatile int l10a = LogL10fromY(y64, em1);
        volatile int l10b = LogL10fromY(y64b, em2);

        volatile double ry16a = LogL16toY(l16a);
        volatile double ry16b = LogL16toY(l16b);
        volatile double ry10a = LogL10toY(l10a);
        volatile double ry10b = LogL10toY(l10b);

        (void)ry16a;
        (void)ry16b;
        (void)ry10a;
        (void)ry10b;
    }

    {
        float xyz_a[3] = {xyz[0], xyz[1], xyz[2]};
        float xyz_b[3] = {xyz[0], xyz[1], xyz[2]};
        float xyz_c[3] = {0.3127f, 0.3290f, 0.3583f};
        float xyz_d[3] = {1.0f, 1.0f, 1.0f};

        if (Size > 0) {
            switch (Data[0] % 4) {
                case 0:
                    xyz_b[1] = 0.0f;
                    break;
                case 1:
                    xyz_b[0] = 0.0f;
                    xyz_b[1] = 1.0e-6f;
                    xyz_b[2] = 0.0f;
                    break;
                case 2:
                    xyz_b[0] = 10.0f;
                    xyz_b[1] = 20.0f;
                    xyz_b[2] = 30.0f;
                    break;
                default:
                    xyz_b[0] = 0.2f;
                    xyz_b[1] = 0.3f;
                    xyz_b[2] = 0.4f;
                    break;
            }
        }

        sanitize_xyz(xyz_a);
        sanitize_xyz(xyz_b);
        sanitize_xyz(xyz_c);
        sanitize_xyz(xyz_d);

        volatile uint32_t luv32a = LogLuv32fromXYZ(xyz_a, em0);
        volatile uint32_t luv24a = LogLuv24fromXYZ(xyz_a, em1);
        volatile uint32_t luv32b = LogLuv32fromXYZ(xyz_b, em2);
        volatile uint32_t luv24b = LogLuv24fromXYZ(xyz_b, em0);
        volatile uint32_t luv32c = LogLuv32fromXYZ(xyz_c, 0);
        volatile uint32_t luv24c = LogLuv24fromXYZ(xyz_c, 1);
        volatile uint32_t luv32d = LogLuv32fromXYZ(xyz_d, 0);
        volatile uint32_t luv24d = LogLuv24fromXYZ(xyz_d, 1);

        (void)luv32a;
        (void)luv24a;
        (void)luv32b;
        (void)luv24b;
        (void)luv32c;
        (void)luv24c;
        (void)luv32d;
        (void)luv24d;
    }

    {
        int samples[12];
        samples[0] = 0;
        samples[1] = 1;
        samples[2] = 0x3ff;
        samples[3] = 0xffff;
        samples[4] = 0x7fff;
        samples[5] = 0x8000;
        samples[6] = (Size >= 2) ? (int)read_u16(Data) : 123;
        samples[7] = (Size >= 4) ? (int)read_u16(Data + 2) : 456;
        samples[8] = -1;
        samples[9] = 1024;
        samples[10] = 32768;
        samples[11] = -32768;

        for (size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
            volatile double a = LogL10toY(samples[i]);
            volatile double b = LogL16toY(samples[i]);
            (void)a;
            (void)b;
        }
    }

    {
        double ys[8];
        ys[0] = 0.0;
        ys[1] = -0.0;
        ys[2] = 0.00024283;
        ys[3] = 15.742;
        ys[4] = -1.0;
        ys[5] = 1.0;
        ys[6] = INFINITY;
        ys[7] = NAN;

        for (size_t i = 0; i < sizeof(ys) / sizeof(ys[0]); ++i) {
            volatile int a = LogL10fromY(ys[i], em0);
            volatile int b = LogL16fromY(ys[i], em1);
            (void)a;
            (void)b;
        }
    }

    remove("./dummy_file");
    return 0;
}