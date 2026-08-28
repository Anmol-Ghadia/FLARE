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

static char *make_cstring(const uint8_t *data, size_t size, size_t *consumed)
{
    size_t len = 0;
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

    if (len > 0) {
        memcpy(out, data + 1, len);
    }
    out[len] = '\0';

    if (consumed != NULL) {
        *consumed = len + 1;
    }
    return out;
}

static double make_double(const uint8_t *data, size_t size)
{
    union {
        uint64_t u64;
        double d;
    } u;
    size_t i;
    uint64_t v = 0;

    for (i = 0; i < 8; ++i) {
        v <<= 8;
        if (i < size) {
            v |= data[i];
        }
    }
    u.u64 = v;
    return u.d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *str_item = NULL;
    cJSON *arr = NULL;
    cJSON *obj_in_arr = NULL;
    cJSON *num1 = NULL;
    cJSON *num2 = NULL;
    char *print_buf = NULL;

    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *strval = NULL;

    size_t off = 0, used = 0;
    double d1, d2;

    key1 = make_cstring(Data + off, Size > off ? Size - off : 0, &used);
    off += used;
    key2 = make_cstring(Data + off, Size > off ? Size - off : 0, &used);
    off += used;
    key3 = make_cstring(Data + off, Size > off ? Size - off : 0, &used);
    off += used;
    strval = make_cstring(Data + off, Size > off ? Size - off : 0, &used);
    off += used;

    d1 = make_double(Data + off, Size > off ? Size - off : 0);
    if (Size > off) {
        off += (Size - off >= 8) ? 8 : (Size - off);
    }
    d2 = make_double(Data + off, Size > off ? Size - off : 0);

    root = cJSON_CreateObject();
    if (root == NULL) {
        goto cleanup;
    }

    str_item = cJSON_CreateString(strval != NULL ? strval : "");
    if (str_item == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(root, key1 != NULL ? key1 : "", str_item)) {
        cJSON_Delete(str_item);
        str_item = NULL;
        goto cleanup;
    }
    str_item = NULL;

    arr = cJSON_CreateArray();
    if (arr == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(root, key2 != NULL ? key2 : "", arr)) {
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

    num1 = cJSON_CreateNumber(d1);
    if (num1 == NULL) {
        goto cleanup;
    }
    if (!cJSON_AddItemToObject(obj_in_arr, key3 != NULL ? key3 : "", num1)) {
        cJSON_Delete(num1);
        num1 = NULL;
        goto cleanup;
    }
    num1 = NULL;

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

    print_buf = cJSON_Print(root);
    free(print_buf);

cleanup:
    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(key3);
    free(strval);
    return 0;
}