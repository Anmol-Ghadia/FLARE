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

static int read_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(int)) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static float read_float(const uint8_t **data, size_t *size)
{
    float value = 0.0f;
    size_t n = (*size < sizeof(float)) ? *size : sizeof(float);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    cJSON *array = cJSON_CreateArray();
    if (array == NULL) {
        return 0;
    }

    int float_count = 0;
    if (remaining > 0) {
        float_count = (int)(ptr[0] % 16);
        ptr++;
        remaining--;
    }

    float *numbers = NULL;
    if (float_count > 0) {
        numbers = (float *)malloc((size_t)float_count * sizeof(float));
        if (numbers != NULL) {
            for (int i = 0; i < float_count; i++) {
                numbers[i] = read_float(&ptr, &remaining);
            }
        } else {
            float_count = 0;
        }
    }

    cJSON *float_array = cJSON_CreateFloatArray(numbers, float_count);
    free(numbers);
    numbers = NULL;

    if (float_array != NULL) {
        (void)cJSON_AddItemToArray(array, float_array);
    }

    int iterations = 0;
    if (remaining > 0) {
        iterations = (int)(ptr[0] % 32);
        ptr++;
        remaining--;
    }

    for (int i = 0; i < iterations; i++) {
        int op = 0;
        if (remaining > 0) {
            op = ptr[0] % 5;
            ptr++;
            remaining--;
        }

        switch (op) {
            case 0: {
                cJSON *nested = cJSON_CreateArray();
                if (nested != NULL) {
                    (void)cJSON_AddItemToArray(array, nested);
                }
                break;
            }
            case 1: {
                int index = read_int(&ptr, &remaining);
                (void)cJSON_GetArrayItem(array, index);
                break;
            }
            case 2: {
                int index = read_int(&ptr, &remaining);
                cJSON_DeleteItemFromArray(array, index);
                break;
            }
            case 3: {
                int index = read_int(&ptr, &remaining);
                cJSON *item = cJSON_GetArrayItem(array, index);
                if (item != NULL) {
                    (void)cJSON_AddItemReferenceToArray(array, item);
                } else {
                    (void)cJSON_AddItemReferenceToArray(array, NULL);
                }
                break;
            }
            case 4: {
                cJSON *new_array = cJSON_CreateArray();
                if (new_array != NULL) {
                    if (!cJSON_AddItemToArray(array, new_array)) {
                        cJSON_Delete(new_array);
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    if (remaining >= sizeof(int)) {
        int final_index = read_int(&ptr, &remaining);
        (void)cJSON_GetArrayItem(array, final_index);
        cJSON_DeleteItemFromArray(array, final_index);
    }

    cJSON_Delete(array);
    return 0;
}