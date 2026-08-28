// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
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

static float bytes_to_float(const uint8_t *p) {
    uint32_t u = read_u32(p);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static double bytes_to_double(const uint8_t *p) {
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

int LLVMFuzzerTestOneInput_41(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    double y = 0.0;
    float xyz[3] = {0.0f, 0.0f, 0.0f};
    int em = 0;
    int p10 = 0;
    int p16 = 0;
    uint32_t packed32 = 0;

    if (Size >= 8) {
        y = bytes_to_double(Data);
    } else if (Size > 0) {
        uint64_t acc = 0;
        size_t i;
        for (i = 0; i < Size; ++i) {
            acc |= ((uint64_t)Data[i] << (i * 8));
        }
        memcpy(&y, &acc, sizeof(y) < sizeof(acc) ? sizeof(y) : sizeof(acc));
    }

    if (Size >= 20) {
        xyz[0] = bytes_to_float(Data + 8);
        xyz[1] = bytes_to_float(Data + 12);
        xyz[2] = bytes_to_float(Data + 16);
    } else if (Size >= 12) {
        xyz[0] = bytes_to_float(Data + 0);
        xyz[1] = bytes_to_float(Data + 4);
        xyz[2] = bytes_to_float(Data + 8);
    } else if (Size > 0) {
        xyz[0] = (float)y;
        xyz[1] = (float)(-y);
        xyz[2] = (float)(y * 0.5);
    }

    if (Size >= 21) {
        em = (int8_t)Data[20];
    } else if (Size > 0) {
        em = (int8_t)Data[Size - 1];
    }

    if (Size >= 23) {
        p10 = (int)(read_u16(Data + 21) & 0x3ff);
    } else if (Size >= 2) {
        p10 = (int)(read_u16(Data) & 0x3ff);
    }

    if (Size >= 25) {
        p16 = (int)read_u16(Data + 23);
    } else if (Size >= 4) {
        p16 = (int)read_u16(Data + 2);
    }

    if (Size >= 29) {
        packed32 = read_u32(Data + 25);
    } else if (Size >= 4) {
        packed32 = read_u32(Data);
    }

    {
        int l16 = LogL16fromY(y, em);
        double y_from_l16 = LogL16toY(l16);
        int l10 = LogL10fromY(y, em);
        double y_from_l10 = LogL10toY(l10);

        float out_xyz[3] = {0.0f, 0.0f, 0.0f};
        uint32_t enc32 = LogLuv32fromXYZ(xyz, em);
        LogLuv32toXYZ(enc32, out_xyz);

        (void)y_from_l16;
        (void)y_from_l10;
        (void)out_xyz;
    }

    {
        double d1 = LogL10toY(p10);
        double d2 = LogL16toY(p16);
        int e1 = LogL10fromY(d1, em);
        int e2 = LogL16fromY(d2, em);

        float out_xyz2[3] = {0.0f, 0.0f, 0.0f};
        LogLuv32toXYZ(packed32, out_xyz2);
        uint32_t reenc = LogLuv32fromXYZ(out_xyz2, em);

        (void)e1;
        (void)e2;
        (void)reenc;
    }

    {
        double vals[] = {
            y,
            -y,
            0.0,
            0.00024283,
            15.742,
            1.0,
            -1.0,
            INFINITY,
            -INFINITY,
            NAN
        };
        size_t i;
        for (i = 0; i < sizeof(vals) / sizeof(vals[0]); ++i) {
            int a = LogL10fromY(vals[i], em);
            int b = LogL16fromY(vals[i], em);
            double c = LogL10toY(a & 0x3ff);
            double d = LogL16toY(b & 0xffff);
            (void)c;
            (void)d;
        }
    }

    {
        float xyz_variants[4][3] = {
            { xyz[0], xyz[1], xyz[2] },
            { 0.0f, 0.0f, 0.0f },
            { -xyz[0], -xyz[1], -xyz[2] },
            { NAN, INFINITY, -INFINITY }
        };
        size_t i;
        for (i = 0; i < 4; ++i) {
            float tmp[3] = { xyz_variants[i][0], xyz_variants[i][1], xyz_variants[i][2] };
            uint32_t enc = LogLuv32fromXYZ(tmp, em);
            float dec[3] = {0.0f, 0.0f, 0.0f};
            LogLuv32toXYZ(enc, dec);
        }
    }

    return 0;
}