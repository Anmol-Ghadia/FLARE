// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
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

static double read_double_from_bytes(const uint8_t *data, size_t size, size_t offset) {
    uint64_t bits = 0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        bits <<= 8;
        if (offset + i < size) {
            bits |= data[offset + i];
        }
    }

    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = bits;
    return conv.d;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
    double num1 = 0.0;
    double num2 = 0.0;
    cJSON *object = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    size_t split;
    size_t key1_len, key2_len;
    cJSON_bool added, replaced;

    num1 = read_double_from_bytes(Data, Size, 0);
    num2 = read_double_from_bytes(Data, Size, 8);

    object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    item1 = cJSON_CreateNumber(num1);
    item2 = cJSON_CreateNumber(num2);

    if (item1 == NULL || item2 == NULL) {
        if (item1 != NULL) {
            cJSON_Delete(item1);
        }
        if (item2 != NULL) {
            cJSON_Delete(item2);
        }
        cJSON_Delete(object);
        return 0;
    }

    if (Size > 16) {
        split = 16 + ((size_t)Data[16] % (Size - 16 + 1));
    } else {
        split = 16;
    }

    if (split > Size) {
        split = Size;
    }

    key1_len = (split > 16) ? (split - 16) : 0;
    key2_len = (Size > split) ? (Size - split) : 0;

    key1 = (char *)malloc(key1_len + 1);
    key2 = (char *)malloc(key2_len + 1);
    if (key1 == NULL || key2 == NULL) {
        free(key1);
        free(key2);
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        cJSON_Delete(object);
        return 0;
    }

    if (key1_len > 0) {
        memcpy(key1, Data + 16, key1_len);
    }
    key1[key1_len] = '\0';

    if (key2_len > 0) {
        memcpy(key2, Data + split, key2_len);
    }
    key2[key2_len] = '\0';

    added = cJSON_AddItemToObject(object, key1, item1);
    if (!added) {
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        free(key1);
        free(key2);
        cJSON_Delete(object);
        return 0;
    }

    replaced = cJSON_ReplaceItemInObject(object, key2_len > 0 ? key2 : key1, item2);
    if (!replaced) {
        cJSON_Delete(item2);
    }

    free(key1);
    free(key2);
    cJSON_Delete(object);
    return 0;
}