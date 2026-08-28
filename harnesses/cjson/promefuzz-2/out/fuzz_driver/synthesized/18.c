// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
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
#include "cJSON.h"

static char *make_cstring(const uint8_t *data, size_t size) {
    char *s = (char *)malloc(size + 1);
    if (s == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(s, data, size);
    }
    s[size] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput_18(const uint8_t *Data, size_t Size) {
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) {
        return 0;
    }

    if (Size == 0) {
        cJSON_AddRawToObject(obj, "", "");
        cJSON_Delete(obj);
        return 0;
    }

    size_t key_len = Data[0] % (Size + 1);
    if (key_len > Size - 1) {
        key_len = Size - 1;
    }

    const uint8_t *key_data = Data + 1;
    size_t remaining = Size - 1;
    if (key_len > remaining) {
        key_len = remaining;
    }

    const uint8_t *raw_data = key_data + key_len;
    size_t raw_len = remaining - key_len;

    char *key = make_cstring(key_data, key_len);
    char *raw = make_cstring(raw_data, raw_len);

    if (key != NULL && raw != NULL) {
        (void)cJSON_AddRawToObject(obj, key, raw);

        if (raw_len > 0) {
            size_t half = raw_len / 2;
            char *raw2 = make_cstring(raw_data, half);
            if (raw2 != NULL) {
                (void)cJSON_AddRawToObject(obj, key, raw2);
                free(raw2);
            }
        }

        if (key_len > 0) {
            size_t half_key = key_len / 2;
            char *key2 = make_cstring(key_data, half_key);
            if (key2 != NULL) {
                (void)cJSON_AddRawToObject(obj, key2, raw);
                free(key2);
            }
        }
    }

    free(key);
    free(raw);
    cJSON_Delete(obj);
    return 0;
}