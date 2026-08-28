// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2174:22 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2174:22 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2174:22 in cJSON.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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

    (void)cJSON_AddFalseToObject(root, name);

    if (Size > 0) {
        size_t half = Size / 2;
        name[half] = '\0';
        (void)cJSON_AddFalseToObject(root, name);
    }

    if (Size > 1) {
        char *name2 = (char *)malloc(Size + 1);
        if (name2 != NULL) {
            for (size_t i = 0; i < Size; ++i) {
                uint8_t b = Data[Size - 1 - i];
                name2[i] = (char)((b % 94) + 32);
            }
            name2[Size] = '\0';
            (void)cJSON_AddFalseToObject(root, name2);
            free(name2);
        }
    }

    free(name);
    cJSON_Delete(root);
    return 0;
}