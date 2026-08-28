// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddNullToObject at cJSON.c:2150:22 in cJSON.h
// cJSON_AddNullToObject at cJSON.c:2150:22 in cJSON.h
// cJSON_AddNullToObject at cJSON.c:2150:22 in cJSON.h
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

int LLVMFuzzerTestOneInput_20(const uint8_t *Data, size_t Size) {
    cJSON *root;
    cJSON *added;
    char *key;
    size_t key_len;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    key_len = Size;
    key = (char *)malloc(key_len + 1);
    if (key == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    if (Size > 0) {
        memcpy(key, Data, Size);
    }
    key[key_len] = '\0';

    added = cJSON_AddNullToObject(root, key);
    (void)added;

    if (Size > 0) {
        size_t mid = Size / 2;
        char *key2 = (char *)malloc(mid + 1);
        if (key2 != NULL) {
            if (mid > 0) {
                memcpy(key2, Data, mid);
            }
            key2[mid] = '\0';
            (void)cJSON_AddNullToObject(root, key2);
            free(key2);
        }
    }

    if (Size == 0) {
        (void)cJSON_AddNullToObject(root, "");
    }

    free(key);
    cJSON_Delete(root);
    return 0;
}