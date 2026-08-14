// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
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

static char *make_string_from_data(const uint8_t *data, size_t size, size_t *consumed)
{
    size_t len;
    char *out;

    if (size == 0) {
        out = (char *)malloc(1);
        if (out != NULL) {
            out[0] = '\0';
        }
        if (consumed != NULL) {
            *consumed = 0;
        }
        return out;
    }

    len = data[0];
    if (len > size - 1) {
        len = size - 1;
    }

    out = (char *)malloc(len + 1);
    if (out == NULL) {
        if (consumed != NULL) {
            *consumed = 0;
        }
        return NULL;
    }

    memcpy(out, data + 1, len);
    out[len] = '\0';

    if (consumed != NULL) {
        *consumed = len + 1;
    }
    return out;
}

static double make_double_from_data(const uint8_t *data, size_t size)
{
    union {
        uint64_t u64;
        double d;
    } u;
    size_t i;
    uint64_t v = 0;

    for (i = 0; i < 8; i++) {
        v <<= 8;
        if (i < size) {
            v |= data[i];
        }
    }

    u.u64 = v;

    if (!isfinite(u.d)) {
        return (double)(int64_t)v;
    }

    return u.d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *str_item = NULL;
    cJSON *array_item = NULL;
    cJSON *inner_obj = NULL;
    cJSON *num1 = NULL;
    cJSON *num2 = NULL;
    char *printed = NULL;

    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *strval = NULL;

    size_t off = 0;
    size_t used = 0;
    double d1, d2;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    key1 = make_string_from_data(Data + off, Size - off, &used);
    off += used;
    if (key1 == NULL) {
        goto cleanup;
    }

    strval = make_string_from_data(Data + off, Size - off, &used);
    off += used;
    if (strval == NULL) {
        goto cleanup;
    }

    str_item = cJSON_CreateString(strval);
    if (str_item == NULL) {
        goto cleanup;
    }

    if (!cJSON_AddItemToObject(root, key1, str_item)) {
        cJSON_Delete(str_item);
        str_item = NULL;
        goto cleanup;
    }
    str_item = NULL;

    array_item = cJSON_CreateArray();
    if (array_item == NULL) {
        goto cleanup;
    }

    key2 = make_string_from_data(Data + off, Size - off, &used);
    off += used;
    if (key2 == NULL) {
        goto cleanup;
    }

    if (!cJSON_AddItemToObject(root, key2, array_item)) {
        cJSON_Delete(array_item);
        array_item = NULL;
        goto cleanup;
    }

    inner_obj = cJSON_CreateObject();
    if (inner_obj == NULL) {
        goto cleanup;
    }

    if (!cJSON_AddItemToArray(array_item, inner_obj)) {
        cJSON_Delete(inner_obj);
        inner_obj = NULL;
        goto cleanup;
    }

    d1 = make_double_from_data(Data + off, Size - off);
    if (Size - off >= 8) {
        off += 8;
    } else {
        off = Size;
    }

    num1 = cJSON_CreateNumber(d1);
    if (num1 == NULL) {
        goto cleanup;
    }

    key3 = make_string_from_data(Data + off, Size - off, &used);
    off += used;
    if (key3 == NULL) {
        goto cleanup;
    }

    if (!cJSON_AddItemToObject(inner_obj, key3, num1)) {
        cJSON_Delete(num1);
        num1 = NULL;
        goto cleanup;
    }
    num1 = NULL;

    d2 = make_double_from_data(Data + off, Size - off);
    num2 = cJSON_CreateNumber(d2);
    if (num2 == NULL) {
        goto cleanup;
    }

    if (!cJSON_AddItemToObject(root, "number2", num2)) {
        cJSON_Delete(num2);
        num2 = NULL;
        goto cleanup;
    }
    num2 = NULL;

    printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
        printed = NULL;
    }

cleanup:
    free(key1);
    free(key2);
    free(key3);
    free(strval);
    cJSON_Delete(root);
    return 0;
}