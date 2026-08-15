#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiffio.h"

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float rd_float(const uint8_t *p) {
    float f;
    uint32_t u = rd32(p);
    memcpy(&f, &u, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFFDisplay display;
    TIFFCIELabToRGB cielab;
    TIFFYCbCrToRGB ycbcr;
    float refWhite[3];
    float xyz[3];
    float X, Y, Z;
    uint32_t R, G, B;
    uint32_t packed32, packed24;
    uint32_t L;
    int32_t a, b;
    uint32_t yv;
    int32_t cb, cr;
    int init_ok;
    size_t off = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(&display, 0, sizeof(display));
    memset(&cielab, 0, sizeof(cielab));
    memset(&ycbcr, 0, sizeof(ycbcr));
    memset(refWhite, 0, sizeof(refWhite));
    memset(xyz, 0, sizeof(xyz));

    if (Size >= 3 * sizeof(float)) {
        refWhite[0] = rd_float(Data + off); off += 4;
        refWhite[1] = rd_float(Data + off); off += 4;
        refWhite[2] = rd_float(Data + off); off += 4;
    } else {
        refWhite[0] = 95.047f;
        refWhite[1] = 100.0f;
        refWhite[2] = 108.883f;
    }

    if (Size >= off + sizeof(TIFFDisplay)) {
        memcpy(&display, Data + off, sizeof(TIFFDisplay));
        off += sizeof(TIFFDisplay);
    }

    init_ok = TIFFCIELabToRGBInit(&cielab, &display, refWhite);

    if (Size >= off + 4) {
        packed32 = rd32(Data + off);
        off += 4;
    } else {
        packed32 = 0;
    }
    LogLuv32toXYZ(packed32, xyz);

    if (Size >= off + 4) {
        packed24 = rd32(Data + off) & 0x00FFFFFFU;
        off += 4;
    } else {
        packed24 = 0;
    }
    LogLuv24toXYZ(packed24, xyz);

    if (Size >= off + 8) {
        L = rd32(Data + off);
        off += 4;
        a = (int16_t)rd16(Data + off);
        off += 2;
        b = (int16_t)rd16(Data + off);
        off += 2;
    } else {
        L = 0;
        a = 0;
        b = 0;
    }

    TIFFCIELabToXYZ(&cielab, L, a, b, &X, &Y, &Z);

    if (init_ok) {
        TIFFXYZToRGB(&cielab, X, Y, Z, &R, &G, &B);
        TIFFXYZToRGB(&cielab, xyz[0], xyz[1], xyz[2], &R, &G, &B);
    } else {
        TIFFXYZToRGB(&cielab, 0.0f, 0.0f, 0.0f, &R, &G, &B);
    }

    if (Size >= off + 8) {
        if (sizeof(TIFFYCbCrToRGB) <= Size - off) {
            memcpy(&ycbcr, Data + off, sizeof(TIFFYCbCrToRGB));
            off += sizeof(TIFFYCbCrToRGB);
        }
    }

    if (Size >= off + 8) {
        yv = rd32(Data + off);
        off += 4;
        cb = (int32_t)rd16(Data + off);
        off += 2;
        cr = (int32_t)rd16(Data + off);
        off += 2;
    } else {
        yv = 0;
        cb = 0;
        cr = 0;
    }

    TIFFYCbCrtoRGB(&ycbcr, yv, cb, cr, &R, &G, &B);

    if (Size > 0) {
        uint32_t altY = Data[0];
        int32_t altCb = (Size > 1) ? (int32_t)((int)Data[1] - 128) : 0;
        int32_t altCr = (Size > 2) ? (int32_t)((int)Data[2] - 128) : 0;
        TIFFYCbCrtoRGB(&ycbcr, altY, altCb, altCr, &R, &G, &B);
    }

    if (Size >= 12) {
        float fx = rd_float(Data);
        float fy = rd_float(Data + 4);
        float fz = rd_float(Data + 8);
        TIFFXYZToRGB(&cielab, fx, fy, fz, &R, &G, &B);
    }

    return 0;
}