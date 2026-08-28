// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *item1;
    cJSON *item2;
    cJSON *item3;
    cJSON *array;
    cJSON_bool ok1 = 0, ok2 = 0, ok3 = 0;

    (void)Data;
    (void)Size;

    item1 = cJSON_CreateNull();
    item2 = cJSON_CreateNull();
    item3 = cJSON_CreateNull();
    array = cJSON_CreateArray();

    if (array != NULL) {
        if (item1 != NULL) {
            ok1 = cJSON_AddItemToArray(array, item1);
        }
        if (item2 != NULL) {
            ok2 = cJSON_AddItemToArray(array, item2);
        }
        if (item3 != NULL) {
            ok3 = cJSON_AddItemToArray(array, item3);
        }
    }

    if (!ok1 && item1 != NULL) {
        cJSON_free(item1);
    }
    if (!ok2 && item2 != NULL) {
        cJSON_free(item2);
    }
    if (!ok3 && item3 != NULL) {
        cJSON_free(item3);
    }

    if (array != NULL) {
        cJSON_Delete(array);
    }

    return 0;
}