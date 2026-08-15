#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <tiffio.h>

static uint32_t read_u32(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static int read_i32(const uint8_t *p)
{
    int32_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static float read_f32(const uint8_t *p)
{
    float v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static double read_f64(const uint8_t *p)
{
    double v;
    memcpy(&v, p, sizeof(v));
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (Size == 0) {
        float xyz0[3] = {0.0f, 0.0f, 0.0f};
        float out0[3] = {0.0f, 0.0f, 0.0f};
        (void)LogL10fromY(0.0, 0);
        (void)LogL10toY(0);
        (void)LogL16fromY(0.0, 0);
        (void)LogL16toY(0);
        (void)LogLuv24fromXYZ(xyz0, 0);
        LogLuv32toXYZ(0, out0);
        return 0;
    }

    size_t off = 0;

    while (off < Size) {
        uint8_t tag = Data[off++];
        switch (tag % 6) {
            case 0: {
                double y = 0.0;
                int em = 0;
                if (off + sizeof(double) <= Size) {
                    y = read_f64(Data + off);
                    off += sizeof(double);
                } else {
                    y = (double)(int8_t)tag;
                }
                if (off + sizeof(int32_t) <= Size) {
                    em = read_i32(Data + off);
                    off += sizeof(int32_t);
                } else {
                    em = (int)(tag & 1);
                }

                (void)LogL10fromY(y, em);
                (void)LogL10fromY(-y, em);
                (void)LogL10fromY(0.0, em);
                (void)LogL10fromY(0.00024283, em);
                (void)LogL10fromY(15.742, em);
                break;
            }

            case 1: {
                int p10 = 0;
                if (off + sizeof(int32_t) <= Size) {
                    p10 = read_i32(Data + off);
                    off += sizeof(int32_t);
                } else {
                    p10 = (int)tag;
                }

                (void)LogL10toY(p10);
                (void)LogL10toY(p10 & 0x3ff);
                (void)LogL10toY(0);
                (void)LogL10toY(0x3ff);
                break;
            }

            case 2: {
                double y = 0.0;
                int em = 0;
                if (off + sizeof(double) <= Size) {
                    y = read_f64(Data + off);
                    off += sizeof(double);
                } else {
                    y = (double)(int8_t)tag;
                }
                if (off + sizeof(int32_t) <= Size) {
                    em = read_i32(Data + off);
                    off += sizeof(int32_t);
                } else {
                    em = (int)(tag & 3);
                }

                int enc = LogL16fromY(y, em);
                (void)LogL16toY(enc);
                (void)LogL16fromY(-y, em);
                (void)LogL16fromY(0.0, em);
                (void)LogL16fromY(1.0, em);
                break;
            }

            case 3: {
                int p16 = 0;
                if (off + sizeof(int32_t) <= Size) {
                    p16 = read_i32(Data + off);
                    off += sizeof(int32_t);
                } else {
                    p16 = (int)tag;
                }

                (void)LogL16toY(p16);
                (void)LogL16toY(p16 & 0xffff);
                (void)LogL16toY(0);
                (void)LogL16toY(0x8000);
                (void)LogL16toY(0xffff);
                break;
            }

            case 4: {
                float xyz[3] = {0.0f, 0.0f, 0.0f};
                int em = 0;
                if (off + 3 * sizeof(float) <= Size) {
                    xyz[0] = read_f32(Data + off);
                    off += sizeof(float);
                    xyz[1] = read_f32(Data + off);
                    off += sizeof(float);
                    xyz[2] = read_f32(Data + off);
                    off += sizeof(float);
                } else {
                    xyz[0] = (float)(tag);
                    xyz[1] = (float)(tag - 128);
                    xyz[2] = (float)(tag ^ 0x55);
                }
                if (off + sizeof(int32_t) <= Size) {
                    em = read_i32(Data + off);
                    off += sizeof(int32_t);
                } else {
                    em = (int)(tag & 7);
                }

                (void)LogLuv24fromXYZ(xyz, em);

                {
                    float xyz2[3] = { xyz[0], 0.0f, 0.0f };
                    (void)LogLuv24fromXYZ(xyz2, em);
                }
                {
                    float xyz3[3] = { 0.0f, xyz[1], xyz[2] };
                    (void)LogLuv24fromXYZ(xyz3, em);
                }
                {
                    float xyz4[3] = { -xyz[0], -xyz[1], -xyz[2] };
                    (void)LogLuv24fromXYZ(xyz4, em);
                }
                break;
            }

            case 5: {
                uint32_t packed = 0;
                float out[3] = {0.0f, 0.0f, 0.0f};
                if (off + sizeof(uint32_t) <= Size) {
                    packed = read_u32(Data + off);
                    off += sizeof(uint32_t);
                } else {
                    packed = (uint32_t)tag;
                }

                LogLuv32toXYZ(packed, out);
                LogLuv32toXYZ(0, out);
                LogLuv32toXYZ(0xffffffffU, out);
                LogLuv32toXYZ(packed ^ 0xAAAAAAAAU, out);
                break;
            }
        }
    }

    if (Size >= 12) {
        float xyz[3];
        xyz[0] = read_f32(Data);
        xyz[1] = read_f32(Data + 4);
        xyz[2] = read_f32(Data + 8);
        {
            uint32_t luv24 = LogLuv24fromXYZ(xyz, 0);
            float out[3] = {0.0f, 0.0f, 0.0f};
            LogLuv32toXYZ(luv24, out);
            (void)out[0];
        }
    }

    if (Size >= 8) {
        double y = read_f64(Data);
        int l10 = LogL10fromY(y, 0);
        double y10 = LogL10toY(l10 & 0x3ff);
        int l16 = LogL16fromY(y10, 0);
        (void)LogL16toY(l16);
    }

    return 0;
}