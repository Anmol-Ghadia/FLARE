#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

static int pick_mode(uint8_t b) {
    switch (b % 8) {
        case 0: return -3;
        case 1: return -1;
        case 2: return 0;
        case 3: return 1;
        case 4: return 2;
        case 5: return 3;
        case 6: return 127;
        default: return (int)(int8_t)b;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    double y64 = 0.0;
    double y64b = 0.0;
    float xyz[3] = {0.0f, 0.0f, 0.0f};
    int em0 = 0, em1 = 1, em2 = -1;

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
        em0 = pick_mode(Data[12]);
    if (Size >= 14)
        em1 = pick_mode(Data[13]);
    if (Size >= 15)
        em2 = pick_mode(Data[14]);

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
        volatile uint32_t luv32a = LogLuv32fromXYZ(xyz, em0);
        volatile uint32_t luv24a = LogLuv24fromXYZ(xyz, em1);

        float xyz2[3];
        xyz2[0] = xyz[0];
        xyz2[1] = xyz[1];
        xyz2[2] = xyz[2];

        if (Size > 0) {
            switch (Data[0] % 6) {
                case 0:
                    xyz2[1] = 0.0f;
                    break;
                case 1:
                    xyz2[0] = -fabsf(xyz2[0]);
                    xyz2[1] = -fabsf(xyz2[1]);
                    xyz2[2] = -fabsf(xyz2[2]);
                    break;
                case 2:
                    xyz2[0] = INFINITY;
                    xyz2[1] = NAN;
                    xyz2[2] = -INFINITY;
                    break;
                case 3:
                    xyz2[0] = 0.0f;
                    xyz2[1] = 1.0e-30f;
                    xyz2[2] = 0.0f;
                    break;
                case 4:
                    xyz2[0] = 1.0e30f;
                    xyz2[1] = 1.0e30f;
                    xyz2[2] = 1.0e30f;
                    break;
                default:
                    xyz2[0] = 0.3127f;
                    xyz2[1] = 0.3290f;
                    xyz2[2] = 0.3583f;
                    break;
            }
        }

        volatile uint32_t luv32b = LogLuv32fromXYZ(xyz2, em2);
        volatile uint32_t luv24b = LogLuv24fromXYZ(xyz2, em0);

        (void)luv32a;
        (void)luv24a;
        (void)luv32b;
        (void)luv24b;
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