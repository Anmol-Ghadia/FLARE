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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *strbuf = NULL;
    cJSON *string_item = NULL;
    cJSON *number_item = NULL;
    double num = 0.0;

    strbuf = (char *)cJSON_malloc(Size + 1);
    if (strbuf == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(strbuf, Data, Size);
    }
    strbuf[Size] = '\0';

    string_item = cJSON_CreateString(strbuf);

    if (Size >= sizeof(double)) {
        memcpy(&num, Data, sizeof(double));
    } else {
        for (size_t i = 0; i < Size; ++i) {
            num = (num * 256.0) + (double)Data[i];
        }
    }

    number_item = cJSON_CreateNumber(num);

    (void)cJSON_GetNumberValue(number_item);
    (void)cJSON_GetNumberValue(string_item);
    (void)cJSON_GetNumberValue(number_item);

    cJSON_Delete(string_item);
    cJSON_Delete(number_item);

    cJSON_free(strbuf);
    return 0;
}