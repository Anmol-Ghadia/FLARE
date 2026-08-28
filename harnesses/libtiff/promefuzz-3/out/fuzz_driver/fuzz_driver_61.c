// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToXYZ at tif_color.c:44:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// TIFFXYZToRGB at tif_color.c:76:1 in tiffio.h
// TIFFYCbCrToRGBInit at tif_color.c:234:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float rd_float(const uint8_t *p) {
    uint32_t u = rd32(p);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static float sanitize_float(float f, float fallback) {
    volatile float z = f;
    if (z != z) return fallback;
    if (z > 1.0e20f) return 1.0e20f;
    if (z < -1.0e20f) return -1.0e20f;
    return z;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    if (Size < 64) {
        return 0;
    }

    size_t off = 0;

    float swabf = rd_float(Data + off);
    off += 4;
    TIFFSwabFloat(&swabf);
    TIFFSwabFloat(&swabf);

    TIFFDisplay display;
    memset(&display, 0, sizeof(display));

    float display_vals[9];
    for (int i = 0; i < 9; i++) {
        if (off + 4 <= Size) {
            display_vals[i] = sanitize_float(rd_float(Data + off), 0.0f);
            off += 4;
        } else {
            display_vals[i] = 0.0f;
        }
    }

    display.d_mat[0][0] = display_vals[0];
    display.d_mat[0][1] = display_vals[1];
    display.d_mat[0][2] = display_vals[2];
    display.d_mat[1][0] = display_vals[3];
    display.d_mat[1][1] = display_vals[4];
    display.d_mat[1][2] = display_vals[5];
    display.d_mat[2][0] = display_vals[6];
    display.d_mat[2][1] = display_vals[7];
    display.d_mat[2][2] = display_vals[8];

    if (off + 4 <= Size) {
        display.d_YCR = sanitize_float(rd_float(Data + off), 1.0f);
        off += 4;
    } else {
        display.d_YCR = 1.0f;
    }
    if (off + 4 <= Size) {
        display.d_YCG = sanitize_float(rd_float(Data + off), 1.0f);
        off += 4;
    } else {
        display.d_YCG = 1.0f;
    }
    if (off + 4 <= Size) {
        display.d_YCB = sanitize_float(rd_float(Data + off), 1.0f);
        off += 4;
    } else {
        display.d_YCB = 1.0f;
    }
    if (off + 4 <= Size) {
        display.d_Vrwr = sanitize_float(rd_float(Data + off), 1.0f);
        off += 4;
    } else {
        display.d_Vrwr = 1.0f;
    }
    if (off + 4 <= Size) {
        display.d_Vrwg = sanitize_float(rd_float(Data + off), 1.0f);
        off += 4;
    } else {
        display.d_Vrwg = 1.0f;
    }
    if (off + 4 <= Size) {
        display.d_Vrwb = sanitize_float(rd_float(Data + off), 1.0f);
        off += 4;
    } else {
        display.d_Vrwb = 1.0f;
    }
    if (off + 4 <= Size) {
        display.d_Y0R = sanitize_float(rd_float(Data + off), 100.0f);
        off += 4;
    } else {
        display.d_Y0R = 100.0f;
    }
    if (off + 4 <= Size) {
        display.d_Y0G = sanitize_float(rd_float(Data + off), 100.0f);
        off += 4;
    } else {
        display.d_Y0G = 100.0f;
    }
    if (off + 4 <= Size) {
        display.d_Y0B = sanitize_float(rd_float(Data + off), 100.0f);
        off += 4;
    } else {
        display.d_Y0B = 100.0f;
    }
    if (off + 4 <= Size) {
        display.d_gammaR = sanitize_float(rd_float(Data + off), 2.2f);
        off += 4;
    } else {
        display.d_gammaR = 2.2f;
    }
    if (off + 4 <= Size) {
        display.d_gammaG = sanitize_float(rd_float(Data + off), 2.2f);
        off += 4;
    } else {
        display.d_gammaG = 2.2f;
    }
    if (off + 4 <= Size) {
        display.d_gammaB = sanitize_float(rd_float(Data + off), 2.2f);
        off += 4;
    } else {
        display.d_gammaB = 2.2f;
    }

    float refWhite[3] = {95.047f, 100.0f, 108.883f};
    for (int i = 0; i < 3; i++) {
        if (off + 4 <= Size) {
            refWhite[i] = sanitize_float(rd_float(Data + off), refWhite[i]);
            off += 4;
        }
    }

    TIFFCIELabToRGB cielab;
    memset(&cielab, 0, sizeof(cielab));

    int cielab_ok = TIFFCIELabToRGBInit(&cielab, &display, refWhite);
    if (cielab_ok) {
        uint32_t l = (off + 4 <= Size) ? rd32(Data + off) : 0;
        if (off + 4 <= Size) off += 4;
        int32_t a = (off + 4 <= Size) ? (int32_t)rd32(Data + off) : 0;
        if (off + 4 <= Size) off += 4;
        int32_t b = (off + 4 <= Size) ? (int32_t)rd32(Data + off) : 0;
        if (off + 4 <= Size) off += 4;

        float X = 0.0f, Y = 0.0f, Z = 0.0f;
        TIFFCIELabToXYZ(&cielab, l, a, b, &X, &Y, &Z);

        uint32_t r = 0, g = 0, bl = 0;
        TIFFXYZToRGB(&cielab, X, Y, Z, &r, &g, &bl);

        TIFFXYZToRGB(&cielab,
                     sanitize_float(X + swabf, 0.0f),
                     sanitize_float(Y - swabf, 0.0f),
                     sanitize_float(Z * 0.5f, 0.0f),
                     &r, &g, &bl);
    }

    uint32_t logluv = (off + 4 <= Size) ? rd32(Data + off) : 0;
    float xyz[3] = {0.0f, 0.0f, 0.0f};
    LogLuv32toXYZ(logluv, xyz);

    if (cielab_ok) {
        uint32_t r2 = 0, g2 = 0, b2 = 0;
        TIFFXYZToRGB(&cielab, xyz[0], xyz[1], xyz[2], &r2, &g2, &b2);
    }

    {
        float luma[3] = {0.299f, 0.587f, 0.114f};
        float refBlackWhiteYCbCr[6] = {0.0f, 255.0f, 128.0f, 255.0f, 128.0f, 255.0f};

        for (int i = 0; i < 3; i++) {
            if (off + 4 <= Size) {
                luma[i] = sanitize_float(rd_float(Data + off), luma[i]);
                off += 4;
            }
        }
        for (int i = 0; i < 6; i++) {
            if (off + 4 <= Size) {
                refBlackWhiteYCbCr[i] = sanitize_float(rd_float(Data + off), refBlackWhiteYCbCr[i]);
                off += 4;
            }
        }

        {
            enum { YCBCR_BUF_SIZE = 16384 };
            unsigned char *buf = (unsigned char *)calloc(1, YCBCR_BUF_SIZE);
            if (buf) {
                TIFFYCbCrToRGB *ycbcr = (TIFFYCbCrToRGB *)buf;
                (void)TIFFYCbCrToRGBInit(ycbcr, luma, refBlackWhiteYCbCr);
                free(buf);
            }
        }
    }

    return 0;
}