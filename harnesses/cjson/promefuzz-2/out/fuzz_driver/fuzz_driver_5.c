// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2168:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
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

#include "cJSON.h"

static char *make_cstring(const uint8_t *data, size_t len) {
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(s, data, len);
    }
    s[len] = '\0';
    return s;
}

static double bytes_to_double(const uint8_t *data, size_t len, size_t offset) {
    double result = 0.0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        unsigned char b = 0;
        if (offset + i < len) {
            b = data[offset + i];
        }
        result = result * 257.0 + (double)b;
    }
    if (offset < len && (data[offset] & 1)) {
        result = -result;
    }
    return result;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t p = 0;
    size_t key1_len, val1_len, arr_name_len, child_key1_len, child_key2_len;
    char *key1 = NULL, *val1 = NULL, *arr_name = NULL, *child_key1 = NULL, *child_key2 = NULL;
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *child = NULL;
    char *printed = NULL;
    double num1, num2;

    if (Data == NULL) {
        return 0;
    }

    key1_len = (p < Size) ? (Data[p++] % 32) : 0;
    if (key1_len > Size - p) key1_len = Size - p;
    key1 = make_cstring(Data + p, key1_len);
    if (key1 == NULL) goto cleanup;
    p += key1_len;

    val1_len = (p < Size) ? (Data[p++] % 64) : 0;
    if (val1_len > Size - p) val1_len = Size - p;
    val1 = make_cstring(Data + p, val1_len);
    if (val1 == NULL) goto cleanup;
    p += val1_len;

    arr_name_len = (p < Size) ? (Data[p++] % 32) : 0;
    if (arr_name_len > Size - p) arr_name_len = Size - p;
    arr_name = make_cstring(Data + p, arr_name_len);
    if (arr_name == NULL) goto cleanup;
    p += arr_name_len;

    child_key1_len = (p < Size) ? (Data[p++] % 32) : 0;
    if (child_key1_len > Size - p) child_key1_len = Size - p;
    child_key1 = make_cstring(Data + p, child_key1_len);
    if (child_key1 == NULL) goto cleanup;
    p += child_key1_len;

    child_key2_len = (p < Size) ? (Data[p++] % 32) : 0;
    if (child_key2_len > Size - p) child_key2_len = Size - p;
    child_key2 = make_cstring(Data + p, child_key2_len);
    if (child_key2 == NULL) goto cleanup;
    p += child_key2_len;

    num1 = bytes_to_double(Data, Size, p);
    p += (p + 8 <= Size) ? 8 : (Size - p);
    num2 = bytes_to_double(Data, Size, p);

    root = cJSON_CreateObject();
    if (root == NULL) goto cleanup;

    (void)cJSON_AddStringToObject(root, key1[0] ? key1 : "k", val1);

    array = cJSON_AddArrayToObject(root, arr_name[0] ? arr_name : "arr");
    if (array == NULL) goto cleanup;

    child = cJSON_CreateObject();
    if (child == NULL) goto cleanup;

    (void)cJSON_AddNumberToObject(child, child_key1[0] ? child_key1 : "n1", num1);
    (void)cJSON_AddNumberToObject(child, child_key2[0] ? child_key2 : "n2", num2);

    if (!cJSON_AddItemToArray(array, child)) {
        cJSON_Delete(child);
        child = NULL;
        goto cleanup;
    }
    child = NULL;

    printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
        printed = NULL;
    }

cleanup:
    cJSON_Delete(root);
    free(key1);
    free(val1);
    free(arr_name);
    free(child_key1);
    free(child_key2);
    return 0;
}