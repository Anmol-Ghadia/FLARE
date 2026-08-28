#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';

    cJSON *item_false = cJSON_CreateFalse();
    cJSON *item_string = cJSON_CreateString(buf);
    cJSON *item_object = cJSON_CreateObject();
    cJSON *item_string_ref = cJSON_CreateStringReference(buf);

    cJSON_Delete(item_false);

    cJSON *item_object_ref = cJSON_CreateObjectReference(item_object);

    cJSON_Delete(item_string);
    cJSON_Delete(item_object_ref);
    cJSON_Delete(item_object);
    cJSON_Delete(item_string_ref);

    free(buf);
    return 0;
}