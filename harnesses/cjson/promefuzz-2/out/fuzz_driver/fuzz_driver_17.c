// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
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
    cJSON *root;
    cJSON *item1;
    cJSON *item2;
    char *name1;
    char *name2;
    size_t split;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    split = (Size > 0) ? (Size / 2) : 0;

    name1 = (char *)malloc(split + 1);
    if (name1 == NULL) {
        cJSON_Delete(root);
        return 0;
    }
    if (split > 0) {
        memcpy(name1, Data, split);
    }
    name1[split] = '\0';

    name2 = (char *)malloc((Size - split) + 1);
    if (name2 == NULL) {
        free(name1);
        cJSON_Delete(root);
        return 0;
    }
    if ((Size - split) > 0) {
        memcpy(name2, Data + split, Size - split);
    }
    name2[Size - split] = '\0';

    item1 = cJSON_AddBoolToObject(root, name1, (Size > 0) ? (Data[0] & 1) : 0);
    (void)item1;

    item2 = cJSON_AddBoolToObject(root, name2, (Size > 1) ? (Data[Size - 1] & 1) : 1);
    (void)item2;

    free(name1);
    free(name2);
    cJSON_Delete(root);
    return 0;
}