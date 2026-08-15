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
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
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

static double read_double(const uint8_t *p) {
    uint64_t u = read_u64(p);
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    uint32_t single32 = 0;
    uint64_t single64 = 0;
    double singled = 0.0;

    if (Size >= 4) {
        single32 = read_u32(Data);
    } else {
        for (size_t i = 0; i < Size; ++i) {
            single32 |= ((uint32_t)Data[i]) << ((i % 4) * 8);
        }
    }

    if (Size >= 8) {
        single64 = read_u64(Data);
        singled = read_double(Data);
    } else {
        for (size_t i = 0; i < Size; ++i) {
            single64 |= ((uint64_t)Data[i]) << ((i % 8) * 8);
        }
        memcpy(&singled, &single64, sizeof(singled));
    }

    TIFFSwabLong(&single32);
    TIFFSwabLong(&single32);

    TIFFSwabLong8(&single64);
    TIFFSwabLong8(&single64);

    TIFFSwabDouble(&singled);
    TIFFSwabDouble(&singled);

    size_t short_count = Size / 2;
    size_t long8_count = Size / 8;
    size_t double_count = Size / 8;

    if (short_count > 0) {
        uint16_t *shorts = (uint16_t *)malloc(short_count * sizeof(uint16_t));
        if (shorts) {
            for (size_t i = 0; i < short_count; ++i) {
                shorts[i] = read_u16(Data + i * 2);
            }

            TIFFSwabArrayOfShort(shorts, (tmsize_t)short_count);
            TIFFSwabArrayOfShort(shorts, (tmsize_t)(short_count / 2));
            TIFFSwabArrayOfShort(shorts, 0);

            free(shorts);
        }
    } else {
        TIFFSwabArrayOfShort(NULL, 0);
    }

    if (long8_count > 0) {
        uint64_t *longs = (uint64_t *)malloc(long8_count * sizeof(uint64_t));
        if (longs) {
            for (size_t i = 0; i < long8_count; ++i) {
                longs[i] = read_u64(Data + i * 8);
            }

            TIFFSwabArrayOfLong8(longs, (tmsize_t)long8_count);
            TIFFSwabArrayOfLong8(longs, (tmsize_t)(long8_count > 1 ? long8_count - 1 : 0));
            TIFFSwabArrayOfLong8(longs, 0);

            for (size_t i = 0; i < long8_count; ++i) {
                TIFFSwabLong8(&longs[i]);
            }

            free(longs);
        }
    } else {
        TIFFSwabArrayOfLong8(NULL, 0);
    }

    if (double_count > 0) {
        double *doubles = (double *)malloc(double_count * sizeof(double));
        if (doubles) {
            for (size_t i = 0; i < double_count; ++i) {
                doubles[i] = read_double(Data + i * 8);
            }

            TIFFSwabArrayOfDouble(doubles, (tmsize_t)double_count);
            TIFFSwabArrayOfDouble(doubles, (tmsize_t)(double_count / 2));
            TIFFSwabArrayOfDouble(doubles, 0);

            for (size_t i = 0; i < double_count; ++i) {
                TIFFSwabDouble(&doubles[i]);
            }

            free(doubles);
        }
    } else {
        TIFFSwabArrayOfDouble(NULL, 0);
    }

    return 0;
}