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
#include "tiffio.h"

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8);
}

static uint32_t read_u32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t read_u64(const uint8_t *p) {
    return (uint64_t)p[0] |
           ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    size_t offset = 0;
    size_t i;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size >= 8) {
        uint64_t v64 = read_u64(Data);
        TIFFSwabLong8(&v64);
        TIFFSwabLong8(&v64);
        offset = 8;
    }

    if (Size >= offset + 4) {
        union {
            uint32_t u32;
            float f;
        } fu;
        fu.u32 = read_u32(Data + offset);
        TIFFSwabFloat(&fu.f);
        TIFFSwabFloat(&fu.f);
        offset += 4;
    }

    {
        size_t remaining = (offset < Size) ? (Size - offset) : 0;
        size_t count16 = remaining / sizeof(uint16_t);
        if (count16 > 0) {
            uint16_t *arr16 = (uint16_t *)malloc(count16 * sizeof(uint16_t));
            if (arr16 != NULL) {
                for (i = 0; i < count16; i++) {
                    arr16[i] = read_u16(Data + offset + i * 2);
                }
                TIFFSwabArrayOfShort(arr16, (tmsize_t)count16);
                if (count16 > 1) {
                    TIFFSwabArrayOfShort(arr16, (tmsize_t)(count16 / 2));
                    TIFFSwabArrayOfShort(arr16 + (count16 / 2),
                                         (tmsize_t)(count16 - (count16 / 2)));
                }
                TIFFSwabArrayOfShort(arr16, (tmsize_t)count16);
                free(arr16);
            }
        } else {
            uint16_t dummy16 = 0;
            TIFFSwabArrayOfShort(&dummy16, (tmsize_t)0);
        }
    }

    {
        size_t count32 = Size / sizeof(uint32_t);
        if (count32 > 0) {
            uint32_t *arr32 = (uint32_t *)malloc(count32 * sizeof(uint32_t));
            if (arr32 != NULL) {
                for (i = 0; i < count32; i++) {
                    arr32[i] = read_u32(Data + i * 4);
                }
                TIFFSwabArrayOfLong(arr32, (tmsize_t)count32);
                if (count32 > 1) {
                    TIFFSwabArrayOfLong(arr32, (tmsize_t)1);
                    TIFFSwabArrayOfLong(arr32 + 1, (tmsize_t)(count32 - 1));
                }
                TIFFSwabArrayOfLong(arr32, (tmsize_t)count32);
                free(arr32);
            }
        } else {
            uint32_t dummy32 = 0;
            TIFFSwabArrayOfLong(&dummy32, (tmsize_t)0);
        }
    }

    {
        size_t count64 = Size / sizeof(uint64_t);
        if (count64 > 0) {
            uint64_t *arr64 = (uint64_t *)malloc(count64 * sizeof(uint64_t));
            if (arr64 != NULL) {
                for (i = 0; i < count64; i++) {
                    arr64[i] = read_u64(Data + i * 8);
                }
                TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);
                if (count64 > 1) {
                    TIFFSwabArrayOfLong8(arr64, (tmsize_t)(count64 / 2));
                    TIFFSwabArrayOfLong8(arr64 + (count64 / 2),
                                         (tmsize_t)(count64 - (count64 / 2)));
                }
                TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);
                free(arr64);
            }
        } else {
            uint64_t dummy64 = 0;
            TIFFSwabArrayOfLong8(&dummy64, (tmsize_t)0);
        }
    }

    {
        size_t countd = Size / sizeof(double);
        if (countd > 0) {
            double *arrd = (double *)malloc(countd * sizeof(double));
            if (arrd != NULL) {
                for (i = 0; i < countd; i++) {
                    union {
                        uint64_t u64;
                        double d;
                    } du;
                    du.u64 = read_u64(Data + i * 8);
                    arrd[i] = du.d;
                }
                TIFFSwabArrayOfDouble(arrd, (tmsize_t)countd);
                if (countd > 1) {
                    TIFFSwabArrayOfDouble(arrd, (tmsize_t)1);
                    TIFFSwabArrayOfDouble(arrd + 1, (tmsize_t)(countd - 1));
                }
                TIFFSwabArrayOfDouble(arrd, (tmsize_t)countd);
                free(arrd);
            }
        } else {
            double dummyd = 0.0;
            TIFFSwabArrayOfDouble(&dummyd, (tmsize_t)0);
        }
    }

    return 0;
}