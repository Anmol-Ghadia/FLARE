// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput_15(const uint8_t *Data, size_t Size) {
    char *strbuf = (char *)malloc(Size + 1);
    if (strbuf == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(strbuf, Data, Size);
    }
    strbuf[Size] = '\0';

    double num = 0.0;
    size_t copy_size = Size < sizeof(num) ? Size : sizeof(num);
    if (copy_size > 0) {
        memcpy(&num, Data, copy_size);
    }

    cJSON *string_item = cJSON_CreateString(strbuf);
    cJSON *number_item = cJSON_CreateNumber(num);

    (void)cJSON_GetNumberValue(number_item);
    (void)cJSON_GetNumberValue(string_item);
    (void)cJSON_GetNumberValue(NULL);

    cJSON_Delete(number_item);
    cJSON_Delete(string_item);

    free(strbuf);
    return 0;
}