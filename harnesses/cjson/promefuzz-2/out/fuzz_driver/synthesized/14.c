// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
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

static double read_double_from_data(const uint8_t *Data, size_t Size, size_t offset) {
    double value = 0.0;
    if (Data == NULL || offset >= Size) {
        return value;
    }

    size_t remaining = Size - offset;
    size_t copy_size = remaining < sizeof(double) ? remaining : sizeof(double);
    memcpy(&value, Data + offset, copy_size);
    return value;
}

static char *make_key_from_data(const uint8_t *Data, size_t Size, size_t offset, size_t max_len) {
    size_t len = 0;
    char *key = NULL;

    if (offset < Size) {
        len = Size - offset;
    }
    if (len > max_len) {
        len = max_len;
    }

    key = (char *)malloc(len + 1);
    if (key == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(key, Data + offset, len);
    }
    key[len] = '\0';

    return key;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
    double num1 = read_double_from_data(Data, Size, 0);
    double num2 = read_double_from_data(Data, Size, sizeof(double));

    char *key_add = make_key_from_data(Data, Size, 2 * sizeof(double), 32);
    char *key_replace = make_key_from_data(Data, Size, 2 * sizeof(double) + 32, 32);

    cJSON *object = cJSON_CreateObject();
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;

    if (object == NULL) {
        free(key_add);
        free(key_replace);
        return 0;
    }

    item1 = cJSON_CreateNumber(num1);
    item2 = cJSON_CreateNumber(num2);

    if (item1 != NULL) {
        if (key_add == NULL) {
            cJSON_Delete(item1);
            item1 = NULL;
        } else {
            if (!cJSON_AddItemToObject(object, key_add, item1)) {
                cJSON_Delete(item1);
                item1 = NULL;
            }
        }
    }

    if (item2 != NULL) {
        if (key_replace == NULL) {
            cJSON_Delete(item2);
            item2 = NULL;
        } else {
            if (!cJSON_ReplaceItemInObject(object, key_replace, item2)) {
                cJSON_Delete(item2);
                item2 = NULL;
            }
        }
    }

    cJSON_Delete(object);
    free(key_add);
    free(key_replace);
    return 0;
}