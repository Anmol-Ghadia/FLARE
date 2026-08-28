// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    (void)Data;
    (void)Size;

    cJSON *item1 = cJSON_CreateNull();
    cJSON *item2 = cJSON_CreateNull();
    cJSON *item3 = cJSON_CreateNull();
    cJSON *array = cJSON_CreateArray();

    if (array != NULL)
    {
        if (!cJSON_AddItemToArray(array, item1))
        {
            cJSON_Delete(item1);
            item1 = NULL;
        }
        else
        {
            item1 = NULL;
        }

        if (!cJSON_AddItemToArray(array, item2))
        {
            cJSON_Delete(item2);
            item2 = NULL;
        }
        else
        {
            item2 = NULL;
        }

        if (!cJSON_AddItemToArray(array, item3))
        {
            cJSON_Delete(item3);
            item3 = NULL;
        }
        else
        {
            item3 = NULL;
        }

        cJSON_Delete(array);
        array = NULL;
    }
    else
    {
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        cJSON_Delete(item3);
        item1 = NULL;
        item2 = NULL;
        item3 = NULL;
    }

    cJSON_free(NULL);
    return 0;
}