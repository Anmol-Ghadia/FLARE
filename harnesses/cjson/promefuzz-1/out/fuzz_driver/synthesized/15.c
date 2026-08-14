// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_malloc at cJSON.c:3155:22 in cJSON.h
// cJSON_malloc at cJSON.c:3155:22 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
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

int LLVMFuzzerTestOneInput_15(const uint8_t *Data, size_t Size) {
    char *strbuf = NULL;
    cJSON *string_item = NULL;
    cJSON *number_item = NULL;
    double num = 0.0;
    double v1, v2, v3;

    if (Size > 0) {
        strbuf = (char *)cJSON_malloc(Size + 1);
        if (strbuf == NULL) {
            return 0;
        }
        memcpy(strbuf, Data, Size);
        strbuf[Size] = '\0';
    } else {
        strbuf = (char *)cJSON_malloc(1);
        if (strbuf == NULL) {
            return 0;
        }
        strbuf[0] = '\0';
    }

    string_item = cJSON_CreateString(strbuf);

    if (Size >= sizeof(double)) {
        memcpy(&num, Data, sizeof(double));
    } else {
        for (size_t i = 0; i < Size; ++i) {
            num = num * 257.0 + (double)Data[i];
        }
    }

    number_item = cJSON_CreateNumber(num);

    v1 = cJSON_GetNumberValue(number_item);
    v2 = cJSON_GetNumberValue(string_item);
    v3 = cJSON_GetNumberValue((Size & 1) ? number_item : string_item);

    if (isnan(v1) || isnan(v2) || isnan(v3)) {
        /* intentionally empty */
    }

    cJSON_Delete(string_item);
    cJSON_Delete(number_item);

    cJSON_free(strbuf);
    return 0;
}