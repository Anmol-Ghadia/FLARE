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

#include "cJSON.h"

static int read_int32_le(const uint8_t *p)
{
    return (int)(
        ((unsigned int)p[0]) |
        ((unsigned int)p[1] << 8) |
        ((unsigned int)p[2] << 16) |
        ((unsigned int)p[3] << 24));
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *arr_from_ints = NULL;
    cJSON *empty_array = NULL;
    cJSON *array_ref = NULL;
    cJSON *nested_array = NULL;
    cJSON *standalone_item = NULL;
    int *numbers = NULL;
    int count = 0;
    size_t max_ints = 0;
    size_t i = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    (void)cJSON_IsArray(NULL);
    (void)cJSON_GetArraySize(NULL);
    (void)cJSON_CreateIntArray(NULL, 0);
    (void)cJSON_CreateIntArray(NULL, -1);
    (void)cJSON_CreateArrayReference(NULL);

    empty_array = cJSON_CreateArray();
    if (empty_array != NULL) {
        (void)cJSON_IsArray(empty_array);
        (void)cJSON_GetArraySize(empty_array);
    }

    max_ints = Size / 4;
    if (max_ints > 0) {
        numbers = (int *)malloc(max_ints * sizeof(int));
        if (numbers != NULL) {
            for (i = 0; i < max_ints; i++) {
                numbers[i] = read_int32_le(Data + (i * 4));
            }

            count = (int)max_ints;
            arr_from_ints = cJSON_CreateIntArray(numbers, count);
            if (arr_from_ints != NULL) {
                (void)cJSON_IsArray(arr_from_ints);
                (void)cJSON_GetArraySize(arr_from_ints);

                array_ref = cJSON_CreateArrayReference(arr_from_ints->child);
                if (array_ref != NULL) {
                    (void)cJSON_IsArray(array_ref);
                    (void)cJSON_GetArraySize(array_ref);
                }

                if (empty_array != NULL) {
                    (void)cJSON_AddItemReferenceToArray(empty_array, arr_from_ints);
                    (void)cJSON_GetArraySize(empty_array);
                }
            }

            if (count > 0) {
                cJSON *partial_arr = cJSON_CreateIntArray(numbers, count / 2);
                if (partial_arr != NULL) {
                    (void)cJSON_IsArray(partial_arr);
                    (void)cJSON_GetArraySize(partial_arr);
                    cJSON_Delete(partial_arr);
                }
            }
        }
    }

    nested_array = cJSON_CreateArray();
    if (nested_array != NULL) {
        (void)cJSON_IsArray(nested_array);
        (void)cJSON_GetArraySize(nested_array);

        if (empty_array != NULL) {
            (void)cJSON_AddItemReferenceToArray(nested_array, empty_array);
            (void)cJSON_GetArraySize(nested_array);
        }

        if (arr_from_ints != NULL) {
            (void)cJSON_AddItemReferenceToArray(nested_array, arr_from_ints);
            (void)cJSON_GetArraySize(nested_array);
        }
    }

    standalone_item = cJSON_CreateArray();
    if (standalone_item != NULL) {
        (void)cJSON_AddItemReferenceToArray(NULL, standalone_item);
        if (empty_array != NULL) {
            (void)cJSON_AddItemReferenceToArray(empty_array, standalone_item);
            (void)cJSON_GetArraySize(empty_array);
        }
    }

    cJSON_Delete(standalone_item);
    cJSON_Delete(nested_array);
    cJSON_Delete(array_ref);
    cJSON_Delete(empty_array);
    cJSON_Delete(arr_from_ints);
    free(numbers);

    return 0;
}