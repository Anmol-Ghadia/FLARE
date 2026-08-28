#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    (void)Data;
    (void)Size;

    cJSON *item1 = cJSON_CreateNull();
    cJSON *item2 = cJSON_CreateNull();
    cJSON *item3 = cJSON_CreateNull();
    cJSON *array = cJSON_CreateArray();

    if (array != NULL) {
        if (!cJSON_AddItemToArray(array, item1) && item1 != NULL) {
            cJSON_free(item1);
            item1 = NULL;
        }
        if (!cJSON_AddItemToArray(array, item2) && item2 != NULL) {
            cJSON_free(item2);
            item2 = NULL;
        }
        if (!cJSON_AddItemToArray(array, item3) && item3 != NULL) {
            cJSON_free(item3);
            item3 = NULL;
        }
    } else {
        if (item1 != NULL) {
            cJSON_free(item1);
            item1 = NULL;
        }
        if (item2 != NULL) {
            cJSON_free(item2);
            item2 = NULL;
        }
        if (item3 != NULL) {
            cJSON_free(item3);
            item3 = NULL;
        }
    }

    cJSON_free(array);
    return 0;
}