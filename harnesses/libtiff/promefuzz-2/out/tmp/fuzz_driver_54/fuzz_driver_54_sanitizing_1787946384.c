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

#ifndef TIFFroundup_32
#define TIFFroundup_32(x, y) ((((x) + ((y)-1)) / (y)) * (y))
#endif

static uint8_t get_u8(const uint8_t *Data, size_t Size, size_t *Off) {
    if (*Off >= Size) return 0;
    return Data[(*Off)++];
}

static uint32_t get_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | get_u8(Data, Size, Off);
    }
    return v;
}

static int32_t get_s32(const uint8_t *Data, size_t Size, size_t *Off) {
    return (int32_t)get_u32(Data, Size, Off);
}

static float get_floatish(const uint8_t *Data, size_t Size, size_t *Off) {
    union {
        uint32_t u;
        float f;
    } v;
    v.u = get_u32(Data, Size, Off);
    return v.f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFFDisplay display;
    memset(&display, 0, sizeof(display));

    float refWhite[3];
    refWhite[0] = get_floatish(Data, Size, &off);
    refWhite[1] = get_floatish(Data, Size, &off);
    refWhite[2] = get_floatish(Data, Size, &off);

    TIFFCIELabToRGB cielab;
    memset(&cielab, 0, sizeof(cielab));

    (void)TIFFCIELabToRGBInit(&cielab, &display, refWhite);

    for (int i = 0; i < 4; i++) {
        uint32_t l = get_u32(Data, Size, &off);
        int32_t a = get_s32(Data, Size, &off);
        int32_t b = get_s32(Data, Size, &off);

        float X = 0.0f, Y = 0.0f, Z = 0.0f;
        TIFFCIELabToXYZ(&cielab, l, a, b, &X, &Y, &Z);

        float xyz[3];
        xyz[0] = X;
        xyz[1] = Y;
        xyz[2] = Z;

        uint8_t rgb24[3] = {0, 0, 0};
        XYZtoRGB24(xyz, rgb24);

        uint32_t r = 0, g = 0, bl = 0;
        TIFFXYZToRGB(&cielab, X, Y, Z, &r, &g, &bl);

        (void)LogLuv24fromXYZ(xyz, (int)(get_u8(Data, Size, &off) & 1));

        xyz[0] = get_floatish(Data, Size, &off);
        xyz[1] = get_floatish(Data, Size, &off);
        xyz[2] = get_floatish(Data, Size, &off);

        XYZtoRGB24(xyz, rgb24);
        TIFFXYZToRGB(&cielab, xyz[0], xyz[1], xyz[2], &r, &g, &bl);
        (void)LogLuv24fromXYZ(xyz, (int)(get_u8(Data, Size, &off) & 3));
    }

    {
        float luma[3];
        float refBlackWhite[6];
        for (int i = 0; i < 3; i++) luma[i] = get_floatish(Data, Size, &off);
        for (int i = 0; i < 6; i++) refBlackWhite[i] = get_floatish(Data, Size, &off);

        size_t ycbcr_alloc = TIFFroundup_32(sizeof(TIFFYCbCrToRGB), sizeof(long)) + 65536;
        void *buf = malloc(ycbcr_alloc);
        if (buf) {
            memset(buf, 0, ycbcr_alloc);
            (void)TIFFYCbCrToRGBInit((TIFFYCbCrToRGB *)buf, luma, refBlackWhite);
            free(buf);
        }
    }

    return 0;
}