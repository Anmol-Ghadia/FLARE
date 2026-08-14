// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2579:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateFloatArray at cJSON.c:2619:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
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
#include "cJSON.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; i++) {
        v <<= 8;
        if (*offset < size) {
            v |= data[*offset];
            (*offset)++;
        }
    }
    return v;
}

static int consume_int(const uint8_t *data, size_t size, size_t *offset) {
    return (int)read_u32(data, size, offset);
}

static float consume_float(const uint8_t *data, size_t size, size_t *offset) {
    union {
        uint32_t u;
        float f;
    } conv;
    conv.u = read_u32(data, size, offset);
    return conv.f;
}

int LLVMFuzzerTestOneInput_32(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    int i;

    cJSON *root = cJSON_CreateArray();
    if (root == NULL) {
        return 0;
    }

    int int_count = 0;
    if (Size > 0) {
        int_count = (int)(Data[offset++] % 16);
    }

    int *ints = NULL;
    if (int_count > 0) {
        ints = (int *)malloc((size_t)int_count * sizeof(int));
        if (ints != NULL) {
            for (i = 0; i < int_count; i++) {
                ints[i] = consume_int(Data, Size, &offset);
            }
        } else {
            int_count = 0;
        }
    }

    cJSON *int_array = cJSON_CreateIntArray(ints, int_count);
    if (int_array != NULL) {
        if ((Size > 1) && (Data[1] & 1)) {
            (void)cJSON_AddItemToArray(root, int_array);
        } else {
            (void)cJSON_AddItemReferenceToArray(root, int_array);
            cJSON_Delete(int_array);
        }
    }

    int float_count = 0;
    if (offset < Size) {
        float_count = (int)(Data[offset++] % 16);
    }

    float *floats = NULL;
    if (float_count > 0) {
        floats = (float *)malloc((size_t)float_count * sizeof(float));
        if (floats != NULL) {
            for (i = 0; i < float_count; i++) {
                floats[i] = consume_float(Data, Size, &offset);
            }
        } else {
            float_count = 0;
        }
    }

    cJSON *float_array = cJSON_CreateFloatArray(floats, float_count);
    if (float_array != NULL) {
        if ((Size > 2) && (Data[2] & 1)) {
            (void)cJSON_AddItemToArray(root, float_array);
        } else {
            (void)cJSON_AddItemReferenceToArray(root, float_array);
            cJSON_Delete(float_array);
        }
    }

    free(ints);
    free(floats);

    int extra_arrays = 0;
    if (offset < Size) {
        extra_arrays = (int)(Data[offset++] % 8);
    }

    for (i = 0; i < extra_arrays; i++) {
        cJSON *tmp = cJSON_CreateArray();
        if (tmp == NULL) {
            continue;
        }

        if (offset < Size && (Data[offset++] & 1)) {
            (void)cJSON_AddItemToArray(root, tmp);
        } else {
            (void)cJSON_AddItemReferenceToArray(root, tmp);
            cJSON_Delete(tmp);
        }
    }

    int delete_ops = 0;
    if (offset < Size) {
        delete_ops = (int)(Data[offset++] % 16);
    }

    for (i = 0; i < delete_ops; i++) {
        int which;
        if (offset < Size) {
            which = (int)((int8_t)Data[offset++]);
        } else {
            which = i;
        }
        cJSON_DeleteItemFromArray(root, which);
    }

    if (offset < Size && (Data[offset] & 1)) {
        cJSON_DeleteItemFromArray(NULL, consume_int(Data, Size, &offset));
    }

    cJSON_Delete(root);
    return 0;
}