// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFCIELabToRGBInit at tif_color.c:123:1 in tiffio.h
// TIFFYCbCrToRGBInit at tif_color.c:234:1 in tiffio.h
// TIFFYCbCrToRGBInit at tif_color.c:234:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// LogLuv32toXYZ at tif_luv.c:1102:1 in tiffio.h
// XYZtoRGB24 at tif_luv.c:807:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <tiffio.h>

extern void LogLuv32toXYZ(uint32_t, float *);
extern int TIFFCIELabToRGBInit(TIFFCIELabToRGB *, const TIFFDisplay *, float *);
extern void XYZtoRGB24(float *, uint8_t *);
extern int TIFFYCbCrToRGBInit(TIFFYCbCrToRGB*, float*, float*);

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
    float f = 0.0f;
    memcpy(&f, p, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL) {
        TIFFRGBAImage img;
        char emsg[1024];
        memset(&img, 0, sizeof(img));
        memset(emsg, 0, sizeof(emsg));
        (void)TIFFRGBAImageBegin(&img, tif, (Size > 0) ? (Data[0] & 1) : 0, emsg);
        TIFFClose(tif);
    }

    {
        uint32_t packed = 0;
        float xyz[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb[3] = {0, 0, 0};

        if (Size >= 4) {
            packed = rd32(Data);
        } else if (Size > 0) {
            size_t i;
            for (i = 0; i < Size; ++i)
                packed |= ((uint32_t)Data[i]) << (8 * i);
        }

        LogLuv32toXYZ(packed, xyz);
        XYZtoRGB24(xyz, rgb);

        if (Size >= 8) {
            uint32_t packed2 = rd32(Data + Size - 4);
            float xyz2[3] = {0.0f, 0.0f, 0.0f};
            uint8_t rgb2[3] = {0, 0, 0};
            LogLuv32toXYZ(packed ^ packed2, xyz2);
            XYZtoRGB24(xyz2, rgb2);
        }
    }

    {
        float f = 0.0f;
        if (Size >= sizeof(float)) {
            f = rd_float(Data);
        } else if (Size > 0) {
            uint8_t tmp[sizeof(float)] = {0};
            memcpy(tmp, Data, Size < sizeof(tmp) ? Size : sizeof(tmp));
            memcpy(&f, tmp, sizeof(f));
        }
        TIFFSwabFloat(&f);

        if (Size >= 2 * sizeof(float)) {
            float f2 = rd_float(Data + sizeof(float));
            TIFFSwabFloat(&f2);
        }
    }

    {
        TIFFDisplay display;
        TIFFCIELabToRGB cielab;
        float refWhite[3];
        memset(&display, 0, sizeof(display));
        memset(&cielab, 0, sizeof(cielab));

        refWhite[0] = (Size >= 4) ? rd_float(Data) : 95.047f;
        refWhite[1] = (Size >= 8) ? rd_float(Data + 4) : 100.0f;
        refWhite[2] = (Size >= 12) ? rd_float(Data + 8) : 108.883f;

        if (Size >= 24) {
            display.d_mat[0][0] = rd_float(Data + 12);
            display.d_mat[0][1] = rd_float(Data + 16);
            display.d_mat[0][2] = rd_float(Data + 20);
        }
        if (Size >= 36) {
            display.d_mat[1][0] = rd_float(Data + 24);
            display.d_mat[1][1] = rd_float(Data + 28);
            display.d_mat[1][2] = rd_float(Data + 32);
        }
        if (Size >= 48) {
            display.d_mat[2][0] = rd_float(Data + 36);
            display.d_mat[2][1] = rd_float(Data + 40);
            display.d_mat[2][2] = rd_float(Data + 44);
        }
        if (Size >= 60) {
            display.d_YCR = rd_float(Data + 48);
            display.d_YCG = rd_float(Data + 52);
            display.d_YCB = rd_float(Data + 56);
        }
        if (Size >= 72) {
            display.d_Vrwr = rd_float(Data + 60);
            display.d_Vrwg = rd_float(Data + 64);
            display.d_Vrwb = rd_float(Data + 68);
        }
        if (Size >= 84) {
            display.d_Y0R = rd_float(Data + 72);
            display.d_Y0G = rd_float(Data + 76);
            display.d_Y0B = rd_float(Data + 80);
        }
        if (Size >= 96) {
            display.d_gammaR = rd_float(Data + 84);
            display.d_gammaG = rd_float(Data + 88);
            display.d_gammaB = rd_float(Data + 92);
        } else {
            if (display.d_gammaR == 0.0f) display.d_gammaR = 2.2f;
            if (display.d_gammaG == 0.0f) display.d_gammaG = 2.2f;
            if (display.d_gammaB == 0.0f) display.d_gammaB = 2.2f;
        }

        (void)TIFFCIELabToRGBInit(&cielab, &display, refWhite);

        refWhite[0] = (refWhite[0] == 0.0f) ? 95.047f : refWhite[0];
        refWhite[1] = (refWhite[1] == 0.0f) ? 100.0f : refWhite[1];
        refWhite[2] = (refWhite[2] == 0.0f) ? 108.883f : refWhite[2];
        (void)TIFFCIELabToRGBInit(&cielab, &display, refWhite);
    }

    {
        float luma[3];
        float refBlackWhite[6];

        luma[0] = (Size >= 4) ? rd_float(Data) : 0.299f;
        luma[1] = (Size >= 8) ? rd_float(Data + 4) : 0.587f;
        luma[2] = (Size >= 12) ? rd_float(Data + 8) : 0.114f;

        refBlackWhite[0] = (Size >= 16) ? rd_float(Data + 12) : 0.0f;
        refBlackWhite[1] = (Size >= 20) ? rd_float(Data + 16) : 255.0f;
        refBlackWhite[2] = (Size >= 24) ? rd_float(Data + 20) : 128.0f;
        refBlackWhite[3] = (Size >= 28) ? rd_float(Data + 24) : 255.0f;
        refBlackWhite[4] = (Size >= 32) ? rd_float(Data + 28) : 128.0f;
        refBlackWhite[5] = (Size >= 36) ? rd_float(Data + 32) : 255.0f;

        if (luma[0] == 0.0f && luma[1] == 0.0f && luma[2] == 0.0f) {
            luma[0] = 0.299f;
            luma[1] = 0.587f;
            luma[2] = 0.114f;
        }

        /* TIFFYCbCrToRGB is an opaque/incomplete type in public headers and
         * the initializer writes internal tables into the provided object.
         * Allocate a generously sized heap buffer instead of a stack object
         * to avoid stack overflows from undersized public declarations. */
        {
            size_t ycbcr_buf_size = 1U << 20;
            void *ycbcr_mem = malloc(ycbcr_buf_size);
            if (ycbcr_mem != NULL) {
                memset(ycbcr_mem, 0, ycbcr_buf_size);
                (void)TIFFYCbCrToRGBInit((TIFFYCbCrToRGB *)ycbcr_mem, luma, refBlackWhite);

                if (Size >= 48) {
                    float luma2[3] = {
                        rd_float(Data + 36),
                        rd_float(Data + 40),
                        rd_float(Data + 44)
                    };
                    if (luma2[0] == 0.0f && luma2[1] == 0.0f && luma2[2] == 0.0f) {
                        luma2[0] = 0.299f;
                        luma2[1] = 0.587f;
                        luma2[2] = 0.114f;
                    }
                    memset(ycbcr_mem, 0, ycbcr_buf_size);
                    (void)TIFFYCbCrToRGBInit((TIFFYCbCrToRGB *)ycbcr_mem, luma2, refBlackWhite);
                }

                free(ycbcr_mem);
            }
        }
    }

    {
        float xyz[3] = {0.0f, 0.0f, 0.0f};
        uint8_t rgb[3] = {0, 0, 0};

        if (Size >= 12) {
            xyz[0] = rd_float(Data);
            xyz[1] = rd_float(Data + 4);
            xyz[2] = rd_float(Data + 8);
        } else if (Size >= 6) {
            xyz[0] = (float)rd16(Data) / 65535.0f;
            xyz[1] = (float)rd16(Data + 2) / 65535.0f;
            xyz[2] = (float)rd16(Data + 4) / 65535.0f;
        } else if (Size >= 3) {
            xyz[0] = (float)Data[0] / 255.0f;
            xyz[1] = (float)Data[1] / 255.0f;
            xyz[2] = (float)Data[2] / 255.0f;
        }

        XYZtoRGB24(xyz, rgb);

        xyz[0] = -xyz[0];
        xyz[1] = xyz[1] * 2.0f;
        xyz[2] = xyz[2] * 1000.0f;
        XYZtoRGB24(xyz, rgb);
    }

    return 0;
}