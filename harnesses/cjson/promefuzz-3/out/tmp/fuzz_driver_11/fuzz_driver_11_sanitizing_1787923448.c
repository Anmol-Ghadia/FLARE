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