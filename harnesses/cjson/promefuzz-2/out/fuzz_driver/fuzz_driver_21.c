// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2120:22 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2120:22 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2120:22 in cJSON.h
// cJSON_AddTrueToObject at cJSON.c:2120:22 in cJSON.h
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
    cJSON *obj;
    cJSON *added;
    char *name;

    obj = cJSON_CreateObject();
    if (obj == NULL) {
        return 0;
    }

    name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(obj);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[Size] = '\0';

    added = cJSON_AddTrueToObject(obj, name);

    if (added == NULL && Size > 0) {
        size_t half = Size / 2;
        name[half] = '\0';
        (void)cJSON_AddTrueToObject(obj, name);
    }

    if (Size == 0) {
        (void)cJSON_AddTrueToObject(obj, "");
    } else if ((Data[0] & 1u) != 0) {
        (void)cJSON_AddTrueToObject(obj, "fixed");
    }

    free(name);
    cJSON_Delete(obj);
    return 0;
}