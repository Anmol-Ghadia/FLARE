// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2162:22 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2162:22 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2162:22 in cJSON.h
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

int LLVMFuzzerTestOneInput_21(const uint8_t *Data, size_t Size) {
    cJSON *obj;
    cJSON *item;
    char *key;
    size_t key_len;

    obj = cJSON_CreateObject();
    if (obj == NULL) {
        return 0;
    }

    key_len = Size;
    key = (char *)malloc(key_len + 1);
    if (key == NULL) {
        cJSON_Delete(obj);
        return 0;
    }

    if (Size > 0) {
        memcpy(key, Data, Size);
    }
    key[key_len] = '\0';

    item = cJSON_AddTrueToObject(obj, key);
    (void)item;

    if (Size > 0) {
        size_t half = Size / 2;
        char *key2 = (char *)malloc(half + 1);
        if (key2 != NULL) {
            if (half > 0) {
                memcpy(key2, Data, half);
            }
            key2[half] = '\0';
            item = cJSON_AddTrueToObject(obj, key2);
            (void)item;
            free(key2);
        }
    }

    if (Size > 1) {
        char *key3 = (char *)malloc(Size + 1);
        if (key3 != NULL) {
            for (size_t i = 0; i < Size; ++i) {
                key3[i] = (char)(Data[Size - 1 - i]);
            }
            key3[Size] = '\0';
            item = cJSON_AddTrueToObject(obj, key3);
            (void)item;
            free(key3);
        }
    }

    free(key);
    cJSON_Delete(obj);
    return 0;
}