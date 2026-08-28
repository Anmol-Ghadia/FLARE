// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2132:22 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2132:22 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2132:22 in cJSON.h
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

int LLVMFuzzerTestOneInput_16(const uint8_t *Data, size_t Size) {
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

    if (Size > 1) {
        size_t split = Data[0] % (Size + 1);
        char *name2 = (char *)malloc(split + 1);
        if (name2 != NULL) {
            if (split > 0) {
                memcpy(name2, Data, split);
            }
            name2[split] = '\0';
            (void)cJSON_AddFalseToObject(root, name2);
            free(name2);
        }
    }

    if (Size > 2) {
        size_t tail_len = Size - 1;
        char *name3 = (char *)malloc(tail_len + 1);
        if (name3 != NULL) {
            memcpy(name3, Data + 1, tail_len);
            name3[tail_len] = '\0';
            (void)cJSON_AddFalseToObject(root, name3);
            free(name3);
        }
    }

    free(name);
    cJSON_Delete(root);
    return 0;
}