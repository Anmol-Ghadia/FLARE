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
    cJSON *object;
    cJSON *added;
    char *name;
    size_t name_len;

    object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    name_len = Size;
    name = (char *)malloc(name_len + 1);
    if (name == NULL) {
        cJSON_Delete(object);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[name_len] = '\0';

    added = cJSON_AddNullToObject(object, name);
    (void)added;

    if (Size > 0) {
        size_t split = Data[0] % (Size + 1);
        char *name2 = (char *)malloc(split + 1);
        if (name2 != NULL) {
            if (split > 0) {
                memcpy(name2, Data, split);
            }
            name2[split] = '\0';
            added = cJSON_AddNullToObject(object, name2);
            (void)added;
            free(name2);
        }
    } else {
        added = cJSON_AddNullToObject(object, "");
        (void)added;
    }

    if (Size > 1) {
        size_t i;
        size_t max_items = Size < 32 ? Size : 32;
        for (i = 0; i < max_items; i++) {
            char keybuf[2];
            keybuf[0] = (char)Data[i];
            keybuf[1] = '\0';
            added = cJSON_AddNullToObject(object, keybuf);
            (void)added;
        }
    }

    free(name);
    cJSON_Delete(object);
    return 0;
}