// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateIntArray at cJSON.c:2621:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_CreateDoubleArray at cJSON.c:2701:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2571:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2571:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2571:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2571:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
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

static int consume_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(value)) ? *size : sizeof(value);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static double consume_double(const uint8_t **data, size_t *size)
{
    double value = 0.0;
    size_t n = (*size < sizeof(value)) ? *size : sizeof(value);
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

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    int int_count_raw = consume_int(&ptr, &remaining);
    int double_count_raw = consume_int(&ptr, &remaining);
    int index1 = consume_int(&ptr, &remaining);
    int index2 = consume_int(&ptr, &remaining);

    int int_count = int_count_raw;
    int double_count = double_count_raw;

    if (int_count < 0) {
        int_count = -int_count;
    }
    if (double_count < 0) {
        double_count = -double_count;
    }

    int_count %= 64;
    double_count %= 64;

    int *ints = NULL;
    double *doubles = NULL;

    if (int_count > 0) {
        ints = (int *)malloc((size_t)int_count * sizeof(int));
        if (ints == NULL) {
            return 0;
        }
        for (int i = 0; i < int_count; ++i) {
            ints[i] = consume_int(&ptr, &remaining);
        }
    }

    if (double_count > 0) {
        doubles = (double *)malloc((size_t)double_count * sizeof(double));
        if (doubles == NULL) {
            free(ints);
            return 0;
        }
        for (int i = 0; i < double_count; ++i) {
            doubles[i] = consume_double(&ptr, &remaining);
        }
    }

    cJSON *empty_array = cJSON_CreateArray();
    if (empty_array != NULL) {
        (void)cJSON_IsArray(empty_array);
        (void)cJSON_GetArrayItem(empty_array, index1);
        (void)cJSON_GetArrayItem(empty_array, -1);
    }

    cJSON *int_array = cJSON_CreateIntArray(ints, int_count);
    if (int_array != NULL) {
        if (cJSON_IsArray(int_array)) {
            (void)cJSON_GetArrayItem(int_array, index1);
            (void)cJSON_GetArrayItem(int_array, index2);
            (void)cJSON_GetArrayItem(int_array, 0);
            (void)cJSON_GetArrayItem(int_array, int_count);
            (void)cJSON_GetArrayItem(int_array, -1);
        }
    } else {
        (void)cJSON_IsArray(int_array);
        (void)cJSON_GetArrayItem(int_array, index1);
    }

    cJSON *double_array = cJSON_CreateDoubleArray(doubles, double_count);
    if (double_array != NULL) {
        if (cJSON_IsArray(double_array)) {
            (void)cJSON_GetArrayItem(double_array, index1);
            (void)cJSON_GetArrayItem(double_array, index2);
            (void)cJSON_GetArrayItem(double_array, 0);
            (void)cJSON_GetArrayItem(double_array, double_count);
            (void)cJSON_GetArrayItem(double_array, -1);
        }
    } else {
        (void)cJSON_IsArray(double_array);
        (void)cJSON_GetArrayItem(double_array, index2);
    }

    cJSON *ref_from_empty = NULL;
    if (empty_array != NULL) {
        ref_from_empty = cJSON_CreateArrayReference(empty_array->child);
        if (ref_from_empty != NULL) {
            (void)cJSON_IsArray(ref_from_empty);
            (void)cJSON_GetArrayItem(ref_from_empty, 0);
            (void)cJSON_GetArrayItem(ref_from_empty, index1);
            cJSON_Delete(ref_from_empty);
        }
    }

    cJSON *ref_from_int = NULL;
    if (int_array != NULL) {
        ref_from_int = cJSON_CreateArrayReference(int_array->child);
        if (ref_from_int != NULL) {
            (void)cJSON_IsArray(ref_from_int);
            (void)cJSON_GetArrayItem(ref_from_int, 0);
            (void)cJSON_GetArrayItem(ref_from_int, index1);
            (void)cJSON_GetArrayItem(ref_from_int, index2);
            cJSON_Delete(ref_from_int);
        }
    }

    cJSON *ref_from_double = NULL;
    if (double_array != NULL) {
        ref_from_double = cJSON_CreateArrayReference(double_array->child);
        if (ref_from_double != NULL) {
            (void)cJSON_IsArray(ref_from_double);
            (void)cJSON_GetArrayItem(ref_from_double, 0);
            (void)cJSON_GetArrayItem(ref_from_double, index1);
            (void)cJSON_GetArrayItem(ref_from_double, index2);
            cJSON_Delete(ref_from_double);
        }
    }

    cJSON *null_ref = cJSON_CreateArrayReference(NULL);
    if (null_ref != NULL) {
        (void)cJSON_IsArray(null_ref);
        (void)cJSON_GetArrayItem(null_ref, 0);
        cJSON_Delete(null_ref);
    }

    (void)cJSON_IsArray(NULL);
    (void)cJSON_GetArrayItem(NULL, 0);
    (void)cJSON_GetArrayItem(NULL, -1);

    cJSON_Delete(double_array);
    cJSON_Delete(int_array);
    cJSON_Delete(empty_array);

    free(doubles);
    free(ints);

    return 0;
}