#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

static int read_int(const uint8_t *data, size_t size, size_t *offset)
{
    int value = 0;
    if (*offset + sizeof(int) <= size) {
        memcpy(&value, data + *offset, sizeof(int));
        *offset += sizeof(int);
    } else if (*offset < size) {
        size_t remain = size - *offset;
        memcpy(&value, data + *offset, remain);
        *offset = size;
    }
    return value;
}

static size_t clamp_count(int v, size_t max_count)
{
    if (v < 0) {
        return 0;
    }
    return ((size_t)v > max_count) ? max_count : (size_t)v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    size_t offset = 0;
    size_t i;

    int raw_count1 = read_int(Data, Size, &offset);
    int raw_count2 = read_int(Data, Size, &offset);
    int idx1 = read_int(Data, Size, &offset);
    int idx2 = read_int(Data, Size, &offset);
    int del1 = read_int(Data, Size, &offset);
    int del2 = read_int(Data, Size, &offset);

    size_t remaining = (offset < Size) ? (Size - offset) : 0;
    size_t max_ints = remaining / sizeof(int);
    size_t int_count = clamp_count(raw_count1, max_ints);

    cJSON *root_array = NULL;
    cJSON *int_array = NULL;
    cJSON *double_array = NULL;
    cJSON *empty_array = NULL;

    root_array = cJSON_CreateArray();
    if (root_array == NULL) {
        return 0;
    }

    if (int_count > 0) {
        int *numbers = (int *)malloc(int_count * sizeof(int));
        if (numbers != NULL) {
            for (i = 0; i < int_count; i++) {
                int v = 0;
                if (offset + sizeof(int) <= Size) {
                    memcpy(&v, Data + offset, sizeof(int));
                    offset += sizeof(int);
                } else if (offset < Size) {
                    size_t remain2 = Size - offset;
                    memcpy(&v, Data + offset, remain2);
                    offset = Size;
                }
                numbers[i] = v;
            }
            int_array = cJSON_CreateIntArray(numbers, (int)int_count);
            free(numbers);
        }
    } else {
        static const int dummy_int = 0;
        int_array = cJSON_CreateIntArray(&dummy_int, 0);
    }

    remaining = (offset < Size) ? (Size - offset) : 0;
    size_t max_doubles = remaining / sizeof(double);
    size_t double_count = clamp_count(raw_count2, max_doubles);

    if (double_count > 0) {
        double *numbers = (double *)malloc(double_count * sizeof(double));
        if (numbers != NULL) {
            for (i = 0; i < double_count; i++) {
                double v = 0.0;
                if (offset + sizeof(double) <= Size) {
                    memcpy(&v, Data + offset, sizeof(double));
                    offset += sizeof(double);
                } else if (offset < Size) {
                    size_t remain2 = Size - offset;
                    memcpy(&v, Data + offset, remain2);
                    offset = Size;
                }
                numbers[i] = v;
            }
            double_array = cJSON_CreateDoubleArray(numbers, (int)double_count);
            free(numbers);
        }
    } else {
        static const double dummy_double = 0.0;
        double_array = cJSON_CreateDoubleArray(&dummy_double, 0);
    }

    empty_array = cJSON_CreateArray();

    (void)cJSON_GetArraySize(NULL);
    (void)cJSON_GetArrayItem(NULL, idx1);
    cJSON_DeleteItemFromArray(NULL, del1);

    if (int_array != NULL) {
        (void)cJSON_GetArraySize(int_array);
        (void)cJSON_GetArrayItem(int_array, idx1);
        (void)cJSON_GetArrayItem(int_array, -1);
        cJSON_DeleteItemFromArray(int_array, del1);
        (void)cJSON_GetArraySize(int_array);
    }

    if (double_array != NULL) {
        (void)cJSON_GetArraySize(double_array);
        (void)cJSON_GetArrayItem(double_array, idx2);
        (void)cJSON_GetArrayItem(double_array, -1);
        cJSON_DeleteItemFromArray(double_array, del2);
        (void)cJSON_GetArraySize(double_array);
    }

    if (empty_array != NULL) {
        (void)cJSON_GetArraySize(empty_array);
        (void)cJSON_GetArrayItem(empty_array, 0);
        cJSON_DeleteItemFromArray(empty_array, 0);
        (void)cJSON_GetArraySize(empty_array);
    }

    if (int_array != NULL) {
        cJSON_AddItemToArray(root_array, int_array);
        int_array = NULL;
    }
    if (double_array != NULL) {
        cJSON_AddItemToArray(root_array, double_array);
        double_array = NULL;
    }
    if (empty_array != NULL) {
        cJSON_AddItemToArray(root_array, empty_array);
        empty_array = NULL;
    }

    (void)cJSON_GetArraySize(root_array);
    (void)cJSON_GetArrayItem(root_array, idx1);
    (void)cJSON_GetArrayItem(root_array, idx2);
    cJSON_DeleteItemFromArray(root_array, del1);
    cJSON_DeleteItemFromArray(root_array, del2);
    (void)cJSON_GetArraySize(root_array);

    cJSON_Delete(root_array);
    return 0;
}