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

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*offset < size) {
            v |= data[*offset];
            (*offset)++;
        }
    }
    return v;
}

static double read_double_like(const uint8_t *data, size_t size, size_t *offset) {
    union {
        uint64_t u;
        double d;
    } conv;
    size_t i;
    conv.u = 0;
    for (i = 0; i < 8; ++i) {
        conv.u <<= 8;
        if (*offset < size) {
            conv.u |= data[*offset];
            (*offset)++;
        }
    }
    if (isnan(conv.d) || isinf(conv.d)) {
        return (double)(int64_t)conv.u;
    }
    return conv.d;
}

static char *make_string(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t len32;
    size_t len;
    char *out;

    len32 = read_u32(data, size, offset);
    if (*offset > size) {
        return NULL;
    }

    len = (size_t)len32;
    if (len > size - *offset) {
        len = size - *offset;
    }

    out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(out, data + *offset, len);
    }
    out[len] = '\0';
    *offset += len;
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    char *key1 = NULL;
    char *val1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    double num1, num2;
    cJSON *root = NULL;
    cJSON *str_item = NULL;
    cJSON *arr = NULL;
    cJSON *obj_in_arr = NULL;
    cJSON *num_item1 = NULL;
    cJSON *num_item2 = NULL;
    char *printed = NULL;

    key1 = make_string(Data, Size, &offset);
    val1 = make_string(Data, Size, &offset);
    key2 = make_string(Data, Size, &offset);
    key3 = make_string(Data, Size, &offset);
    num1 = read_double_like(Data, Size, &offset);
    num2 = read_double_like(Data, Size, &offset);

    if (key1 == NULL || val1 == NULL || key2 == NULL || key3 == NULL) {
        free(key1);
        free(val1);
        free(key2);
        free(key3);
        return 0;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        goto cleanup;
    }

    str_item = cJSON_CreateString(val1);
    if (str_item == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(root, key1, str_item)) {
        cJSON_Delete(str_item);
        str_item = NULL;
        goto cleanup;
    }
    str_item = NULL;

    arr = cJSON_CreateArray();
    if (arr == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(root, key2, arr)) {
        cJSON_Delete(arr);
        arr = NULL;
        goto cleanup;
    }

    obj_in_arr = cJSON_CreateObject();
    if (obj_in_arr == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToArray(arr, obj_in_arr)) {
        cJSON_Delete(obj_in_arr);
        obj_in_arr = NULL;
        goto cleanup;
    }

    num_item1 = cJSON_CreateNumber(num1);
    if (num_item1 == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(root, key3, num_item1)) {
        cJSON_Delete(num_item1);
        num_item1 = NULL;
        goto cleanup;
    }
    num_item1 = NULL;

    num_item2 = cJSON_CreateNumber(num2);
    if (num_item2 == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(obj_in_arr, "nested_number", num_item2)) {
        cJSON_Delete(num_item2);
        num_item2 = NULL;
        goto cleanup;
    }
    num_item2 = NULL;

    printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
        printed = NULL;
    }

cleanup:
    cJSON_Delete(root);
    free(key1);
    free(val1);
    free(key2);
    free(key3);
    return 0;
}