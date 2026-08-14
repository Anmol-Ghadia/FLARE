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