// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateDoubleArray at cJSON.c:2659:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_CreateDoubleArray at cJSON.c:2659:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_CreateDoubleArray at cJSON.c:2659:23 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cJSON.h"

static double bytes_to_double(const uint8_t *data, size_t size, size_t offset)
{
    uint64_t bits = 0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        uint8_t b = 0;
        if (offset + i < size) {
            b = data[offset + i];
        }
        bits |= ((uint64_t)b) << (8 * i);
    }

    double out;
    memcpy(&out, &bits, sizeof(out));
    return out;
}

int LLVMFuzzerTestOneInput_33(const uint8_t *Data, size_t Size)
{
    size_t i, count, idx_off;
    cJSON *num = NULL;
    cJSON *arr = NULL;
    double *numbers = NULL;

    if (Data == NULL) {
        return 0;
    }

    if (Size >= 8) {
        double d = bytes_to_double(Data, Size, 0);
        num = cJSON_CreateNumber(d);
        if (num != NULL) {
            (void)cJSON_IsNumber(num);
            (void)cJSON_GetNumberValue(num);

            if (Size >= 16) {
                double d2 = bytes_to_double(Data, Size, 8);
                (void)cJSON_SetNumberHelper(num, d2);
                (void)cJSON_IsNumber(num);
                (void)cJSON_GetNumberValue(num);
            }
        }
    }

    count = Size / 8;
    if (count > 64) {
        count = 64;
    }

    if (count > 0) {
        numbers = (double *)malloc(count * sizeof(double));
        if (numbers != NULL) {
            for (i = 0; i < count; ++i) {
                numbers[i] = bytes_to_double(Data, Size, i * 8);
            }

            arr = cJSON_CreateDoubleArray(numbers, (int)count);
            if (arr != NULL) {
                idx_off = count * 8;

                for (i = 0; i < count + 4 && idx_off + i < Size; ++i) {
                    int index = (int)Data[idx_off + i];
                    cJSON *item = cJSON_GetArrayItem(arr, index);
                    if (item != NULL) {
                        if (cJSON_IsNumber(item)) {
                            (void)cJSON_GetNumberValue(item);
                            if (idx_off + count + (i * 8) + 8 <= Size) {
                                double newv = bytes_to_double(Data, Size, idx_off + count + (i * 8));
                                (void)cJSON_SetNumberHelper(item, newv);
                                (void)cJSON_GetNumberValue(item);
                            }
                        } else {
                            (void)cJSON_GetNumberValue(item);
                        }
                    }
                }

                (void)cJSON_GetArrayItem(arr, -1);
                (void)cJSON_GetArrayItem(arr, (int)count);
                (void)cJSON_GetArrayItem(arr, (int)count + 1000);
            }
        }
    }

    (void)cJSON_CreateDoubleArray(NULL, (int)count);
    if (numbers != NULL) {
        (void)cJSON_CreateDoubleArray(numbers, -1);
    }

    cJSON_Delete(arr);
    cJSON_Delete(num);
    free(numbers);

    return 0;
}