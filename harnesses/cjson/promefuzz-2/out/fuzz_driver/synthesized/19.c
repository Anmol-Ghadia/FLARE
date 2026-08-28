// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
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

int LLVMFuzzerTestOneInput_19(const uint8_t *Data, size_t Size) {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    char *name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[Size] = '\0';

    (void)cJSON_AddObjectToObject(root, name);

    if (Size > 0) {
        size_t mid = Size / 2;
        char *name2 = (char *)malloc(mid + 1);
        if (name2 != NULL) {
            if (mid > 0) {
                memcpy(name2, Data, mid);
            }
            name2[mid] = '\0';
            (void)cJSON_AddObjectToObject(root, name2);
            free(name2);
        }
    }

    if (Size > 1) {
        char *name3 = (char *)malloc(Size + 1);
        if (name3 != NULL) {
            for (size_t i = 0; i < Size; ++i) {
                name3[i] = (char)Data[Size - 1 - i];
            }
            name3[Size] = '\0';
            (void)cJSON_AddObjectToObject(root, name3);
            free(name3);
        }
    }

    free(name);
    cJSON_Delete(root);
    return 0;
}