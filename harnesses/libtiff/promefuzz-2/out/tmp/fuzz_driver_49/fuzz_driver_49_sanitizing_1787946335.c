#include <stdint.h>
#include <stddef.h>
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
    memcpy(&f, p, 4);
    return f;
}

static int safe_em_from_data(const uint8_t *Data, size_t Size) {
    uint32_t v = 0;
    if (Size >= 8) {
        v = read_u32(Data + 4);
    } else if (Size > 4) {
        for (size_t i = 4; i < Size; ++i) {
            v |= ((uint32_t)Data[i]) << (8 * (i - 4));
        }
    }
    return (int)(v & 1U);
}

static int finite_positive_xyz(const float *xyz) {
    return xyz[0] >= 0.0f && xyz[1] > 0.0f && xyz[2] >= 0.0f &&
           xyz[0] < 1.0e20f && xyz[1] < 1.0e20f && xyz[2] < 1.0e20f;
}

static void sanitize_xyz(float *xyz) {
    size_t i;
    for (i = 0; i < 3; ++i) {
        if (!(xyz[i] == xyz[i])) {
            xyz[i] = 0.0f;
        } else if (xyz[i] < 0.0f) {
            xyz[i] = 0.0f;
        } else if (xyz[i] > 1.0e6f) {
            xyz[i] = 1.0e6f;
        } else if (xyz[i] < -1.0e6f) {
            xyz[i] = 0.0f;
        }
    }
    if (xyz[1] <= 0.0f) {
        xyz[1] = 1.0f;
    }
}

static uint32_t safe_LogLuv24fromXYZ(float *xyz, int em) {
    float tmp[3];
    memcpy(tmp, xyz, sizeof(tmp));
    sanitize_xyz(tmp);
    if (!finite_positive_xyz(tmp)) {
        tmp[0] = 0.5f;
        tmp[1] = 1.0f;
        tmp[2] = 0.5f;
    }
    return LogLuv24fromXYZ(tmp, em & 1);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (sizeof(float) != 4) {
        return 0;
    }

    uint32_t packed24 = 0;
    uint32_t packed32 = 0;
    int em = safe_em_from_data(Data, Size);
    float xyz1[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float xyz3[3] = {0.0f, 0.0f, 0.0f};
    float arr[8] = {0.0f};
    uint32_t r = 0, g = 0, b = 0;

    if (Size >= 4) {
        packed24 = read_u32(Data);
        packed32 = read_u32(Data);
    } else {
        size_t i;
        for (i = 0; i < Size; ++i) {
            packed24 |= ((uint32_t)Data[i]) << (8 * i);
            packed32 |= ((uint32_t)Data[i]) << (8 * i);
        }
    }

    {
        size_t i;
        for (i = 0; i < 8; ++i) {
            size_t off = i * 4;
            if (off + 4 <= Size) {
                arr[i] = read_f32(Data + off);
            } else {
                arr[i] = (float)(i + 1);
            }
        }
    }

    LogLuv24toXYZ(packed24, xyz1);
    LogLuv32toXYZ(packed32, xyz2);

    {
        uint32_t enc1 = safe_LogLuv24fromXYZ(xyz1, em);
        uint32_t enc2 = safe_LogLuv24fromXYZ(xyz2, em ^ 1);

        LogLuv24toXYZ(enc1, xyz3);
        LogLuv24toXYZ(enc2, xyz1);
    }

    TIFFSwabFloat(&arr[0]);
    TIFFSwabFloat(&arr[1]);

    TIFFSwabArrayOfFloat(arr, (tmsize_t)0);
    TIFFSwabArrayOfFloat(arr, (tmsize_t)1);
    TIFFSwabArrayOfFloat(arr, (tmsize_t)3);
    TIFFSwabArrayOfFloat(arr, (tmsize_t)8);

    TIFFSwabArrayOfFloat(xyz1, (tmsize_t)3);
    TIFFSwabArrayOfFloat(xyz2, (tmsize_t)3);
    TIFFSwabArrayOfFloat(xyz3, (tmsize_t)3);

    if (Size >= 12) {
        float custom_xyz[3];
        custom_xyz[0] = read_f32(Data);
        custom_xyz[1] = read_f32(Data + 4);
        custom_xyz[2] = read_f32(Data + 8);

        {
            uint32_t enc3 = safe_LogLuv24fromXYZ(custom_xyz, em);
            LogLuv24toXYZ(enc3, custom_xyz);
        }

        TIFFSwabArrayOfFloat(custom_xyz, (tmsize_t)3);
    }

    (void)TIFFXYZToRGB;
    (void)r;
    (void)g;
    (void)b;
    return 0;
}