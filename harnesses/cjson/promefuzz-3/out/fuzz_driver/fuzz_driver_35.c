// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_malloc at cJSON.c:3197:22 in cJSON.h
// cJSON_malloc at cJSON.c:3197:22 in cJSON.h
// cJSON_malloc at cJSON.c:3197:22 in cJSON.h
// cJSON_CreateDoubleArray at cJSON.c:2701:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *offset < size; i++, (*offset)++) {
        v |= ((uint32_t)data[*offset]) << (i * 8);
    }
    return v;
}

static double make_double_from_bytes(const uint8_t *data, size_t size, size_t *offset) {
    union {
        uint64_t u;
        double d;
    } conv;
    size_t i;
    conv.u = 0;
    for (i = 0; i < 8 && *offset < size; i++, (*offset)++) {
        conv.u |= ((uint64_t)data[*offset]) << (i * 8);
    }
    return conv.d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    uint32_t raw_count;
    int count;
    double *numbers = NULL;
    cJSON *arr_from_numbers = NULL;
    cJSON *empty_array = NULL;
    cJSON *empty_object = NULL;
    cJSON *dup1 = NULL;
    cJSON *dup2 = NULL;
    cJSON *dup3 = NULL;
    cJSON *dup4 = NULL;
    void *mem1 = NULL;
    void *mem2 = NULL;
    size_t alloc_size1, alloc_size2;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    raw_count = read_u32(Data, Size, &offset);
    count = (int)(raw_count % 64);

    if (count > 0) {
        int i;
        numbers = (double *)cJSON_malloc((size_t)count * sizeof(double));
        if (numbers != NULL) {
            for (i = 0; i < count; i++) {
                numbers[i] = make_double_from_bytes(Data, Size, &offset);
            }
        }
    }

    alloc_size1 = (size_t)(read_u32(Data, Size, &offset) % 4096);
    alloc_size2 = (size_t)(read_u32(Data, Size, &offset) % 4096);

    mem1 = cJSON_malloc(alloc_size1);
    mem2 = cJSON_malloc(alloc_size2);

    arr_from_numbers = cJSON_CreateDoubleArray(numbers, count);
    empty_array = cJSON_CreateArray();
    empty_object = cJSON_CreateObject();

    (void)cJSON_GetArraySize(NULL);
    if (arr_from_numbers != NULL) {
        (void)cJSON_GetArraySize(arr_from_numbers);
    }
    if (empty_array != NULL) {
        (void)cJSON_GetArraySize(empty_array);
    }
    if (empty_object != NULL) {
        (void)cJSON_GetArraySize(empty_object);
    }

    dup1 = cJSON_Duplicate(arr_from_numbers, 0);
    dup2 = cJSON_Duplicate(arr_from_numbers, 1);
    dup3 = cJSON_Duplicate(empty_array, 1);
    dup4 = cJSON_Duplicate(empty_object, 1);
    (void)cJSON_Duplicate(NULL, 0);
    (void)cJSON_Duplicate(NULL, 1);

    if (dup1 != NULL) {
        (void)cJSON_GetArraySize(dup1);
    }
    if (dup2 != NULL) {
        (void)cJSON_GetArraySize(dup2);
    }
    if (dup3 != NULL) {
        (void)cJSON_GetArraySize(dup3);
    }
    if (dup4 != NULL) {
        (void)cJSON_GetArraySize(dup4);
    }

    if (mem1 != NULL) {
        cJSON_free(mem1);
    }
    if (mem2 != NULL) {
        cJSON_free(mem2);
    }
    if (numbers != NULL) {
        cJSON_free(numbers);
    }

    cJSON_Delete(dup1);
    cJSON_Delete(dup2);
    cJSON_Delete(dup3);
    cJSON_Delete(dup4);
    cJSON_Delete(arr_from_numbers);
    cJSON_Delete(empty_array);
    cJSON_Delete(empty_object);

    return 0;
}