// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2294:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput_12(const uint8_t *Data, size_t Size) {
    cJSON *array1 = cJSON_CreateArray();
    cJSON *array2 = cJSON_CreateArray();
    cJSON *array3 = cJSON_CreateArray();
    cJSON *dup = NULL;
    cJSON *detached = NULL;
    cJSON_bool recurse = 0;
    int which = 0;

    if ((array1 == NULL) || (array2 == NULL) || (array3 == NULL)) {
        cJSON_Delete(array1);
        cJSON_Delete(array2);
        cJSON_Delete(array3);
        return 0;
    }

    (void)cJSON_AddItemToArray(array1, array2);
    (void)cJSON_AddItemToArray(array1, array3);

    if (Size > 0 && (Data[0] & 1)) {
        cJSON *array4 = cJSON_CreateArray();
        if (array4 != NULL) {
            (void)cJSON_AddItemToArray(array1, array4);
        }
    } else {
        cJSON *array4 = cJSON_CreateArray();
        if (array4 != NULL) {
            (void)cJSON_AddItemToArray(array2, array4);
        }
    }

    recurse = (Size > 1) ? (Data[1] & 1) : 0;
    dup = cJSON_Duplicate(array1, recurse);

    if (Size > 2) {
        which = (int)Data[2] - 128;
    } else {
        which = 0;
    }

    detached = cJSON_DetachItemFromArray(array1, which);

    cJSON_Delete(detached);
    cJSON_Delete(dup);
    cJSON_Delete(array1);

    return 0;
}