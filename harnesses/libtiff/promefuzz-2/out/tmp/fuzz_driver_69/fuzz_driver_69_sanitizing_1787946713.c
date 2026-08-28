#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <tiffio.h>

#ifndef TIFFroundup_32
#define TIFFroundup_32(x, y) ((((x) + ((y)-1)) / (y)) * (y))
#endif

/* Minimal concrete layouts for opaque types used by the target APIs. */
struct TIFFYCbCrToRGB {
    TIFFRGBValue *clamptab;
    int *Cr_r_tab;
    int *Cb_b_tab;
    int32_t *Cr_g_tab;
    int32_t *Cb_g_tab;
    int32_t *Y_tab;
};

struct TIFFCIELabToRGB {
    int range;
    float rstep, gstep, bstep;
    float X0, Y0, Z0;
    TIFFDisplay display;
    int *Yr2r;
    int *Yg2g;
    int *Yb2b;
};

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++];
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++] << 8;
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++] << 16;
    if (*Off < Size) v |= (uint32_t)Data[(*Off)++] << 24;
    return v;
}

static int32_t read_s32(const uint8_t *Data, size_t Size, size_t *Off) {
    return (int32_t)read_u32(Data, Size, Off);
}

static float read_floatish(const uint8_t *Data, size_t Size, size_t *Off, float scale) {
    return (float)read_s32(Data, Size, Off) / scale;
}

static double read_doubleish(const uint8_t *Data, size_t Size, size_t *Off, double scale) {
    return (double)read_s32(Data, Size, Off) / scale;
}

static struct TIFFCIELabToRGB *alloc_cielab_safe(void) {
    struct TIFFCIELabToRGB *cielab =
        (struct TIFFCIELabToRGB *)_TIFFmalloc(sizeof(struct TIFFCIELabToRGB));
    if (!cielab)
        return NULL;
    memset(cielab, 0, sizeof(*cielab));

    /*
     * TIFFXYZToRGB indexes Yr2r/Yg2g/Yb2b with:
     *   i = floor(Y / step)
     * and then clamps only against "range", so allocate one extra element
     * because i == range can occur for boundary values.
     */
    cielab->range = 1500;
    cielab->rstep = 1.0f;
    cielab->gstep = 1.0f;
    cielab->bstep = 1.0f;
    cielab->X0 = 0.0f;
    cielab->Y0 = 0.0f;
    cielab->Z0 = 0.0f;

    memset(&cielab->display, 0, sizeof(cielab->display));
    cielab->display.d_mat[0][0] = 1.0f;
    cielab->display.d_mat[1][1] = 1.0f;
    cielab->display.d_mat[2][2] = 1.0f;
    cielab->display.d_YCR = 100.0f;
    cielab->display.d_YCG = 100.0f;
    cielab->display.d_YCB = 100.0f;
    cielab->display.d_Vrwr = 1.0f;
    cielab->display.d_Vrwg = 1.0f;
    cielab->display.d_Vrwb = 1.0f;
    cielab->display.d_Y0R = 0.0f;
    cielab->display.d_Y0G = 0.0f;
    cielab->display.d_Y0B = 0.0f;
    cielab->display.d_gammaR = 1.0f;
    cielab->display.d_gammaG = 1.0f;
    cielab->display.d_gammaB = 1.0f;

    cielab->Yr2r = (int *)_TIFFmalloc((tmsize_t)(cielab->range + 1) * sizeof(int));
    cielab->Yg2g = (int *)_TIFFmalloc((tmsize_t)(cielab->range + 1) * sizeof(int));
    cielab->Yb2b = (int *)_TIFFmalloc((tmsize_t)(cielab->range + 1) * sizeof(int));
    if (!cielab->Yr2r || !cielab->Yg2g || !cielab->Yb2b) {
        if (cielab->Yr2r) _TIFFfree(cielab->Yr2r);
        if (cielab->Yg2g) _TIFFfree(cielab->Yg2g);
        if (cielab->Yb2b) _TIFFfree(cielab->Yb2b);
        _TIFFfree(cielab);
        return NULL;
    }

    for (int i = 0; i <= cielab->range; i++) {
        cielab->Yr2r[i] = i;
        cielab->Yg2g[i] = i;
        cielab->Yb2b[i] = i;
    }

    return cielab;
}

static void free_cielab(struct TIFFCIELabToRGB *cielab) {
    if (!cielab)
        return;
    if (cielab->Yr2r) _TIFFfree(cielab->Yr2r);
    if (cielab->Yg2g) _TIFFfree(cielab->Yg2g);
    if (cielab->Yb2b) _TIFFfree(cielab->Yb2b);
    _TIFFfree(cielab);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t Off = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        uint32_t packed = read_u32(Data, Size, &Off);
        float xyz[3] = {0.0f, 0.0f, 0.0f};
        LogLuv32toXYZ(packed, xyz);

        struct TIFFCIELabToRGB *cielab = alloc_cielab_safe();
        if (cielab) {
            uint32_t r = 0, g = 0, b = 0;

            /*
             * Keep XYZ in a conservative range so TIFFXYZToRGB's internal
             * indexing remains within our deliberately minimal tables.
             */
            float x = xyz[0], y = xyz[1], z = xyz[2];
            if (!isfinite(x) || x < 0.0f) x = 0.0f;
            if (!isfinite(y) || y < 0.0f) y = 0.0f;
            if (!isfinite(z) || z < 0.0f) z = 0.0f;
            if (x > 1499.0f) x = 1499.0f;
            if (y > 1499.0f) y = 1499.0f;
            if (z > 1499.0f) z = 1499.0f;
            TIFFXYZToRGB(cielab, x, y, z, &r, &g, &b);

            x = fabsf(read_floatish(Data, Size, &Off, 1024.0f));
            y = fabsf(read_floatish(Data, Size, &Off, 1024.0f));
            z = fabsf(read_floatish(Data, Size, &Off, 1024.0f));
            if (!isfinite(x)) x = 0.0f;
            if (!isfinite(y)) y = 0.0f;
            if (!isfinite(z)) z = 0.0f;
            if (x > 1499.0f) x = 1499.0f;
            if (y > 1499.0f) y = 1499.0f;
            if (z > 1499.0f) z = 1499.0f;
            TIFFXYZToRGB(cielab, x, y, z, &r, &g, &b);

            free_cielab(cielab);
        }
    }

    {
        double u = read_doubleish(Data, Size, &Off, 4096.0);
        double v = read_doubleish(Data, Size, &Off, 4096.0);
        int em = (int)(read_u32(Data, Size, &Off) % 8) - 4;
        int code = uv_encode(u, v, em);

        double du = 0.0, dv = 0.0;
        (void)uv_decode(&du, &dv, code);
        (void)uv_decode(&du, &dv, (int)read_s32(Data, Size, &Off));
    }

    {
        float luma[3];
        float refbw[6];
        for (int i = 0; i < 3; i++)
            luma[i] = fabsf(read_floatish(Data, Size, &Off, 1024.0f));
        if (luma[0] == 0.0f && luma[1] == 0.0f && luma[2] == 0.0f) {
            luma[0] = 0.299f;
            luma[1] = 0.587f;
            luma[2] = 0.114f;
        }
        for (int i = 0; i < 6; i++)
            refbw[i] = read_floatish(Data, Size, &Off, 16.0f);

        size_t alloc_sz = TIFFroundup_32(sizeof(struct TIFFYCbCrToRGB), sizeof(long)) +
                          1024 * sizeof(TIFFRGBValue) +
                          256 * sizeof(int) * 2 +
                          256 * sizeof(int32_t) * 3;
        void *mem = _TIFFmalloc((tmsize_t)alloc_sz);
        if (mem) {
            memset(mem, 0, alloc_sz);
            TIFFYCbCrToRGB *ycbcr = (TIFFYCbCrToRGB *)mem;
            if (TIFFYCbCrToRGBInit(ycbcr, luma, refbw)) {
                uint32_t r = 0, g = 0, b = 0;
                uint32_t Y = read_u32(Data, Size, &Off) & 0xFFu;
                int32_t Cb = (int32_t)(read_u32(Data, Size, &Off) & 0xFFu);
                int32_t Cr = (int32_t)(read_u32(Data, Size, &Off) & 0xFFu);
                TIFFYCbCrtoRGB(ycbcr, Y, Cb, Cr, &r, &g, &b);

                TIFFYCbCrtoRGB(ycbcr,
                               read_u32(Data, Size, &Off),
                               read_s32(Data, Size, &Off),
                               read_s32(Data, Size, &Off),
                               &r, &g, &b);
            }
            _TIFFfree(mem);
        }
    }

    return 0;
}