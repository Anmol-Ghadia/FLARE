#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"
#include "tiffiop.h"

static uint32_t rd32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int32_t rd32s(const uint8_t *p) {
    return (int32_t)rd32(p);
}

static float rd_float(const uint8_t *p) {
    float f = 0.0f;
    memcpy(&f, p, sizeof(float));
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

    float swabf = 0.0f;
    if (Size >= 4) {
        swabf = rd_float(Data);
    } else if (Size > 0) {
        uint8_t tmp[4] = {0, 0, 0, 0};
        memcpy(tmp, Data, Size);
        swabf = rd_float(tmp);
    }
    TIFFSwabFloat(&swabf);

    float refWhite[3] = {95.047f, 100.0f, 108.883f};
    float luma[3] = {0.299f, 0.587f, 0.114f};
    float refBlackWhite[6] = {0.0f, 255.0f, 128.0f, 255.0f, 128.0f, 255.0f};

    if (Size >= 12) {
        refWhite[0] = rd_float(Data + 0);
        refWhite[1] = rd_float(Data + 4);
        refWhite[2] = rd_float(Data + 8);
    } else {
        for (size_t i = 0; i < Size; i++) {
            ((uint8_t *)refWhite)[i % sizeof(refWhite)] ^= Data[i];
        }
    }

    if (Size >= 24) {
        luma[0] = rd_float(Data + 12);
        luma[1] = rd_float(Data + 16);
        luma[2] = rd_float(Data + 20);
    } else {
        for (size_t i = 0; i < Size; i++) {
            ((uint8_t *)luma)[i % sizeof(luma)] ^= (uint8_t)(Data[i] + (uint8_t)i);
        }
    }

    if (Size >= 48) {
        refBlackWhite[0] = rd_float(Data + 24);
        refBlackWhite[1] = rd_float(Data + 28);
        refBlackWhite[2] = rd_float(Data + 32);
        refBlackWhite[3] = rd_float(Data + 36);
        refBlackWhite[4] = rd_float(Data + 40);
        refBlackWhite[5] = rd_float(Data + 44);
    } else {
        for (size_t i = 0; i < Size; i++) {
            ((uint8_t *)refBlackWhite)[i % sizeof(refBlackWhite)] ^= (uint8_t)(Data[i] ^ 0x5A);
        }
    }

    TIFFYCbCrToRGB ycbcr;
    memset(&ycbcr, 0, sizeof(ycbcr));

    TIFFCIELabToRGB cielab;
    memset(&cielab, 0, sizeof(cielab));

    TIFFDisplay display;
    memset(&display, 0, sizeof(display));

    (void)TIFFYCbCrToRGBInit(&ycbcr, luma, refBlackWhite);
    (void)TIFFCIELabToRGBInit(&cielab, &display, refWhite);

    uint32_t l = 0;
    int32_t a = 0;
    int32_t b = 0;
    if (Size >= 60) {
        l = rd32(Data + 48);
        a = rd32s(Data + 52);
        b = rd32s(Data + 56);
    } else if (Size >= 4) {
        l = rd32(Data);
        a = (int32_t)(int8_t)Data[Size / 2];
        b = (int32_t)(int8_t)Data[Size - 1];
    }

    float X = 0.0f, Y = 0.0f, Z = 0.0f;
    TIFFCIELabToXYZ(&cielab, l, a, b, &X, &Y, &Z);

    uint32_t R = 0, G = 0, B = 0;
    TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);

    float xyz_out[3] = {0.0f, 0.0f, 0.0f};
    uint32_t packed = 0;
    if (Size >= 64) {
        packed = rd32(Data + 60);
    } else if (Size >= 4) {
        packed = rd32(Data);
    }
    LogLuv32toXYZ(packed, xyz_out);

    TIFFXYZToRGB(&cielab, xyz_out[0], xyz_out[1], xyz_out[2], &R, &G, &B);

    TIFFSwabFloat(&X);
    TIFFSwabFloat(&Y);
    TIFFSwabFloat(&Z);

    TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);

    if (Size > 0) {
        uint32_t l2 = Data[0];
        int32_t a2 = (Size > 1) ? (int32_t)(int8_t)Data[1] : 0;
        int32_t b2 = (Size > 2) ? (int32_t)(int8_t)Data[2] : 0;
        TIFFCIELabToXYZ(&cielab, l2, a2, b2, &X, &Y, &Z);
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);
    }

    return 0;
}