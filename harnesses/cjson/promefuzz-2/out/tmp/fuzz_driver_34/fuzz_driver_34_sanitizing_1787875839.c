#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

static int read_int32(const uint8_t *data, size_t size, size_t *offset)
{
    int value = 0;
    size_t remaining = (size > *offset) ? (size - *offset) : 0;
    size_t to_copy = remaining < sizeof(value) ? remaining : sizeof(value);

    if (to_copy > 0) {
        memcpy(&value, data + *offset, to_copy);
        *offset += to_copy;
    }

    return value;
}

static size_t choose_count(const uint8_t *data, size_t size, size_t *offset, size_t max_count)
{
    if (max_count == 0) {
        return 0;
    }

    if (*offset >= size) {
        return 0;
    }

    return (size_t)(data[(*offset)++] % (max_count + 1));
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    size_t offset = 0;
    size_t max_ints, max_doubles, max_floats;
    size_t int_count, double_count, float_count;
    int *ints = NULL;
    double *doubles = NULL;
    float *floats = NULL;
    cJSON *int_array = NULL;
    cJSON *double_array = NULL;
    cJSON *float_array = NULL;
    cJSON *empty_array = NULL;
    cJSON *null_result = NULL;
    cJSON *manual_array = NULL;

    (void)cJSON_IsArray(NULL);
    (void)cJSON_GetArraySize(NULL);
    null_result = cJSON_CreateIntArray(NULL, 0);
    if (null_result != NULL) {
        cJSON_Delete(null_result);
    }
    null_result = cJSON_CreateDoubleArray(NULL, 0);
    if (null_result != NULL) {
        cJSON_Delete(null_result);
    }
    null_result = cJSON_CreateFloatArray(NULL, 0);
    if (null_result != NULL) {
        cJSON_Delete(null_result);
    }

    max_ints = (Size - offset) / sizeof(int);
    int_count = choose_count(Data, Size, &offset, max_ints);
    if (int_count > 0) {
        ints = (int *)malloc(int_count * sizeof(int));
        if (ints == NULL) {
            return 0;
        }
        for (size_t i = 0; i < int_count; ++i) {
            ints[i] = read_int32(Data, Size, &offset);
        }
    }

    max_doubles = (Size - offset) / sizeof(double);
    double_count = choose_count(Data, Size, &offset, max_doubles);
    if (double_count > 0) {
        doubles = (double *)malloc(double_count * sizeof(double));
        if (doubles == NULL) {
            free(ints);
            return 0;
        }
        for (size_t i = 0; i < double_count; ++i) {
            uint64_t bits = 0;
            size_t remaining = (Size > offset) ? (Size - offset) : 0;
            size_t to_copy = remaining < sizeof(bits) ? remaining : sizeof(bits);
            if (to_copy > 0) {
                memcpy(&bits, Data + offset, to_copy);
                offset += to_copy;
            }
            memcpy(&doubles[i], &bits, sizeof(double));
        }
    }

    max_floats = (Size - offset) / sizeof(float);
    float_count = choose_count(Data, Size, &offset, max_floats);
    if (float_count > 0) {
        floats = (float *)malloc(float_count * sizeof(float));
        if (floats == NULL) {
            free(ints);
            free(doubles);
            return 0;
        }
        for (size_t i = 0; i < float_count; ++i) {
            uint32_t bits = 0;
            size_t remaining = (Size > offset) ? (Size - offset) : 0;
            size_t to_copy = remaining < sizeof(bits) ? remaining : sizeof(bits);
            if (to_copy > 0) {
                memcpy(&bits, Data + offset, to_copy);
                offset += to_copy;
            }
            memcpy(&floats[i], &bits, sizeof(float));
        }
    }

    int_array = cJSON_CreateIntArray(ints, (int)int_count);
    if (int_array != NULL) {
        (void)cJSON_IsArray(int_array);
        (void)cJSON_GetArraySize(int_array);
    }

    double_array = cJSON_CreateDoubleArray(doubles, (int)double_count);
    if (double_array != NULL) {
        (void)cJSON_IsArray(double_array);
        (void)cJSON_GetArraySize(double_array);
    }

    float_array = cJSON_CreateFloatArray(floats, (int)float_count);
    if (float_array != NULL) {
        (void)cJSON_IsArray(float_array);
        (void)cJSON_GetArraySize(float_array);
    }

    empty_array = cJSON_CreateArray();
    if (empty_array != NULL) {
        (void)cJSON_IsArray(empty_array);
        (void)cJSON_GetArraySize(empty_array);
    }

    manual_array = cJSON_CreateArray();
    if (manual_array != NULL) {
        cJSON *child1 = cJSON_CreateIntArray(ints, (int)int_count);
        cJSON *child2 = cJSON_CreateDoubleArray(doubles, (int)double_count);
        cJSON *child3 = cJSON_CreateFloatArray(floats, (int)float_count);

        if (child1 != NULL) {
            child1->prev = NULL;
            child1->next = child2;
        }
        if (child2 != NULL) {
            child2->prev = child1;
            child2->next = child3;
        }
        if (child3 != NULL) {
            child3->prev = child2;
            child3->next = NULL;
        }

        manual_array->child = child1 ? child1 : (child2 ? child2 : child3);

        (void)cJSON_IsArray(manual_array);
        (void)cJSON_GetArraySize(manual_array);

        cJSON_Delete(manual_array);
        manual_array = NULL;
        child1 = NULL;
        child2 = NULL;
        child3 = NULL;
    }

    cJSON_Delete(int_array);
    cJSON_Delete(double_array);
    cJSON_Delete(float_array);
    cJSON_Delete(empty_array);

    free(ints);
    free(doubles);
    free(floats);

    return 0;
}