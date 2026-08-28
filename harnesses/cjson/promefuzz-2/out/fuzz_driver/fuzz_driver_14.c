// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2450:26 in cJSON.h
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

static double read_double_from_data(const uint8_t *Data, size_t Size, size_t Offset) {
    double value = 0.0;
    if (Offset + sizeof(double) <= Size) {
        memcpy(&value, Data + Offset, sizeof(double));
    } else {
        uint8_t buf[sizeof(double)] = {0};
        size_t remaining = (Offset < Size) ? (Size - Offset) : 0;
        if (remaining > 0) {
            memcpy(buf, Data + Offset, remaining);
        }
        memcpy(&value, buf, sizeof(double));
    }
    return value;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num1 = read_double_from_data(Data, Size, 0);
    double num2 = read_double_from_data(Data, Size, sizeof(double));

    const char *key_add = "key";
    const char *key_replace = "key";

    if (Size > 2 * sizeof(double)) {
        size_t key_offset = 2 * sizeof(double);
        size_t key_len = Size - key_offset;

        if (key_len > 0) {
            static char keybuf[256];
            size_t copy_len = key_len;
            if (copy_len >= sizeof(keybuf)) {
                copy_len = sizeof(keybuf) - 1;
            }
            memcpy(keybuf, Data + key_offset, copy_len);
            keybuf[copy_len] = '\0';
            key_add = keybuf;
            key_replace = keybuf;
        }
    }

    cJSON *object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    cJSON *item1 = cJSON_CreateNumber(num1);
    cJSON *item2 = cJSON_CreateNumber(num2);

    if (item1 != NULL) {
        (void)cJSON_AddItemToObject(object, key_add, item1);
    }

    if (item2 != NULL) {
        (void)cJSON_ReplaceItemInObject(object, key_replace, item2);
    }

    cJSON_Delete(object);
    return 0;
}