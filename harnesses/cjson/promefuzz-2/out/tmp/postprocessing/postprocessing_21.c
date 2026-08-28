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
    cJSON *obj;
    cJSON *added;
    char *name;
    size_t name_len;

    obj = cJSON_CreateObject();
    if (obj == NULL) {
        return 0;
    }

    name_len = Size;
    name = (char *)malloc(name_len + 1);
    if (name == NULL) {
        cJSON_Delete(obj);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[name_len] = '\0';

    added = cJSON_AddTrueToObject(obj, name);
    (void)added;

    if (Size > 0) {
        size_t split = Data[0] % (Size + 1);
        char *name2 = (char *)malloc(split + 1);
        if (name2 != NULL) {
            if (split > 0) {
                memcpy(name2, Data, split);
            }
            name2[split] = '\0';
            added = cJSON_AddTrueToObject(obj, name2);
            (void)added;
            free(name2);
        }
    }

    free(name);
    cJSON_Delete(obj);
    return 0;
}