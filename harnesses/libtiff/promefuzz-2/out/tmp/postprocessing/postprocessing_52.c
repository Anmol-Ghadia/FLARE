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
    return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static uint64_t read_u64(const uint8_t *p) {
    return ((uint64_t)p[0] << 56) |
           ((uint64_t)p[1] << 48) |
           ((uint64_t)p[2] << 40) |
           ((uint64_t)p[3] << 32) |
           ((uint64_t)p[4] << 24) |
           ((uint64_t)p[5] << 16) |
           ((uint64_t)p[6] << 8)  |
           ((uint64_t)p[7]);
}

static double read_double(const uint8_t *p) {
    uint64_t u = read_u64(p);
    double d = 0.0;
    memcpy(&d, &u, sizeof(d));
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size == 0) {
        uint16_t s = 0;
        uint64_t l8 = 0;
        double d = 0.0;
        TIFFSwabShort(&s);
        TIFFSwabLong8(&l8);
        TIFFSwabDouble(&d);
        TIFFSwabArrayOfShort(&s, 0);
        TIFFSwabArrayOfLong8(&l8, 0);
        TIFFSwabArrayOfDouble(&d, 0);
        return 0;
    }

    size_t offset = 0;

    if (Size - offset >= 2) {
        uint16_t s = read_u16(Data + offset);
        TIFFSwabShort(&s);
        TIFFSwabShort(&s);
        offset += 2;
    }

    if (Size - offset >= 8) {
        uint64_t l8 = read_u64(Data + offset);
        TIFFSwabLong8(&l8);
        TIFFSwabLong8(&l8);
        offset += 8;
    }

    if (Size - offset >= 8) {
        double d = read_double(Data + offset);
        TIFFSwabDouble(&d);
        TIFFSwabDouble(&d);
        offset += 8;
    }

    {
        size_t remain = Size - offset;
        size_t count16 = remain / 2;
        if (count16 > 0) {
            uint16_t *arr16 = (uint16_t *)malloc(count16 * sizeof(uint16_t));
            if (arr16 != NULL) {
                for (size_t i = 0; i < count16; ++i) {
                    arr16[i] = read_u16(Data + offset + i * 2);
                }

                TIFFSwabArrayOfShort(arr16, (tmsize_t)count16);

                if (count16 > 1) {
                    TIFFSwabArrayOfShort(arr16, (tmsize_t)(count16 / 2));
                    TIFFSwabArrayOfShort(arr16 + (count16 / 2),
                                         (tmsize_t)(count16 - count16 / 2));
                }

                TIFFSwabArrayOfShort(arr16, 0);
                free(arr16);
            }
        }
        offset += count16 * 2;
    }

    {
        size_t remain = Size - offset;
        size_t count64 = remain / 8;
        if (count64 > 0) {
            uint64_t *arr64 = (uint64_t *)malloc(count64 * sizeof(uint64_t));
            if (arr64 != NULL) {
                for (size_t i = 0; i < count64; ++i) {
                    arr64[i] = read_u64(Data + offset + i * 8);
                }

                TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);

                if (count64 > 1) {
                    TIFFSwabArrayOfLong8(arr64, (tmsize_t)(count64 / 2));
                    TIFFSwabArrayOfLong8(arr64 + (count64 / 2),
                                         (tmsize_t)(count64 - count64 / 2));
                }

                TIFFSwabArrayOfLong8(arr64, 0);
                free(arr64);
            }
        }
        offset += count64 * 8;
    }

    {
        size_t remain = Size - offset;
        size_t countd = remain / 8;
        if (countd > 0) {
            double *arrd = (double *)malloc(countd * sizeof(double));
            if (arrd != NULL) {
                for (size_t i = 0; i < countd; ++i) {
                    arrd[i] = read_double(Data + offset + i * 8);
                }

                TIFFSwabArrayOfDouble(arrd, (tmsize_t)countd);

                if (countd > 1) {
                    TIFFSwabArrayOfDouble(arrd, (tmsize_t)(countd / 2));
                    TIFFSwabArrayOfDouble(arrd + (countd / 2),
                                          (tmsize_t)(countd - countd / 2));
                }

                TIFFSwabArrayOfDouble(arrd, 0);
                free(arrd);
            }
        }
    }

    return 0;
}