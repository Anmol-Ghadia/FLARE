#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *array1 = cJSON_CreateArray();
    cJSON *array2 = cJSON_CreateArray();
    cJSON *array3 = cJSON_CreateArray();
    cJSON *dup = NULL;
    cJSON *detached = NULL;
    int which = 0;
    cJSON_bool recurse = 0;

    if ((array1 == NULL) || (array2 == NULL) || (array3 == NULL))
    {
        cJSON_Delete(array1);
        cJSON_Delete(array2);
        cJSON_Delete(array3);
        return 0;
    }

    (void)cJSON_AddItemToArray(array1, array2);
    (void)cJSON_AddItemToArray(array1, array3);

    {
        cJSON *extra = cJSON_CreateArray();
        if (extra != NULL)
        {
            (void)cJSON_AddItemToArray(array1, extra);
        }
    }

    recurse = (Size > 0) ? (cJSON_bool)(Data[0] & 1) : 0;
    dup = cJSON_Duplicate(array1, recurse);

    if (Size > 1)
    {
        which = (int)((int8_t)Data[1]);
    }
    else
    {
        which = 0;
    }

    detached = cJSON_DetachItemFromArray(array1, which);
    cJSON_Delete(detached);

    cJSON_Delete(dup);
    cJSON_Delete(array1);

    return 0;
}