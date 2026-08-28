#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <tiffio.h>

extern void LogLuv32toXYZ(uint32_t, float *);
extern int uv_encode(double, double, int);
extern int uv_decode(double *, double *, int);

#ifndef D65_X0
#define D65_X0 95.047f
#endif
#ifndef D65_Y0
#define D65_Y0 100.0f
#endif
#ifndef D65_Z0
#define D65_Z0 108.883f
#endif

#define YCBCR_EXTRA_BYTES (16u * 1024u)

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float rd_floatish(const uint8_t *p) {
    int32_t v = (int32_t)rd32(p);
    return (float)(v % 200000) / 1000.0f;
}

static double rd_doubleish(const uint8_t *p) {
    int32_t v = (int32_t)rd32(p);
    return (double)(v % 200000) / 1000.0;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Data && Size)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    {
        uint32_t packed = 0;
        float xyz[3] = {0.0f, 0.0f, 0.0f};
        if (Size >= 4)
            packed = rd32(Data);
        LogLuv32toXYZ(packed, xyz);

        if (Size >= 8) {
            packed ^= rd32(Data + 4);
            LogLuv32toXYZ(packed, xyz);
        }
    }

    {
        double u = 0.0, v = 0.0;
        int em = 0;
        if (Size >= 4)
            u = rd_doubleish(Data);
        if (Size >= 8)
            v = rd_doubleish(Data + 4);
        if (Size >= 12)
            em = (int)(rd32(Data + 8) % 7) - 3;

        int code = uv_encode(u, v, em);

        double du = 0.0, dv = 0.0;
        (void)uv_decode(&du, &dv, code);
        (void)uv_decode(&du, &dv, -1);
        (void)uv_decode(&du, &dv, INT_MAX);

        if (code >= 0) {
            int code2 = uv_encode(du, dv, em);
            (void)code2;
        }

        (void)uv_encode(0.210526316, 0.473684211, em);
        (void)uv_encode(0.125, 0.157894737, em);
        (void)uv_encode(0.175, 0.5625, em);
        (void)uv_encode(1000.0, -1000.0, em);
    }

    {
        TIFFCIELabToRGB *cielab = NULL;
        TIFFDisplay display;
        float refWhite[3];
        float xyz[3] = {0.0f, 0.0f, 0.0f};
        uint32_t r = 0, g = 0, b = 0;

        memset(&display, 0, sizeof(display));
        memset(refWhite, 0, sizeof(refWhite));

        if (Size >= 36) {
            size_t off = 0;
            display.d_mat[0][0] = rd_floatish(Data + off); off += 4;
            display.d_mat[0][1] = rd_floatish(Data + off); off += 4;
            display.d_mat[0][2] = rd_floatish(Data + off); off += 4;
            display.d_mat[1][0] = rd_floatish(Data + off); off += 4;
            display.d_mat[1][1] = rd_floatish(Data + off); off += 4;
            display.d_mat[1][2] = rd_floatish(Data + off); off += 4;
            display.d_mat[2][0] = rd_floatish(Data + off); off += 4;
            display.d_mat[2][1] = rd_floatish(Data + off); off += 4;
            display.d_mat[2][2] = rd_floatish(Data + off); off += 4;
        } else {
            display.d_mat[0][0] = 3.2406f;  display.d_mat[0][1] = -1.5372f; display.d_mat[0][2] = -0.4986f;
            display.d_mat[1][0] = -0.9689f; display.d_mat[1][1] = 1.8758f;  display.d_mat[1][2] = 0.0415f;
            display.d_mat[2][0] = 0.0557f;  display.d_mat[2][1] = -0.2040f; display.d_mat[2][2] = 1.0570f;
        }

        if (Size >= 48) {
            refWhite[0] = rd_floatish(Data + 36);
            refWhite[1] = rd_floatish(Data + 40);
            refWhite[2] = rd_floatish(Data + 44);
        } else {
            refWhite[0] = D65_X0;
            refWhite[1] = D65_Y0;
            refWhite[2] = D65_Z0;
        }

        if (Size >= 60) {
            display.d_YCR = rd_floatish(Data + 48);
            display.d_YCG = rd_floatish(Data + 52);
            display.d_YCB = rd_floatish(Data + 56);
        } else {
            display.d_YCR = 100.0f;
            display.d_YCG = 100.0f;
            display.d_YCB = 100.0f;
        }

        if (Size >= 84) {
            display.d_Vrwr = rd_floatish(Data + 60);
            display.d_Vrwg = rd_floatish(Data + 64);
            display.d_Vrwb = rd_floatish(Data + 68);
            display.d_Y0R = rd_floatish(Data + 72);
            display.d_Y0G = rd_floatish(Data + 76);
            display.d_Y0B = rd_floatish(Data + 80);
        } else {
            display.d_Vrwr = 1.0f;
            display.d_Vrwg = 1.0f;
            display.d_Vrwb = 1.0f;
            display.d_Y0R = 0.0f;
            display.d_Y0G = 0.0f;
            display.d_Y0B = 0.0f;
        }

        cielab = (TIFFCIELabToRGB *)_TIFFmalloc(sizeof(TIFFCIELabToRGB));
        if (cielab != NULL) {
            memset(cielab, 0, sizeof(TIFFCIELabToRGB));
            if (TIFFCIELabToRGBInit(cielab, &display, refWhite)) {
                if (Size >= 88) {
                    uint32_t packed = rd32(Data + 84);
                    LogLuv32toXYZ(packed, xyz);
                } else {
                    xyz[0] = D65_X0;
                    xyz[1] = D65_Y0;
                    xyz[2] = D65_Z0;
                }

                TIFFXYZToRGB(cielab, xyz[0], xyz[1], xyz[2], &r, &g, &b);
                TIFFXYZToRGB(cielab, -1.0f, -1.0f, -1.0f, &r, &g, &b);
                TIFFXYZToRGB(cielab, 1e9f, 1e9f, 1e9f, &r, &g, &b);

                if (Size >= 100) {
                    float x = rd_floatish(Data + 88);
                    float y = rd_floatish(Data + 92);
                    float z = rd_floatish(Data + 96);
                    TIFFXYZToRGB(cielab, x, y, z, &r, &g, &b);
                }
            }
            _TIFFfree(cielab);
        }
    }

    {
        float luma[3] = {299.0f, 587.0f, 114.0f};
        float refBlackWhite[6] = {0.0f, 255.0f, 128.0f, 255.0f, 128.0f, 255.0f};
        size_t alloc_size = ((sizeof(TIFFYCbCrToRGB) + sizeof(long) - 1) / sizeof(long)) * sizeof(long);
        alloc_size += YCBCR_EXTRA_BYTES;

        if (Size >= 12) {
            luma[0] = rd_floatish(Data + 0);
            luma[1] = rd_floatish(Data + 4);
            luma[2] = rd_floatish(Data + 8);
        }
        if (Size >= 36) {
            for (int i = 0; i < 6; i++)
                refBlackWhite[i] = rd_floatish(Data + 12 + i * 4);
        }

        void *mem = malloc(alloc_size);
        if (mem != NULL) {
            memset(mem, 0, alloc_size);
            TIFFYCbCrToRGB *ycbcr = (TIFFYCbCrToRGB *)mem;
            if (TIFFYCbCrToRGBInit(ycbcr, luma, refBlackWhite)) {
                uint32_t r = 0, g = 0, b = 0;
                uint32_t Y = 0;
                int32_t Cb = 0, Cr = 0;

                if (Size >= 40)
                    Y = rd32(Data + 36);
                if (Size >= 44)
                    Cb = (int32_t)rd32(Data + 40);
                if (Size >= 48)
                    Cr = (int32_t)rd32(Data + 44);

                TIFFYCbCrtoRGB(ycbcr, Y, Cb, Cr, &r, &g, &b);
                TIFFYCbCrtoRGB(ycbcr, 0, 0, 0, &r, &g, &b);
                TIFFYCbCrtoRGB(ycbcr, 255, 128, 128, &r, &g, &b);
                TIFFYCbCrtoRGB(ycbcr, 0xffffffffu, INT32_MIN, INT32_MAX, &r, &g, &b);

                if (Size >= 51) {
                    TIFFYCbCrtoRGB(ycbcr,
                                   Data[48],
                                   (int32_t)(int8_t)Data[49],
                                   (int32_t)(int8_t)Data[50],
                                   &r, &g, &b);
                }
            }
            free(mem);
        }
    }

    return 0;
}