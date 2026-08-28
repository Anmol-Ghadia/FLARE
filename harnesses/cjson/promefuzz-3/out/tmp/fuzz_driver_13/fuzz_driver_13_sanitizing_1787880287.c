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

static double read_double_from_data(const uint8_t *Data, size_t Size) {
    double value = 0.0;
    size_t n = Size < sizeof(double) ? Size : sizeof(double);
    if (n > 0) {
        memcpy(&value, Data, n);
    }
    return value;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num = read_double_from_data(Data, Size);

    if (Size > sizeof(double)) {
        switch (Data[sizeof(double)] % 8) {
            case 0:
                num = 0.0;
                break;
            case 1:
                num = -0.0;
                break;
            case 2:
                num = 1.0;
                break;
            case 3:
                num = -1.0;
                break;
            case 4:
                num = 1e308;
                break;
            case 5:
                num = -1e308;
                break;
            case 6:
                num = INFINITY;
                break;
            case 7:
                num = NAN;
                break;
        }
    }

    cJSON *array = cJSON_CreateArray();
    if (array == NULL) {
        return 0;
    }

    cJSON *number = cJSON_CreateNumber(num);
    if (number != NULL) {
        (void)cJSON_AddItemToArray(array, number);
    }

    cJSON *array_ref = cJSON_CreateArrayReference(array->child);

    cJSON_Delete(array_ref);
    cJSON_Delete(array);

    return 0;
}