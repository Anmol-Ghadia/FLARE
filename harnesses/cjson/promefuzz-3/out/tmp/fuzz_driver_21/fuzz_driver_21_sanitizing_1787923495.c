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

    char *key = (char *)malloc(Size + 1);
    if (key == NULL) {
        cJSON_Delete(object);
        return 0;
    }

    if (Size > 0) {
        memcpy(key, Data, Size);
    }
    key[Size] = '\0';

    (void)cJSON_AddTrueToObject(object, key);

    if (Size > 0) {
        size_t half = Size / 2;
        key[half] = '\0';
        (void)cJSON_AddTrueToObject(object, key);
    }

    if (Size > 1) {
        char saved = key[0];
        key[0] = '\0';
        (void)cJSON_AddTrueToObject(object, key);
        key[0] = saved;
    }

    cJSON_Delete(object);
    free(key);
    return 0;
}