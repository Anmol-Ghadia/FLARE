// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
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

int LLVMFuzzerTestOneInput_18(const uint8_t *Data, size_t Size) {
    cJSON *root;
    cJSON *item;
    char *name;
    char *raw;
    size_t split;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    split = (Size > 0) ? (size_t)(Data[0] % (Size + 1)) : 0;

    name = (char *)malloc(split + 1);
    if (name == NULL) {
        cJSON_Delete(root);
        return 0;
    }
    if (split > 0) {
        memcpy(name, Data, split);
    }
    name[split] = '\0';

    raw = (char *)malloc((Size - split) + 1);
    if (raw == NULL) {
        free(name);
        cJSON_Delete(root);
        return 0;
    }
    if (Size > split) {
        memcpy(raw, Data + split, Size - split);
    }
    raw[Size - split] = '\0';

    item = cJSON_AddRawToObject(root, name, raw);
    (void)item;

    if (Size > 1) {
        size_t alt_name_len = (size_t)(Data[Size - 1] % (Size + 1));
        if (alt_name_len > Size) {
            alt_name_len = Size;
        }

        char *name2 = (char *)malloc(alt_name_len + 1);
        char *raw2 = (char *)malloc(Size + 1);
        if (name2 != NULL && raw2 != NULL) {
            if (alt_name_len > 0) {
                memcpy(name2, Data, alt_name_len);
            }
            name2[alt_name_len] = '\0';

            if (Size > 0) {
                memcpy(raw2, Data, Size);
            }
            raw2[Size] = '\0';

            item = cJSON_AddRawToObject(root, name2, raw2);
            (void)item;
        }
        free(name2);
        free(raw2);
    }

    free(name);
    free(raw);
    cJSON_Delete(root);
    return 0;
}