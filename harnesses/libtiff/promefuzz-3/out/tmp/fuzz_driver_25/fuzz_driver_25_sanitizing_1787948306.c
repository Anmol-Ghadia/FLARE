#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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
        xyz[0] = (float)((packed32 >>  0) & 0xFF) / 255.0f;
        xyz[1] = (float)((packed32 >>  8) & 0xFF) / 255.0f;
        xyz[2] = (float)((packed32 >> 16) & 0xFF) / 255.0f;
    }

    if (Size > 20)
        em = (int)(Data[20] % 8) - 4;

    LogLuv32toXYZ(packed32, xyz2);
    XYZtoRGB24(xyz2, rgb24);

    {
        uint32_t enc32 = LogLuv32fromXYZ(xyz, em);
        float dec32[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb_from_dec32[3] = {0, 0, 0};
        LogLuv32toXYZ(enc32, dec32);
        XYZtoRGB24(dec32, rgb_from_dec32);
    }

    LogLuv24toXYZ(packed24, xyz3);
    XYZtoRGB24(xyz3, rgb24);

    {
        uint32_t enc24 = LogLuv24fromXYZ(xyz, em);
        float dec24[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb_from_dec24[3] = {0, 0, 0};
        LogLuv24toXYZ(enc24, dec24);
        XYZtoRGB24(dec24, rgb_from_dec24);
    }

    {
        float varied_xyz[3];
        varied_xyz[0] = isfinite(xyz[0]) ? -xyz[0] : 0.0f;
        varied_xyz[1] = isfinite(xyz[1]) ? xyz[1] * 2.0f : 0.0f;
        varied_xyz[2] = isfinite(xyz[2]) ? xyz[2] - 1.0f : 0.0f;

        uint32_t enc32b = LogLuv32fromXYZ(varied_xyz, -em);
        uint32_t enc24b = LogLuv24fromXYZ(varied_xyz, em + 1);
        float out1[3] = {0.0f, 0.0f, 0.0f};
        float out2[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb1[3] = {0, 0, 0};
        uint8_t rgb2[3] = {0, 0, 0};

        LogLuv32toXYZ(enc32b, out1);
        LogLuv24toXYZ(enc24b, out2);
        XYZtoRGB24(out1, rgb1);
        XYZtoRGB24(out2, rgb2);
    }

    if (Size >= sizeof(TIFFCIELabToRGB)) {
        TIFFCIELabToRGB cielab;
        uint32_t r = 0, g = 0, b = 0;
        memset(&cielab, 0, sizeof(cielab));
        memcpy(&cielab, Data, sizeof(cielab));
        TIFFXYZToRGB(&cielab, xyz[0], xyz[1], xyz[2], &r, &g, &b);
        TIFFXYZToRGB(&cielab, xyz2[0], xyz2[1], xyz2[2], &r, &g, &b);
        TIFFXYZToRGB(&cielab, xyz3[0], xyz3[1], xyz3[2], &r, &g, &b);
    }

    return 0;
}