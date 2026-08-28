#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *array1 = cJSON_CreateArray();
    cJSON *array2 = cJSON_CreateArray();
    cJSON *array3 = cJSON_CreateArray();
    cJSON *dup = NULL;
    cJSON *detached = NULL;

    if (array1 == NULL || array2 == NULL || array3 == NULL) {
        cJSON_Delete(array1);
        cJSON_Delete(array2);
        cJSON_Delete(array3);
        return 0;
    }

    (void)cJSON_AddItemToArray(array1, array2);
    (void)cJSON_AddItemToArray(array1, array3);

    /* Explore an additional state: optionally add a NULL item. */
    if (Size > 0 && (Data[0] & 1)) {
        (void)cJSON_AddItemToArray(array1, NULL);
    } else {
        cJSON *array4 = cJSON_CreateArray();
        if (array4 != NULL) {
            (void)cJSON_AddItemToArray(array1, array4);
        }
    }

    dup = cJSON_Duplicate(array1, (Size > 1) ? (cJSON_bool)(Data[1] & 1) : 0);

    {
        int which = 0;
        if (Size > 2) {
            which = (int)((int8_t)Data[2]);
        }
        detached = cJSON_DetachItemFromArray(array1, which);
    }

    cJSON_Delete(detached);
    cJSON_Delete(dup);
    cJSON_Delete(array1);

    return 0;
}