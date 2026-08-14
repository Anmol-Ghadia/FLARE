// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
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

    if (Size == 0) {
        cJSON_AddObjectToObject(root, "");
        cJSON_Delete(root);
        return 0;
    }

    char *name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    memcpy(name, Data, Size);
    name[Size] = '\0';

    cJSON *child1 = cJSON_AddObjectToObject(root, name);

    if (Size > 1) {
        size_t mid = Size / 2;
        name[mid] = '\0';
        cJSON *child2 = cJSON_AddObjectToObject(root, name);

        if (child1 != NULL) {
            cJSON_AddObjectToObject(child1, name);
        }
        if (child2 != NULL && mid + 1 < Size) {
            cJSON_AddObjectToObject(child2, (const char *)(name + mid + 1));
        }
    } else if (child1 != NULL) {
        cJSON_AddObjectToObject(child1, "");
    }

    free(name);
    cJSON_Delete(root);
    return 0;
}