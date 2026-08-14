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

    (void)cJSON_AddTrueToObject(object, name);

    if (Size > 1) {
        size_t mid = Size / 2;
        name[mid] = '\0';
        (void)cJSON_AddTrueToObject(object, name);
    }

    if (Size > 0) {
        size_t alt_len = Size < 32 ? Size : 32;
        char alt_name[33];
        for (size_t i = 0; i < alt_len; ++i) {
            alt_name[i] = (char)((Data[i] % 94) + 33);
        }
        alt_name[alt_len] = '\0';
        (void)cJSON_AddTrueToObject(object, alt_name);
    }

    free(name);
    cJSON_Delete(object);
    return 0;
}