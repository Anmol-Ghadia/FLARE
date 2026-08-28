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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    char *name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(object);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[Size] = '\0';

    (void)cJSON_AddFalseToObject(object, name);

    if (Size > 0) {
        size_t alt_len = Size / 2;
        char *alt_name = (char *)malloc(alt_len + 1);
        if (alt_name != NULL) {
            if (alt_len > 0) {
                memcpy(alt_name, Data, alt_len);
            }
            alt_name[alt_len] = '\0';
            (void)cJSON_AddFalseToObject(object, alt_name);
            free(alt_name);
        }
    }

    if (Size == 0) {
        (void)cJSON_AddFalseToObject(object, "");
    }

    free(name);
    cJSON_Delete(object);
    return 0;
}