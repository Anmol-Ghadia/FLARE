// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2450:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

static double read_double_from_data(const uint8_t *data, size_t size, size_t offset) {
    double out = 0.0;
    if (offset < size) {
        size_t remaining = size - offset;
        size_t copy_size = remaining < sizeof(double) ? remaining : sizeof(double);
        memcpy(&out, data + offset, copy_size);
    }
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num1 = read_double_from_data(Data, Size, 0);
    double num2 = read_double_from_data(Data, Size, sizeof(double));

    size_t key_offset = 2 * sizeof(double);
    const char *fallback_key = "k";
    char *key = NULL;

    if (Size > key_offset) {
        size_t key_len = Size - key_offset;
        key = (char *)malloc(key_len + 1);
        if (key != NULL) {
            memcpy(key, Data + key_offset, key_len);
            key[key_len] = '\0';
        }
    }

    cJSON *object = cJSON_CreateObject();
    if (object == NULL) {
        free(key);
        return 0;
    }

    cJSON *item1 = cJSON_CreateNumber(num1);
    if (item1 == NULL) {
        cJSON_Delete(object);
        free(key);
        return 0;
    }

    const char *use_key = (key != NULL) ? key : fallback_key;
    if (!cJSON_AddItemToObject(object, use_key, item1)) {
        cJSON_Delete(item1);
        cJSON_Delete(object);
        free(key);
        return 0;
    }

    cJSON *item2 = cJSON_CreateNumber(num2);
    if (item2 != NULL) {
        if (!cJSON_ReplaceItemInObject(object, use_key, item2)) {
            cJSON_Delete(item2);
        }
    }

    cJSON_Delete(object);
    free(key);
    return 0;
}