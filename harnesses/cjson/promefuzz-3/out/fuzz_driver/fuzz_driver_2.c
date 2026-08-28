// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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

static double read_double_from_data(const uint8_t *data, size_t size, size_t offset)
{
    double out = 0.0;
    if (data == NULL || offset >= size) {
        return 0.0;
    }

    size_t remaining = size - offset;
    size_t copy_size = remaining < sizeof(double) ? remaining : sizeof(double);
    memcpy(&out, data + offset, copy_size);
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *str_item = NULL;
    cJSON *array = NULL;
    cJSON *inner_obj = NULL;
    cJSON *num1 = NULL;
    cJSON *num2 = NULL;
    char *printed = NULL;

    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *str_value = NULL;

    size_t pos = 0;
    size_t len1, len2, len3, len4;
    double d1, d2;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    len1 = (Size > pos) ? Data[pos++] : 0;
    len2 = (Size > pos) ? Data[pos++] : 0;
    len3 = (Size > pos) ? Data[pos++] : 0;
    len4 = (Size > pos) ? Data[pos++] : 0;

    if (len1 > Size - pos) len1 = Size - pos;
    key1 = (char *)malloc(len1 + 1);
    if (key1 == NULL) {
        cJSON_Delete(root);
        return 0;
    }
    memcpy(key1, Data + pos, len1);
    key1[len1] = '\0';
    pos += len1;

    if (len2 > Size - pos) len2 = Size - pos;
    str_value = (char *)malloc(len2 + 1);
    if (str_value == NULL) {
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    memcpy(str_value, Data + pos, len2);
    str_value[len2] = '\0';
    pos += len2;

    if (len3 > Size - pos) len3 = Size - pos;
    key2 = (char *)malloc(len3 + 1);
    if (key2 == NULL) {
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    memcpy(key2, Data + pos, len3);
    key2[len3] = '\0';
    pos += len3;

    if (len4 > Size - pos) len4 = Size - pos;
    key3 = (char *)malloc(len4 + 1);
    if (key3 == NULL) {
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    memcpy(key3, Data + pos, len4);
    key3[len4] = '\0';
    pos += len4;

    d1 = read_double_from_data(Data, Size, pos);
    if (pos < Size) {
        pos += (Size - pos >= sizeof(double)) ? sizeof(double) : (Size - pos);
    }
    d2 = read_double_from_data(Data, Size, pos);

    str_item = cJSON_CreateString(str_value);
    if (str_item == NULL) {
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }

    if (!cJSON_AddItemToObject(root, key1, str_item)) {
        cJSON_Delete(str_item);
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    str_item = NULL;

    array = cJSON_CreateArray();
    if (array == NULL) {
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }

    if (!cJSON_AddItemToObject(root, key2, array)) {
        cJSON_Delete(array);
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    array = NULL;

    array = root->child;
    while (array != NULL && array->string != NULL && strcmp(array->string, key2) != 0) {
        array = array->next;
    }

    inner_obj = cJSON_CreateObject();
    if (inner_obj == NULL) {
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }

    if (array == NULL || !cJSON_AddItemToArray(array, inner_obj)) {
        cJSON_Delete(inner_obj);
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    inner_obj = NULL;

    inner_obj = array->child;

    num1 = cJSON_CreateNumber(d1);
    if (num1 == NULL) {
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }

    if (inner_obj == NULL || !cJSON_AddItemToObject(inner_obj, key3, num1)) {
        cJSON_Delete(num1);
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    num1 = NULL;

    num2 = cJSON_CreateNumber(d2);
    if (num2 == NULL) {
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }

    if (inner_obj == NULL || !cJSON_AddItemToObject(inner_obj, "alt", num2)) {
        cJSON_Delete(num2);
        free(key3);
        free(key2);
        free(str_value);
        free(key1);
        cJSON_Delete(root);
        return 0;
    }
    num2 = NULL;

    printed = cJSON_Print(root);
    free(printed);

    free(key3);
    free(key2);
    free(str_value);
    free(key1);
    cJSON_Delete(root);
    return 0;
}