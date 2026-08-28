// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSwabArrayOfTriples at tif_swab.c:87:1 in tiffio.h
// TIFFSwabArrayOfTriples at tif_swab.c:87:1 in tiffio.h
// TIFFSwabArrayOfTriples at tif_swab.c:87:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabArrayOfFloat at tif_swab.c:151:1 in tiffio.h
// TIFFSwabShort at tif_swab.c:34:1 in tiffio.h
// TIFFSwabShort at tif_swab.c:34:1 in tiffio.h
// TIFFSwabLong at tif_swab.c:45:1 in tiffio.h
// TIFFSwabLong at tif_swab.c:45:1 in tiffio.h
// TIFFSwabLong8 at tif_swab.c:57:1 in tiffio.h
// TIFFSwabLong8 at tif_swab.c:57:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabArrayOfTriples at tif_swab.c:87:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_52(const uint8_t *Data, size_t Size) {
    FILE *fp;
    uint16_t v16 = 0;
    uint32_t v32 = 0;
    uint64_t v64 = 0;
    float fval = 0.0f;
    uint8_t *triple_buf = NULL;
    float *float_buf = NULL;
    size_t triple_count = 0;
    size_t float_count = 0;
    size_t i;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size >= 2) {
        v16 = read_u16(Data);
    } else if (Size == 1) {
        v16 = Data[0];
    }
    TIFFSwabShort(&v16);
    TIFFSwabShort(&v16);

    if (Size >= 4) {
        v32 = read_u32(Data);
    } else {
        for (i = 0; i < Size; ++i) {
            v32 |= ((uint32_t)Data[i]) << (8 * i);
        }
    }
    TIFFSwabLong(&v32);
    TIFFSwabLong(&v32);

    if (Size >= 8) {
        v64 = read_u64(Data);
    } else {
        for (i = 0; i < Size; ++i) {
            v64 |= ((uint64_t)Data[i]) << (8 * i);
        }
    }
    TIFFSwabLong8(&v64);
    TIFFSwabLong8(&v64);

    if (Size >= sizeof(float)) {
        memcpy(&fval, Data, sizeof(float));
    } else if (Size > 0) {
        uint8_t tmp[sizeof(float)] = {0};
        memcpy(tmp, Data, Size);
        memcpy(&fval, tmp, sizeof(float));
    }
    TIFFSwabFloat(&fval);
    TIFFSwabFloat(&fval);

    triple_count = Size / 3;
    if (triple_count > 0) {
        triple_buf = (uint8_t *)malloc(triple_count * 3);
        if (triple_buf != NULL) {
            memcpy(triple_buf, Data, triple_count * 3);
            TIFFSwabArrayOfTriples(triple_buf, (tmsize_t)triple_count);
            TIFFSwabArrayOfTriples(triple_buf, (tmsize_t)triple_count);

            if (triple_count > 1) {
                TIFFSwabArrayOfTriples(triple_buf + 3, (tmsize_t)(triple_count - 1));
            }
            free(triple_buf);
            triple_buf = NULL;
        }
    } else {
        uint8_t dummy[3] = {0, 0, 0};
        TIFFSwabArrayOfTriples(dummy, (tmsize_t)0);
    }

    float_count = Size / sizeof(float);
    if (float_count > 0) {
        float_buf = (float *)malloc(float_count * sizeof(float));
        if (float_buf != NULL) {
            memcpy(float_buf, Data, float_count * sizeof(float));
            TIFFSwabArrayOfFloat(float_buf, (tmsize_t)float_count);
            TIFFSwabArrayOfFloat(float_buf, (tmsize_t)float_count);

            if (float_count > 1) {
                TIFFSwabArrayOfFloat(float_buf + 1, (tmsize_t)(float_count - 1));
            }

            for (i = 0; i < float_count; ++i) {
                TIFFSwabFloat(&float_buf[i]);
            }

            free(float_buf);
            float_buf = NULL;
        }
    } else {
        float zero = 0.0f;
        TIFFSwabArrayOfFloat(&zero, (tmsize_t)0);
    }

    return 0;
}