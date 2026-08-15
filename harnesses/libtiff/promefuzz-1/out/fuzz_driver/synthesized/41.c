// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabLong8 at tif_swab.c:57:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabLong8 at tif_swab.c:57:1 in tiffio.h
// TIFFSwabLong8 at tif_swab.c:57:1 in tiffio.h
// TIFFSwabLong8 at tif_swab.c:57:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_41(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (f) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, f);
        }
        fclose(f);
    }

    if (Size == 0) {
        float fv = 0.0f;
        double dv = 0.0;
        uint64_t lv = 0;
        TIFFSwabFloat(&fv);
        TIFFSwabDouble(&dv);
        TIFFSwabLong8(&lv);
        TIFFSwabArrayOfFloat(&fv, 0);
        TIFFSwabArrayOfDouble(&dv, 0);
        TIFFSwabArrayOfLong8(&lv, 0);
        return 0;
    }

    size_t offset = 0;

    if (Size >= 4) {
        float fv;
        uint32_t bits = read_u32(Data);
        memcpy(&fv, &bits, sizeof(fv));
        TIFFSwabFloat(&fv);
        TIFFSwabFloat(&fv);
        offset = 4;
    } else {
        float fv = 0.0f;
        memcpy(&fv, Data, Size);
        TIFFSwabFloat(&fv);
    }

    if (Size >= 8) {
        double dv;
        uint64_t bits = read_u64(Data);
        memcpy(&dv, &bits, sizeof(dv));
        TIFFSwabDouble(&dv);
        TIFFSwabDouble(&dv);
    } else {
        double dv = 0.0;
        memcpy(&dv, Data, Size < sizeof(dv) ? Size : sizeof(dv));
        TIFFSwabDouble(&dv);
    }

    if (Size >= 8) {
        uint64_t lv = read_u64(Data + (Size >= 16 ? 8 : 0));
        TIFFSwabLong8(&lv);
        TIFFSwabLong8(&lv);
    } else {
        uint64_t lv = 0;
        memcpy(&lv, Data, Size);
        TIFFSwabLong8(&lv);
    }

    {
        size_t remain = (offset < Size) ? (Size - offset) : 0;
        size_t max_count = remain / sizeof(float);
        if (max_count > 1024) max_count = 1024;

        float *arr = NULL;
        if (max_count > 0) {
            arr = (float *)malloc(max_count * sizeof(float));
            if (arr) {
                for (size_t i = 0; i < max_count; ++i) {
                    uint32_t bits = read_u32(Data + offset + i * 4);
                    memcpy(&arr[i], &bits, sizeof(float));
                }
                TIFFSwabArrayOfFloat(arr, (tmsize_t)max_count);
                if (max_count > 1) {
                    TIFFSwabArrayOfFloat(arr, (tmsize_t)(max_count / 2));
                }
                TIFFSwabArrayOfFloat(arr, 0);
                free(arr);
            }
        } else {
            float tmp = 0.0f;
            TIFFSwabArrayOfFloat(&tmp, 0);
        }
    }

    {
        size_t remain = Size;
        size_t max_count = remain / sizeof(double);
        if (max_count > 512) max_count = 512;

        double *arr = NULL;
        if (max_count > 0) {
            arr = (double *)malloc(max_count * sizeof(double));
            if (arr) {
                for (size_t i = 0; i < max_count; ++i) {
                    uint64_t bits = read_u64(Data + i * 8);
                    memcpy(&arr[i], &bits, sizeof(double));
                }
                TIFFSwabArrayOfDouble(arr, (tmsize_t)max_count);
                if (max_count > 1) {
                    TIFFSwabArrayOfDouble(arr, (tmsize_t)(max_count - 1));
                }
                TIFFSwabArrayOfDouble(arr, 0);
                free(arr);
            }
        } else {
            double tmp = 0.0;
            TIFFSwabArrayOfDouble(&tmp, 0);
        }
    }

    {
        size_t remain = Size;
        size_t max_count = remain / sizeof(uint64_t);
        if (max_count > 1024) max_count = 1024;

        uint64_t *arr = NULL;
        if (max_count > 0) {
            arr = (uint64_t *)malloc(max_count * sizeof(uint64_t));
            if (arr) {
                for (size_t i = 0; i < max_count; ++i) {
                    arr[i] = read_u64(Data + i * 8);
                }
                TIFFSwabArrayOfLong8(arr, (tmsize_t)max_count);
                if (max_count > 2) {
                    TIFFSwabArrayOfLong8(arr + 1, (tmsize_t)(max_count - 1));
                }
                TIFFSwabArrayOfLong8(arr, 0);
                free(arr);
            }
        } else {
            uint64_t tmp = 0;
            TIFFSwabArrayOfLong8(&tmp, 0);
        }
    }

    return 0;
}