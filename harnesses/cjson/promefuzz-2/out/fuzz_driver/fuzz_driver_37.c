// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateFloatArray at cJSON.c:2619:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateFloatArray at cJSON.c:2619:23 in cJSON.h
// cJSON_CreateFloatArray at cJSON.c:2619:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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

static uint16_t read_u16(const uint8_t *data, size_t size, size_t *offset) {
    uint16_t v = 0;
    if (*offset < size) {
        v = data[(*offset)++];
    }
    if (*offset < size) {
        v |= (uint16_t)data[(*offset)++] << 8;
    }
    return v;
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        if (*offset < size) {
            v |= (uint32_t)data[(*offset)++] << (8 * i);
        }
    }
    return v;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) {
        if (*offset < size) {
            v |= (uint64_t)data[(*offset)++] << (8 * i);
        }
    }
    return v;
}

static double read_double(const uint8_t *data, size_t size, size_t *offset) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = read_u64(data, size, offset);
    return conv.d;
}

static float read_float(const uint8_t *data, size_t size, size_t *offset) {
    union {
        uint32_t u;
        float f;
    } conv;
    conv.u = read_u32(data, size, offset);
    return conv.f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;

    double initial_num = read_double(Data, Size, &offset);
    cJSON *num = cJSON_CreateNumber(initial_num);
    if (num != NULL) {
        (void)cJSON_IsNumber(num);
        (void)cJSON_GetNumberValue(num);

        for (int i = 0; i < 4; i++) {
            double new_num = read_double(Data, Size, &offset);
            (void)cJSON_SetNumberHelper(num, new_num);
            (void)cJSON_IsNumber(num);
            (void)cJSON_GetNumberValue(num);

            char *printed_num = cJSON_Print(num);
            if (printed_num != NULL) {
                cJSON_free(printed_num);
            }
        }

        cJSON_Delete(num);
    }

    int count = 0;
    if (Size > offset) {
        count = (int)(read_u16(Data, Size, &offset) % 64);
    }

    float *arr = NULL;
    if (count > 0) {
        arr = (float *)malloc((size_t)count * sizeof(float));
        if (arr != NULL) {
            for (int i = 0; i < count; i++) {
                arr[i] = read_float(Data, Size, &offset);
            }
        } else {
            count = 0;
        }
    }

    cJSON *float_array = cJSON_CreateFloatArray(arr, count);
    if (float_array != NULL) {
        char *printed_array = cJSON_Print(float_array);
        if (printed_array != NULL) {
            cJSON_free(printed_array);
        }
        cJSON_Delete(float_array);
    }

    free(arr);

    (void)cJSON_CreateFloatArray(NULL, count);
    (void)cJSON_CreateFloatArray(NULL, -1);

    return 0;
}