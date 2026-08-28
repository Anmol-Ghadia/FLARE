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

static int consume_int(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = (*size < sizeof(v)) ? *size : sizeof(v);
    if (n > 0) {
        memcpy(&v, *data, n);
        *data += n;
        *size -= n;
    }
    return v;
}

static double consume_double(const uint8_t **data, size_t *size) {
    double v = 0.0;
    size_t n = (*size < sizeof(v)) ? *size : sizeof(v);
    if (n > 0) {
        memcpy(&v, *data, n);
        *data += n;
        *size -= n;
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    int requested_count = consume_int(&ptr, &remaining);
    if (requested_count < 0) {
        requested_count = -requested_count;
    }
    requested_count %= 64;

    double *numbers = NULL;
    if (requested_count > 0) {
        numbers = (double *)malloc((size_t)requested_count * sizeof(double));
        if (numbers == NULL) {
            return 0;
        }
        for (int i = 0; i < requested_count; ++i) {
            if (remaining >= sizeof(double)) {
                numbers[i] = consume_double(&ptr, &remaining);
            } else if (remaining > 0) {
                double d = 0.0;
                memcpy(&d, ptr, remaining);
                numbers[i] = d;
                ptr += remaining;
                remaining = 0;
            } else {
                numbers[i] = (double)i;
            }
        }
    }

    cJSON *double_array = cJSON_CreateDoubleArray(numbers, requested_count);
    cJSON *empty_array = cJSON_CreateArray();

    if (double_array != NULL) {
        int size1 = cJSON_GetArraySize(double_array);

        cJSON *ref_array = cJSON_CreateArrayReference(double_array->child);
        if (ref_array != NULL) {
            int size2 = cJSON_GetArraySize(ref_array);

            cJSON_GetArrayItem(ref_array, -1);
            cJSON_GetArrayItem(ref_array, 0);
            cJSON_GetArrayItem(ref_array, size2);
            if (size2 > 0) {
                cJSON_GetArrayItem(ref_array, size2 - 1);
            }

            if (empty_array != NULL) {
                cJSON_AddItemReferenceToArray(empty_array, ref_array);
                cJSON_AddItemReferenceToArray(empty_array, double_array);
                cJSON_GetArraySize(empty_array);
                cJSON_GetArrayItem(empty_array, 0);
                cJSON_GetArrayItem(empty_array, 1);
                cJSON_GetArrayItem(empty_array, size1);
            }

            cJSON_Delete(ref_array);
        }

        cJSON_GetArrayItem(double_array, -1);
        cJSON_GetArrayItem(double_array, 0);
        cJSON_GetArrayItem(double_array, size1);
        if (size1 > 0) {
            cJSON_GetArrayItem(double_array, size1 - 1);
        }
    }

    if (empty_array != NULL) {
        cJSON_GetArraySize(empty_array);
        cJSON_GetArrayItem(empty_array, -1);
        cJSON_GetArrayItem(empty_array, 0);

        cJSON *null_ref = cJSON_CreateArrayReference(NULL);
        if (null_ref != NULL) {
            cJSON_AddItemReferenceToArray(empty_array, null_ref);
            cJSON_Delete(null_ref);
        }
    }

    cJSON_GetArraySize(NULL);
    cJSON_GetArrayItem(NULL, 0);
    cJSON_AddItemReferenceToArray(NULL, double_array);
    cJSON_CreateDoubleArray(NULL, requested_count);

    if (empty_array != NULL) {
        cJSON_Delete(empty_array);
    }
    if (double_array != NULL) {
        cJSON_Delete(double_array);
    }
    free(numbers);
    return 0;
}