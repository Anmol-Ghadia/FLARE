// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

static double read_double_from_bytes(const uint8_t *data, size_t size, size_t offset)
{
    union {
        uint64_t u64;
        double d;
    } conv;
    uint8_t buf[8] = {0};
    size_t i;

    for (i = 0; i < 8; i++) {
        if (offset + i < size) {
            buf[i] = data[offset + i];
        }
    }

    conv.u64 = ((uint64_t)buf[0]) |
               ((uint64_t)buf[1] << 8) |
               ((uint64_t)buf[2] << 16) |
               ((uint64_t)buf[3] << 24) |
               ((uint64_t)buf[4] << 32) |
               ((uint64_t)buf[5] << 40) |
               ((uint64_t)buf[6] << 48) |
               ((uint64_t)buf[7] << 56);
    return conv.d;
}

static size_t bounded_strlen_from_data(const uint8_t *data, size_t size, size_t offset, size_t max_len)
{
    size_t i = 0;
    while ((offset + i) < size && i < max_len && data[offset + i] != '\0') {
        i++;
    }
    return i;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size)
{
    cJSON *object = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    char key1[64];
    char key2[64];
    size_t key1_len, key2_len;
    double num1, num2;

    memset(key1, 0, sizeof(key1));
    memset(key2, 0, sizeof(key2));

    num1 = read_double_from_bytes(Data, Size, 0);
    num2 = read_double_from_bytes(Data, Size, 8);

    key1_len = bounded_strlen_from_data(Data, Size, 16, sizeof(key1) - 1);
    if (key1_len > 0) {
        memcpy(key1, Data + 16, key1_len);
    } else {
        memcpy(key1, "k", 2);
    }

    key2_len = bounded_strlen_from_data(Data, Size, 16 + key1_len + 1, sizeof(key2) - 1);
    if (key2_len > 0 && (16 + key1_len + 1) < Size) {
        memcpy(key2, Data + 16 + key1_len + 1, key2_len);
    } else {
        memcpy(key2, key1, strlen(key1) + 1);
    }

    object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    item1 = cJSON_CreateNumber(num1);
    item2 = cJSON_CreateNumber(num2);

    if (item1 == NULL || item2 == NULL) {
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        cJSON_Delete(object);
        return 0;
    }

    if (!cJSON_AddItemToObject(object, key1, item1)) {
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        cJSON_Delete(object);
        return 0;
    }

    if (!cJSON_ReplaceItemInObject(object, key2, item2)) {
        cJSON_Delete(item2);
    }

    cJSON_Delete(object);
    return 0;
}