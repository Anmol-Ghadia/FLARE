// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2375:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2375:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2375:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateFloatArray at cJSON.c:2619:23 in cJSON.h
// cJSON_CreateFloatArray at cJSON.c:2619:23 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *Offset < Size; ++i) {
        v |= ((uint32_t)Data[*Offset]) << (8 * i);
        (*Offset)++;
    }
    return v;
}

static int read_int(const uint8_t *Data, size_t Size, size_t *Offset) {
    return (int)read_u32(Data, Size, Offset);
}

static float read_float(const uint8_t *Data, size_t Size, size_t *Offset) {
    union {
        uint32_t u;
        float f;
    } conv;
    conv.u = read_u32(Data, Size, Offset);
    return conv.f;
}

int LLVMFuzzerTestOneInput_28(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *empty_array = cJSON_CreateArray();
    if (empty_array != NULL) {
        (void)cJSON_GetArraySize(empty_array);
        (void)cJSON_GetArrayItem(empty_array, 0);
        (void)cJSON_GetArrayItem(empty_array, -1);
        cJSON_DeleteItemFromArray(empty_array, 0);
        cJSON_DeleteItemFromArray(empty_array, -1);
        cJSON *replacement = cJSON_CreateArray();
        if (replacement != NULL) {
            if (!cJSON_ReplaceItemInArray(empty_array, 0, replacement)) {
                cJSON_Delete(replacement);
            }
        }
        cJSON_Delete(empty_array);
    }

    int count = 0;
    if (Size >= 4) {
        count = read_int(Data, Size, &offset);
        if (count < 0) {
            count = -count;
        }
        if (count > 256) {
            count = 256;
        }
    }

    float *numbers = NULL;
    if (count > 0) {
        numbers = (float *)malloc((size_t)count * sizeof(float));
        if (numbers == NULL) {
            count = 0;
        }
    }

    for (i = 0; i < count; ++i) {
        if (offset < Size) {
            numbers[i] = read_float(Data, Size, &offset);
        } else {
            numbers[i] = 0.0f;
        }
    }

    cJSON *array = cJSON_CreateFloatArray(numbers, count);
    if (array == NULL) {
        array = cJSON_CreateFloatArray(NULL, count);
    }

    if (array != NULL) {
        int size1 = cJSON_GetArraySize(array);
        int idx1 = read_int(Data, Size, &offset);
        int idx2 = read_int(Data, Size, &offset);
        int idx3 = read_int(Data, Size, &offset);

        (void)cJSON_GetArrayItem(array, idx1);
        (void)cJSON_GetArrayItem(array, idx2);
        (void)cJSON_GetArrayItem(array, -1);
        (void)cJSON_GetArrayItem(array, size1);
        if (size1 > 0) {
            (void)cJSON_GetArrayItem(array, size1 - 1);
        }

        cJSON *newitem1 = cJSON_CreateArray();
        if (newitem1 != NULL) {
            if (!cJSON_ReplaceItemInArray(array, idx1, newitem1)) {
                cJSON_Delete(newitem1);
            }
        }

        cJSON *newitem2 = cJSON_CreateArray();
        if (newitem2 != NULL) {
            if (!cJSON_ReplaceItemInArray(array, -1, newitem2)) {
                cJSON_Delete(newitem2);
            }
        }

        cJSON_DeleteItemFromArray(array, idx2);
        cJSON_DeleteItemFromArray(array, -1);
        cJSON_DeleteItemFromArray(array, idx3);

        (void)cJSON_GetArraySize(array);
        (void)cJSON_GetArrayItem(array, 0);

        cJSON_Delete(array);
    }

    free(numbers);

    (void)cJSON_GetArraySize(NULL);
    (void)cJSON_GetArrayItem(NULL, 0);
    cJSON_DeleteItemFromArray(NULL, 0);

    return 0;
}