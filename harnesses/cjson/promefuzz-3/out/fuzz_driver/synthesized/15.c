// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
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

static double read_double_from_data(const uint8_t *Data, size_t Size) {
    double value = 0.0;
    if (Size >= sizeof(double)) {
        memcpy(&value, Data, sizeof(double));
    } else if (Size > 0) {
        uint8_t buf[sizeof(double)] = {0};
        memcpy(buf, Data, Size);
        memcpy(&value, buf, sizeof(double));
    }
    return value;
}

int LLVMFuzzerTestOneInput_15(const uint8_t *Data, size_t Size) {
    char *strbuf = NULL;
    cJSON *string_item = NULL;
    cJSON *number_item = NULL;
    double num;
    volatile double sink = 0.0;

    strbuf = (char *)malloc(Size + 1);
    if (strbuf == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(strbuf, Data, Size);
    }
    strbuf[Size] = '\0';

    string_item = cJSON_CreateString(strbuf);

    num = read_double_from_data(Data, Size);
    number_item = cJSON_CreateNumber(num);

    sink += cJSON_GetNumberValue(number_item);
    sink += cJSON_GetNumberValue(string_item);
    sink += cJSON_GetNumberValue((Size & 1) ? number_item : string_item);

    cJSON_Delete(number_item);
    cJSON_Delete(string_item);

    free(strbuf);
    (void)sink;
    return 0;
}