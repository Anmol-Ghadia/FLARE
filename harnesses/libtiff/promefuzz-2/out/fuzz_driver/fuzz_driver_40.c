// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL16fromY at tif_luv.c:758:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10fromY at tif_luv.c:836:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL10toY at tif_luv.c:825:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogL16toY at tif_luv.c:743:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// LogLuv32fromXYZ at tif_luv.c:1127:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
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

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t read_u64(const uint8_t *p) {
    return ((uint64_t)p[0]) |
           ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

static double bytes_to_double(const uint8_t *p) {
    union {
        uint64_t u;
        double d;
    } v;
    v.u = read_u64(p);
    return v.d;
}

static float bytes_to_float(const uint8_t *p) {
    union {
        uint32_t u;
        float f;
    } v;
    v.u = read_u32(p);
    return v.f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    double d1 = 0.0, d2 = 0.0, d3 = 0.0;
    float xyz[3] = {0.0f, 0.0f, 0.0f};
    int em1 = 0, em2 = 1, em3 = -1;
    int p10 = 0, p16 = 0;

    if (Size >= 8) {
        d1 = bytes_to_double(Data);
    }
    if (Size >= 16) {
        d2 = bytes_to_double(Data + 8);
    }
    if (Size >= 24) {
        d3 = bytes_to_double(Data + 16);
    }
    if (Size >= 28) {
        xyz[0] = bytes_to_float(Data + 24);
    }
    if (Size >= 32) {
        xyz[1] = bytes_to_float(Data + 28);
    }
    if (Size >= 36) {
        xyz[2] = bytes_to_float(Data + 32);
    }
    if (Size >= 37) {
        em1 = (int)((int8_t)Data[36]);
    }
    if (Size >= 38) {
        em2 = (int)((int8_t)Data[37]);
    }
    if (Size >= 39) {
        em3 = (int)((int8_t)Data[38]);
    }
    if (Size >= 41) {
        p10 = (int)(read_u16(Data + 39) & 0x3ff);
    } else if (Size >= 2) {
        p10 = (int)(read_u16(Data) & 0x3ff);
    }
    if (Size >= 43) {
        p16 = (int)read_u16(Data + 41);
    } else if (Size >= 4) {
        p16 = (int)read_u16(Data + 2);
    }

    (void)LogL16fromY(d1, em1);
    (void)LogL16fromY(d2, em2);
    (void)LogL16fromY(d3, em3);
    (void)LogL16fromY(0.0, em1);
    (void)LogL16fromY(-0.0, em2);
    (void)LogL16fromY(1.0, em3);
    (void)LogL16fromY(-1.0, em1);

    (void)LogL10fromY(d1, em1);
    (void)LogL10fromY(d2, em2);
    (void)LogL10fromY(d3, em3);
    (void)LogL10fromY(0.0, em1);
    (void)LogL10fromY(0.00024283, em2);
    (void)LogL10fromY(15.742, em3);
    (void)LogL10fromY(-1.0, em1);
    (void)LogL10fromY(1.0, em2);

    (void)LogL10toY(p10);
    (void)LogL10toY(0);
    (void)LogL10toY(1);
    (void)LogL10toY(0x3ff);
    (void)LogL10toY((int)(d1));

    (void)LogL16toY(p16);
    (void)LogL16toY(0);
    (void)LogL16toY(1);
    (void)LogL16toY(0x7fff);
    (void)LogL16toY(0x8001);
    (void)LogL16toY(0xffff);

    (void)LogLuv32fromXYZ(xyz, em1);

    {
        float xyz2[3] = { xyz[1], xyz[2], xyz[0] };
        (void)LogLuv32fromXYZ(xyz2, em2);
    }
    {
        float xyz3[3] = { 0.0f, 0.0f, 0.0f };
        (void)LogLuv32fromXYZ(xyz3, em3);
    }
    {
        float xyz4[3] = { -xyz[0], -xyz[1], -xyz[2] };
        (void)LogLuv32fromXYZ(xyz4, em1);
    }

    TIFFSwabDouble(&d1);
    TIFFSwabDouble(&d1);
    TIFFSwabDouble(&d2);
    TIFFSwabDouble(&d3);

    return 0;
}