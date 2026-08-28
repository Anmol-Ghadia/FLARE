#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tiffio.h>

extern void LogLuv24toXYZ(uint32_t, float *);
extern void TIFFSwabArrayOfFloat(float *fp, tmsize_t n);
extern void LogLuv32toXYZ(uint32_t, float *);
extern void TIFFXYZToRGB(TIFFCIELabToRGB *, float, float, float,
                         uint32_t *, uint32_t *, uint32_t *);
extern uint32_t LogLuv24fromXYZ(float *, int);
extern void TIFFSwabFloat(float *);

static uint32_t read_u32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float read_f32(const uint8_t *p) {
    float f = 0.0f;
    if (sizeof(float) == 4) {
        memcpy(&f, p, 4);
    }
    return f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (sizeof(float) != 4) {
        return 0;
    }

    uint32_t packed24 = 0;
    uint32_t packed32 = 0;
    int em = 0;
    float xyz1[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float xyz3[3] = {0.0f, 0.0f, 0.0f};
    float arr[8] = {0.0f};
    uint32_t r = 0, g = 0, b = 0;
    TIFFCIELabToRGB *cielab = NULL;

    if (Size >= 4) {
        packed24 = read_u32(Data);
        packed32 = read_u32(Data);
    } else {
        for (size_t i = 0; i < Size; ++i) {
            packed24 |= ((uint32_t)Data[i]) << (8 * i);
            packed32 |= ((uint32_t)Data[i]) << (8 * i);
        }
    }

    if (Size >= 8) {
        em = (int)(read_u32(Data + 4) & 0x7fffffffU);
    } else if (Size > 4) {
        uint32_t tmp = 0;
        for (size_t i = 4; i < Size; ++i) {
            tmp |= ((uint32_t)Data[i]) << (8 * (i - 4));
        }
        em = (int)(tmp & 0x7fffffffU);
    }

    for (size_t i = 0; i < 8; ++i) {
        size_t off = i * 4;
        if (off + 4 <= Size) {
            arr[i] = read_f32(Data + off);
        } else {
            arr[i] = (float)(i + 1);
        }
    }

    LogLuv24toXYZ(packed24, xyz1);
    LogLuv32toXYZ(packed32, xyz2);

    uint32_t enc1 = LogLuv24fromXYZ(xyz1, em);
    uint32_t enc2 = LogLuv24fromXYZ(xyz2, em ^ 1);

    LogLuv24toXYZ(enc1, xyz3);
    LogLuv24toXYZ(enc2, xyz1);

    TIFFSwabFloat(&arr[0]);
    TIFFSwabFloat(&arr[1]);

    TIFFSwabArrayOfFloat(arr, (tmsize_t)0);
    TIFFSwabArrayOfFloat(arr, (tmsize_t)1);
    TIFFSwabArrayOfFloat(arr, (tmsize_t)3);
    TIFFSwabArrayOfFloat(arr, (tmsize_t)8);

    TIFFSwabArrayOfFloat(xyz1, (tmsize_t)3);
    TIFFSwabArrayOfFloat(xyz2, (tmsize_t)3);
    TIFFSwabArrayOfFloat(xyz3, (tmsize_t)3);

    TIFFXYZToRGB(cielab, xyz1[0], xyz1[1], xyz1[2], &r, &g, &b);
    TIFFXYZToRGB(cielab, xyz2[0], xyz2[1], xyz2[2], &r, &g, &b);
    TIFFXYZToRGB(cielab, xyz3[0], xyz3[1], xyz3[2], &r, &g, &b);

    if (Size >= 12) {
        float custom_xyz[3];
        custom_xyz[0] = read_f32(Data);
        custom_xyz[1] = read_f32(Data + 4);
        custom_xyz[2] = read_f32(Data + 8);

        uint32_t enc3 = LogLuv24fromXYZ(custom_xyz, em);
        LogLuv24toXYZ(enc3, custom_xyz);
        TIFFSwabArrayOfFloat(custom_xyz, (tmsize_t)3);
        TIFFXYZToRGB(cielab, custom_xyz[0], custom_xyz[1], custom_xyz[2], &r, &g, &b);
    }

    (void)r;
    (void)g;
    (void)b;
    return 0;
}