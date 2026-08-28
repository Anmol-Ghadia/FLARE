// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2571:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput_13(const uint8_t *Data, size_t Size) {
    double num = 0.0;
    cJSON *array = NULL;
    cJSON *number = NULL;
    cJSON *array_ref = NULL;

    if (Size >= sizeof(num)) {
        memcpy(&num, Data, sizeof(num));
    } else {
        uint8_t buf[sizeof(num)] = {0};
        if (Size > 0) {
            memcpy(buf, Data, Size);
        }
        memcpy(&num, buf, sizeof(num));
    }

    array = cJSON_CreateArray();
    if (array == NULL) {
        return 0;
    }

    number = cJSON_CreateNumber(num);
    if (number == NULL) {
        cJSON_Delete(array);
        return 0;
    }

    if (!cJSON_AddItemToArray(array, number)) {
        cJSON_Delete(number);
        cJSON_Delete(array);
        return 0;
    }

    array_ref = cJSON_CreateArrayReference(array->child);

    cJSON_Delete(array_ref);
    cJSON_Delete(array);

    return 0;
}