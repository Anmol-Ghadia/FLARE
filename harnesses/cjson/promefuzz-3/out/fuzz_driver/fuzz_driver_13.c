// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArrayReference at cJSON.c:2529:23 in cJSON.h
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
#include <math.h>
#include "cJSON.h"

static double data_to_double(const uint8_t *Data, size_t Size) {
    double d = 0.0;
    if (Size >= sizeof(double)) {
        memcpy(&d, Data, sizeof(double));
    } else if (Size > 0) {
        uint8_t buf[sizeof(double)] = {0};
        memcpy(buf, Data, Size);
        memcpy(&d, buf, sizeof(double));
    }
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num = data_to_double(Data, Size);

    if (Size > sizeof(double)) {
        switch (Data[sizeof(double)] % 6) {
            case 0:
                num = 0.0;
                break;
            case 1:
                num = -0.0;
                break;
            case 2:
                num = 1.0 / 0.0;
                break;
            case 3:
                num = -1.0 / 0.0;
                break;
            case 4:
                num = NAN;
                break;
            default:
                break;
        }
    }

    cJSON *array = cJSON_CreateArray();
    cJSON *number = cJSON_CreateNumber(num);

    if (array != NULL && number != NULL) {
        (void)cJSON_AddItemToArray(array, number);
    } else if (number != NULL) {
        cJSON_Delete(number);
        number = NULL;
    }

    cJSON *array_ref = cJSON_CreateArrayReference(array ? array->child : NULL);

    cJSON_Delete(array_ref);
    cJSON_Delete(array);

    return 0;
}