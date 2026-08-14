// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size)
{
    (void)Data;
    (void)Size;

    cJSON *item1 = cJSON_CreateNull();
    cJSON *item2 = cJSON_CreateNull();
    cJSON *item3 = cJSON_CreateNull();
    cJSON *array = cJSON_CreateArray();

    if (array != NULL) {
        if (item1 != NULL) {
            if (cJSON_AddItemToArray(array, item1)) {
                item1 = NULL;
            }
        }
        if (item2 != NULL) {
            if (cJSON_AddItemToArray(array, item2)) {
                item2 = NULL;
            }
        }
        if (item3 != NULL) {
            if (cJSON_AddItemToArray(array, item3)) {
                item3 = NULL;
            }
        }
    }

    if (item1 != NULL) {
        cJSON_free(item1);
    }
    if (item2 != NULL) {
        cJSON_free(item2);
    }
    if (item3 != NULL) {
        cJSON_free(item3);
    }

    if (array != NULL) {
        cJSON *child = array->child;
        while (child != NULL) {
            cJSON *next = child->next;
            cJSON_free(child);
            child = next;
        }
        cJSON_free(array);
    }

    return 0;
}