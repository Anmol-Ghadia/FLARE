#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

#ifndef UV_NDIVS
#define UV_NDIVS 16289
#endif

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t Off) {
    uint32_t v = 0;
    if (Off < Size) v |= (uint32_t)Data[Off];
    if (Off + 1 < Size) v |= (uint32_t)Data[Off + 1] << 8;
    if (Off + 2 < Size) v |= (uint32_t)Data[Off + 2] << 16;
    if (Off + 3 < Size) v |= (uint32_t)Data[Off + 3] << 24;
    return v;
}

static int32_t read_s32(const uint8_t *Data, size_t Size, size_t Off) {
    return (int32_t)read_u32(Data, Size, Off);
}

static float make_float(const uint8_t *Data, size_t Size, size_t Off) {
    union {
        uint32_t u;
        float f;
    } cvt;
    cvt.u = read_u32(Data, Size, Off);
    return cvt.f;
}

static double clamp_double(double x, double lo, double hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    float xyz1[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float xyz3[3] = {0.0f, 0.0f, 0.0f};
    float xyz4[3] = {0.0f, 0.0f, 0.0f};

    xyz1[0] = make_float(Data, Size, 0);
    xyz1[1] = make_float(Data, Size, 4);
    xyz1[2] = make_float(Data, Size, 8);

    xyz2[0] = (Size > 0) ? (float)((int8_t)Data[0]) : 0.0f;
    xyz2[1] = (Size > 1) ? (float)((int8_t)Data[1]) : 0.0f;
    xyz2[2] = (Size > 2) ? (float)((int8_t)Data[2]) : 0.0f;

    xyz3[0] = 0.0f;
    xyz3[1] = -1.0f;
    xyz3[2] = 1.0f;

    xyz4[0] = 1.0e30f;
    xyz4[1] = (Size > 3 && (Data[3] & 1)) ? 1.0e-30f : 1.0e30f;
    xyz4[2] = -1.0e30f;

    {
        int em1 = read_s32(Data, Size, 12);
        int em2 = (Size > 16) ? ((int)(int8_t)Data[16]) : 0;
        int em3 = 0;
        int em4 = 1;
        int em5 = -1;

        uint32_t p24_1 = LogLuv24fromXYZ(xyz1, em1);
        uint32_t p24_2 = LogLuv24fromXYZ(xyz2, em2);
        uint32_t p24_3 = LogLuv24fromXYZ(xyz3, em3);
        uint32_t p24_4 = LogLuv24fromXYZ(xyz4, em4);

        uint32_t p32_1 = LogLuv32fromXYZ(xyz1, em1);
        uint32_t p32_2 = LogLuv32fromXYZ(xyz2, em2);
        uint32_t p32_3 = LogLuv32fromXYZ(xyz3, em5);
        uint32_t p32_4 = LogLuv32fromXYZ(xyz4, em4);

        LogLuv24toXYZ(p24_1, xyz1);
        LogLuv24toXYZ(p24_2, xyz2);
        LogLuv24toXYZ(p24_3, xyz3);
        LogLuv24toXYZ(p24_4, xyz4);

        {
            float roundtrip1[3] = {0.0f, 0.0f, 0.0f};
            float roundtrip2[3] = {0.0f, 0.0f, 0.0f};
            LogLuv24toXYZ(p32_1 & 0xFFFFFFu, roundtrip1);
            LogLuv24toXYZ(p32_2 & 0xFFFFFFu, roundtrip2);
        }

        {
            double y1 = (double)make_float(Data, Size, 20);
            double y2 = (Size > 24) ? (double)((int8_t)Data[24]) : 0.0;
            double y3 = 0.0;
            double y4 = -1.0;
            double y5 = 1.0e308;
            double y6 = -1.0e308;

            (void)LogL16fromY(y1, em1);
            (void)LogL16fromY(y2, em2);
            (void)LogL16fromY(y3, em3);
            (void)LogL16fromY(y4, em4);
            (void)LogL16fromY(y5, em5);
            (void)LogL16fromY(y6, em1);
        }

        {
            double du = 0.0, dv = 0.0;
            double u1 = clamp_double((double)make_float(Data, Size, 28), -0.5, 0.7);
            double v1 = clamp_double((double)make_float(Data, Size, 32), 0.02, 0.7);
            double u2 = 0.21052631578947367;
            double v2 = 0.47368421052631576;
            double u3 = -1.0;
            double v3 = -1.0;
            double u4 = 0.62;
            double v4 = 0.65;

            int c1 = uv_encode(u1, v1, em1);
            int c2 = uv_encode(u2, v2, em2);
            int c3 = uv_encode(u3, v3, em3);
            int c4 = uv_encode(u4, v4, em4);

            if (c1 >= 0 && c1 < UV_NDIVS) (void)uv_decode(&du, &dv, c1);
            if (c2 >= 0 && c2 < UV_NDIVS) (void)uv_decode(&du, &dv, c2);
            if (c3 >= 0 && c3 < UV_NDIVS) (void)uv_decode(&du, &dv, c3);
            if (c4 >= 0 && c4 < UV_NDIVS) (void)uv_decode(&du, &dv, c4);

            {
                int idx1 = (Size > 36) ? (int)(read_u32(Data, Size, 36) % UV_NDIVS) : 0;
                int idx2 = -1;
                int idx3 = UV_NDIVS;
                int idx4 = UV_NDIVS - 1;

                (void)uv_decode(&du, &dv, idx1);
                (void)uv_decode(&du, &dv, idx2);
                (void)uv_decode(&du, &dv, idx3);
                (void)uv_decode(&du, &dv, idx4);

                (void)uv_encode(clamp_double(du, -0.5, 0.7),
                                clamp_double(dv, 0.02, 0.7), em5);
            }
        }
    }

    return 0;
}