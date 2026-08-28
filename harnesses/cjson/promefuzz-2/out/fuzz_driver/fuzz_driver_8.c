// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2579:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
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

static int read_int32(const uint8_t *data, size_t size, size_t *offset) {
    int value = 0;
    size_t i;
    for (i = 0; i < 4 && *offset < size; ++i, ++(*offset)) {
        value |= ((int)data[*offset]) << (i * 8);
    }
    return value;
}

static char *make_string(const uint8_t *data, size_t size, size_t *offset) {
    size_t remaining = (*offset < size) ? (size - *offset) : 0;
    size_t len = remaining > 64 ? 64 : remaining;
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(s, data + *offset, len);
    }
    s[len] = '\0';
    *offset += len;
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    int numbers[16];
    int count;
    int index;
    char *key1 = NULL;
    char *val1 = NULL;
    char *key2 = NULL;
    char *val2 = NULL;

    cJSON *root1 = NULL;
    cJSON *array = NULL;
    cJSON *item = NULL;
    cJSON *root2 = NULL;
    cJSON *str1 = NULL;
    cJSON *root3 = NULL;
    cJSON *str2 = NULL;

    for (size_t i = 0; i < 16; ++i) {
        numbers[i] = read_int32(Data, Size, &offset);
    }

    count = read_int32(Data, Size, &offset);
    if (count < 0) {
        count = -(count + 1);
    }
    count %= 17;

    index = read_int32(Data, Size, &offset);

    key1 = make_string(Data, Size, &offset);
    val1 = make_string(Data, Size, &offset);
    key2 = make_string(Data, Size, &offset);
    val2 = make_string(Data, Size, &offset);

    if (key1 == NULL || val1 == NULL || key2 == NULL || val2 == NULL) {
        free(key1);
        free(val1);
        free(key2);
        free(val2);
        return 0;
    }

    root1 = cJSON_CreateObject();
    array = cJSON_CreateIntArray(numbers, count);
    item = cJSON_GetArrayItem(array, index);
    if (root1 != NULL && array != NULL) {
        if (!cJSON_AddItemToObject(root1, key1, array)) {
            cJSON_Delete(array);
            array = NULL;
        } else {
            (void)item;
            array = NULL;
        }
    } else if (array != NULL) {
        cJSON_Delete(array);
        array = NULL;
    }

    root2 = cJSON_CreateObject();
    str1 = cJSON_CreateString(val1);
    if (root2 != NULL && str1 != NULL) {
        if (!cJSON_AddItemToObject(root2, key2, str1)) {
            cJSON_Delete(str1);
            str1 = NULL;
        } else {
            str1 = NULL;
        }
    } else if (str1 != NULL) {
        cJSON_Delete(str1);
        str1 = NULL;
    }

    root3 = cJSON_CreateObject();
    str2 = cJSON_CreateString(val2);
    if (root3 != NULL && str2 != NULL) {
        if (!cJSON_AddItemToObject(root3, key1, str2)) {
            cJSON_Delete(str2);
            str2 = NULL;
        } else {
            str2 = NULL;
        }
    } else if (str2 != NULL) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    cJSON_Delete(root3);
    cJSON_Delete(root2);
    cJSON_Delete(root1);

    free(key1);
    free(val1);
    free(key2);
    free(val2);

    return 0;
}