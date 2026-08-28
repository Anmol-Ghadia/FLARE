// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2529:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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
#include <string.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num = 0.0;

    if (Size >= sizeof(double)) {
        memcpy(&num, Data, sizeof(double));
    } else {
        uint8_t buf[sizeof(double)] = {0};
        if (Size > 0) {
            memcpy(buf, Data, Size);
        }
        memcpy(&num, buf, sizeof(double));
    }

    cJSON *array = cJSON_CreateArray();
    cJSON *number = cJSON_CreateNumber(num);

    if (array != NULL && number != NULL) {
        (void)cJSON_AddItemToArray(array, number);

        cJSON *array_ref = cJSON_CreateArrayReference(array->child);

        cJSON_Delete(array_ref);
        cJSON_Delete(array);
    } else {
        if (number != NULL) {
            cJSON_Delete(number);
        }
        if (array != NULL) {
            cJSON_Delete(array);
        }
    }

    return 0;
}