// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2274:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2274:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2274:23 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cJSON.h"

static char *make_cstring(const uint8_t *data, size_t size) {
    char *out = (char *)malloc(size + 1);
    if (out == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(out, data, size);
    }
    out[size] = '\0';
    return out;
}

static char *make_key_from_bytes(const uint8_t *data, size_t size) {
    size_t key_len = size > 32 ? 32 : size;
    char *key = (char *)malloc(key_len + 1);
    size_t i;

    if (key == NULL) {
        return NULL;
    }

    for (i = 0; i < key_len; i++) {
        unsigned char c = data[i];
        if (c == '\0') {
            key[i] = 'A';
        } else {
            key[i] = (char)c;
        }
    }
    key[key_len] = '\0';
    return key;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *json_text = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *detached = NULL;
    char *strval;

    if (Data == NULL) {
        return 0;
    }

    json_text = make_cstring(Data, Size);
    if (json_text == NULL) {
        return 0;
    }

    key1 = make_key_from_bytes(Data, Size);
    if (key1 == NULL) {
        free(json_text);
        return 0;
    }

    if (Size > 1) {
        key2 = make_key_from_bytes(Data + 1, Size - 1);
    } else {
        key2 = make_key_from_bytes((const uint8_t *)"", 0);
    }
    if (key2 == NULL) {
        free(key1);
        free(json_text);
        return 0;
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0) {
                fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }
    }

    root = cJSON_Parse(json_text);

    cJSON_HasObjectItem(NULL, key1);
    cJSON_GetObjectItem(NULL, key1);
    cJSON_GetObjectItemCaseSensitive(NULL, key1);
    cJSON_GetStringValue(NULL);
    cJSON_DetachItemFromObjectCaseSensitive(NULL, key1);

    if (root != NULL) {
        cJSON_HasObjectItem(root, key1);
        cJSON_HasObjectItem(root, key2);

        item = cJSON_GetObjectItem(root, key1);
        if (item != NULL) {
            strval = cJSON_GetStringValue(item);
            (void)strval;
        }

        item = cJSON_GetObjectItemCaseSensitive(root, key1);
        if (item != NULL) {
            strval = cJSON_GetStringValue(item);
            (void)strval;
        }

        item = cJSON_GetObjectItem(root, key2);
        if (item != NULL) {
            strval = cJSON_GetStringValue(item);
            (void)strval;
        }

        item = cJSON_GetObjectItemCaseSensitive(root, key2);
        if (item != NULL) {
            strval = cJSON_GetStringValue(item);
            (void)strval;
        }

        detached = cJSON_DetachItemFromObjectCaseSensitive(root, key1);
        if (detached != NULL) {
            cJSON_GetStringValue(detached);
            cJSON_Delete(detached);
            detached = NULL;
        }

        detached = cJSON_DetachItemFromObjectCaseSensitive(root, key2);
        if (detached != NULL) {
            cJSON_GetStringValue(detached);
            cJSON_Delete(detached);
            detached = NULL;
        }

        cJSON_Delete(root);
    }

    free(key2);
    free(key1);
    free(json_text);
    return 0;
}