#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static uint64_t read_u64(const uint8_t *p) {
    return (uint64_t)p[0]
         | ((uint64_t)p[1] << 8)
         | ((uint64_t)p[2] << 16)
         | ((uint64_t)p[3] << 24)
         | ((uint64_t)p[4] << 32)
         | ((uint64_t)p[5] << 40)
         | ((uint64_t)p[6] << 48)
         | ((uint64_t)p[7] << 56);
}

static float read_f32(const uint8_t *p) {
    uint32_t u = read_u32(p);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static double read_f64(const uint8_t *p) {
    uint64_t u = read_u64(p);
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    double y = 0.0;
    double u = 0.0;
    double v = 0.0;
    double up = 0.0;
    double vp = 0.0;
    float xyz1[3] = {0.0f, 0.0f, 0.0f};
    float xyz2[3] = {0.0f, 0.0f, 0.0f};
    float xyz3[3] = {0.0f, 0.0f, 0.0f};
    int em1 = 0, em2 = 0, em3 = 0;
    int c = 0;
    uint32_t pix24 = 0;
    uint32_t pix32 = 0;

    if (Size >= 8) {
        y = read_f64(Data);
    } else if (Size > 0) {
        y = (double)((int8_t)Data[0]);
    }

    if (Size >= 16) {
        u = read_f64(Data + 8);
    } else if (Size > 1) {
        u = (double)((int8_t)Data[1]) / 16.0;
    }

    if (Size >= 24) {
        v = read_f64(Data + 16);
    } else if (Size > 2) {
        v = (double)((int8_t)Data[2]) / 16.0;
    }

    if (Size >= 28) {
        xyz1[0] = read_f32(Data + 24);
    }
    if (Size >= 32) {
        xyz1[1] = read_f32(Data + 28);
    }
    if (Size >= 36) {
        xyz1[2] = read_f32(Data + 32);
    }

    if (Size >= 40) {
        xyz2[0] = read_f32(Data + 36);
    } else {
        xyz2[0] = xyz1[0];
    }
    if (Size >= 44) {
        xyz2[1] = read_f32(Data + 40);
    } else {
        xyz2[1] = xyz1[1];
    }
    if (Size >= 48) {
        xyz2[2] = read_f32(Data + 44);
    } else {
        xyz2[2] = xyz1[2];
    }

    if (Size > 48) em1 = (int)((int8_t)Data[48]);
    if (Size > 49) em2 = (int)((int8_t)Data[49]);
    if (Size > 50) em3 = (int)((int8_t)Data[50]);

    if (Size >= 55) {
        c = (int)read_u32(Data + 51);
    } else if (Size > 51) {
        c = (int)((int8_t)Data[51]);
    }

    (void)LogL16fromY(y, em1);
    (void)LogL16fromY(-y, em2);
    (void)LogL16fromY(0.0, em3);
    (void)LogL16fromY(1.0, 0);
    (void)LogL16fromY(-1.0, 1);

    pix24 = LogLuv24fromXYZ(xyz1, em1);
    pix32 = LogLuv32fromXYZ(xyz2, em2);

    LogLuv32toXYZ(pix32, xyz3);
    (void)LogLuv24fromXYZ(xyz3, em3);
    (void)LogLuv32fromXYZ(xyz3, em1);

    c = uv_encode(u, v, em1);
    (void)uv_decode(&up, &vp, c);
    (void)uv_encode(up, vp, em2);

    (void)uv_decode(&up, &vp, -1);
    (void)uv_decode(&up, &vp, c + 1);
    (void)uv_decode(&up, &vp, 0);

    if (Size >= 4) {
        uint32_t arbitrary = read_u32(Data);
        LogLuv32toXYZ(arbitrary, xyz3);
        (void)LogLuv24fromXYZ(xyz3, em2);
        (void)LogLuv32fromXYZ(xyz3, em3);
    }

    xyz3[0] = 0.0f;
    xyz3[1] = 0.0f;
    xyz3[2] = 0.0f;
    (void)LogLuv24fromXYZ(xyz3, 0);
    pix32 = LogLuv32fromXYZ(xyz3, 1);
    LogLuv32toXYZ(pix32, xyz3);

    xyz3[0] = NAN;
    xyz3[1] = INFINITY;
    xyz3[2] = -INFINITY;
    (void)LogLuv24fromXYZ(xyz3, -1);
    pix32 = LogLuv32fromXYZ(xyz3, -2);
    LogLuv32toXYZ(pix32, xyz3);

    remove("./dummy_file");
    return 0;
}