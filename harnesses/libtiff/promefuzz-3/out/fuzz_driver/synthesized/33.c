// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfDouble at tif_swab.c:182:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
// TIFFSwabDouble at tif_swab.c:168:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_33(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (f) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, f);
        }
        fclose(f);
    }

    if (sizeof(float) != 4 || sizeof(double) != 8) {
        return 0;
    }

    if (Size == 0) {
        float fv = 0.0f;
        double dv = 0.0;
        uint32_t lv = 0;
        uint64_t l8v = 0;

        TIFFSwabFloat(&fv);
        TIFFSwabDouble(&dv);
        TIFFSwabArrayOfFloat(&fv, 1);
        TIFFSwabArrayOfDouble(&dv, 1);
        TIFFSwabArrayOfLong(&lv, 1);
        TIFFSwabArrayOfLong8(&l8v, 1);

        TIFFSwabArrayOfFloat(&fv, 0);
        TIFFSwabArrayOfDouble(&dv, 0);
        TIFFSwabArrayOfLong(&lv, 0);
        TIFFSwabArrayOfLong8(&l8v, 0);
        return 0;
    }

    {
        float single_f = 0.0f;
        double single_d = 0.0;
        if (Size >= 4) {
            uint32_t tmp = read_u32(Data);
            memcpy(&single_f, &tmp, sizeof(single_f));
        } else {
            uint8_t buf[4] = {0, 0, 0, 0};
            memcpy(buf, Data, Size);
            uint32_t tmp = read_u32(buf);
            memcpy(&single_f, &tmp, sizeof(single_f));
        }

        if (Size >= 8) {
            uint64_t tmp = read_u64(Data);
            memcpy(&single_d, &tmp, sizeof(single_d));
        } else {
            uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            memcpy(buf, Data, Size);
            uint64_t tmp = read_u64(buf);
            memcpy(&single_d, &tmp, sizeof(single_d));
        }

        TIFFSwabFloat(&single_f);
        TIFFSwabFloat(&single_f);

        TIFFSwabDouble(&single_d);
        TIFFSwabDouble(&single_d);
    }

    {
        size_t count32 = Size / 4;
        if (count32 == 0) {
            count32 = 1;
        }
        if (count32 > 1024) {
            count32 = 1024;
        }

        uint32_t *arr32 = (uint32_t *)malloc(count32 * sizeof(uint32_t));
        float *arrf = (float *)malloc(count32 * sizeof(float));
        if (arr32 && arrf) {
            size_t i;
            for (i = 0; i < count32; ++i) {
                uint8_t buf[4] = {0, 0, 0, 0};
                size_t off = i * 4;
                size_t remain = (off < Size) ? (Size - off) : 0;
                size_t take = remain >= 4 ? 4 : remain;
                if (take > 0) {
                    memcpy(buf, Data + off, take);
                }
                arr32[i] = read_u32(buf);
                memcpy(&arrf[i], &arr32[i], sizeof(float));
            }

            TIFFSwabArrayOfLong(arr32, (tmsize_t)count32);
            TIFFSwabArrayOfLong(arr32, (tmsize_t)count32);

            TIFFSwabArrayOfFloat(arrf, (tmsize_t)count32);
            TIFFSwabArrayOfFloat(arrf, (tmsize_t)count32);

            if (count32 > 1) {
                TIFFSwabArrayOfLong(arr32 + 1, (tmsize_t)(count32 - 1));
                TIFFSwabArrayOfFloat(arrf + 1, (tmsize_t)(count32 - 1));
            }

            TIFFSwabArrayOfLong(arr32, 0);
            TIFFSwabArrayOfFloat(arrf, 0);
        }
        free(arr32);
        free(arrf);
    }

    {
        size_t count64 = Size / 8;
        if (count64 == 0) {
            count64 = 1;
        }
        if (count64 > 1024) {
            count64 = 1024;
        }

        uint64_t *arr64 = (uint64_t *)malloc(count64 * sizeof(uint64_t));
        double *arrd = (double *)malloc(count64 * sizeof(double));
        if (arr64 && arrd) {
            size_t i;
            for (i = 0; i < count64; ++i) {
                uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                size_t off = i * 8;
                size_t remain = (off < Size) ? (Size - off) : 0;
                size_t take = remain >= 8 ? 8 : remain;
                if (take > 0) {
                    memcpy(buf, Data + off, take);
                }
                arr64[i] = read_u64(buf);
                memcpy(&arrd[i], &arr64[i], sizeof(double));
            }

            TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);
            TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);

            TIFFSwabArrayOfDouble(arrd, (tmsize_t)count64);
            TIFFSwabArrayOfDouble(arrd, (tmsize_t)count64);

            if (count64 > 1) {
                TIFFSwabArrayOfLong8(arr64 + 1, (tmsize_t)(count64 - 1));
                TIFFSwabArrayOfDouble(arrd + 1, (tmsize_t)(count64 - 1));
            }

            TIFFSwabArrayOfLong8(arr64, 0);
            TIFFSwabArrayOfDouble(arrd, 0);
        }
        free(arr64);
        free(arrd);
    }

    return 0;
}