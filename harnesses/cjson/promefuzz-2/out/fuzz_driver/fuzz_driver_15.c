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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *strbuf = NULL;
    double num = 0.0;
    cJSON *str_item = NULL;
    cJSON *num_item = NULL;
    volatile double v1, v2, v3;

    if (Size > 0) {
        strbuf = (char *)malloc(Size + 1);
        if (strbuf == NULL) {
            return 0;
        }
        memcpy(strbuf, Data, Size);
        strbuf[Size] = '\0';
    } else {
        strbuf = (char *)malloc(1);
        if (strbuf == NULL) {
            return 0;
        }
        strbuf[0] = '\0';
    }

    str_item = cJSON_CreateString(strbuf);

    if (Size >= sizeof(double)) {
        memcpy(&num, Data, sizeof(double));
    } else {
        for (size_t i = 0; i < Size; ++i) {
            num = num * 257.0 + (double)Data[i];
        }
    }

    num_item = cJSON_CreateNumber(num);

    v1 = cJSON_GetNumberValue(num_item);
    v2 = cJSON_GetNumberValue(str_item);
    v3 = cJSON_GetNumberValue((Size & 1) ? num_item : NULL);

    (void)v1;
    (void)v2;
    (void)v3;

    cJSON_Delete(str_item);
    cJSON_Delete(num_item);

    free(strbuf);
    return 0;
}