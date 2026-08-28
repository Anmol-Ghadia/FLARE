// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2186:22 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2186:22 in cJSON.h
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

static char *make_fuzz_string(const uint8_t *data, size_t size) {
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t split;
    char *key1 = NULL;
    char *key2 = NULL;
    cJSON *root = NULL;
    cJSON_bool b1, b2;

    if (Data == NULL) {
        return 0;
    }

    split = (Size > 0) ? (size_t)(Data[0] % (Size + 1)) : 0;

    key1 = make_fuzz_string(Data + (Size > 0 ? 1 : 0), (Size > 0 && split > 0) ? (split - 1) : 0);
    key2 = make_fuzz_string(
        Data + (Size > 0 ? 1 + ((split > 0) ? (split - 1) : 0) : 0),
        (Size > 0) ? (Size - 1 - ((split > 0) ? (split - 1) : 0)) : 0
    );

    if (key1 == NULL || key2 == NULL) {
        free(key1);
        free(key2);
        return 0;
    }

    b1 = (Size > 1) ? (cJSON_bool)(Data[1] & 1) : 0;
    b2 = (Size > 2) ? (cJSON_bool)(Data[2] & 1) : 1;

    root = cJSON_CreateObject();
    if (root != NULL) {
        (void)cJSON_AddBoolToObject(root, key1, b1);
        (void)cJSON_AddBoolToObject(root, key2, b2);
        cJSON_Delete(root);
    }

    free(key1);
    free(key2);
    return 0;
}