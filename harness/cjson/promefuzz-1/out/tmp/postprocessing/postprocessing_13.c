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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num = 0.0;
    cJSON *array = NULL;
    cJSON *number = NULL;
    cJSON *array_ref = NULL;
    cJSON_bool added = 0;

    if (Size >= sizeof(double)) {
        memcpy(&num, Data, sizeof(double));
    } else {
        uint8_t buf[sizeof(double)] = {0};
        if (Size > 0) {
            memcpy(buf, Data, Size);
        }
        memcpy(&num, buf, sizeof(double));
    }

    if (Size > sizeof(double)) {
        switch (Data[sizeof(double)] % 4) {
            case 0:
                break;
            case 1:
                num = -num;
                break;
            case 2:
                num = INFINITY;
                break;
            case 3:
                num = NAN;
                break;
        }
    }

    array = cJSON_CreateArray();
    number = cJSON_CreateNumber(num);

    if (array != NULL && number != NULL) {
        added = cJSON_AddItemToArray(array, number);
        if (!added) {
            cJSON_Delete(number);
            number = NULL;
        }
    } else if (number != NULL) {
        cJSON_Delete(number);
        number = NULL;
    }

    array_ref = cJSON_CreateArrayReference((array != NULL) ? array->child : NULL);

    cJSON_Delete(array_ref);
    cJSON_Delete(array);

    return 0;
}