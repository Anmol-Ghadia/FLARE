// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2455:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2450:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2316:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2472:23 in cJSON.h
// cJSON_CreateFalse at cJSON.c:2483:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2581:23 in cJSON.h
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

static char *dup_bytes_as_cstring(const uint8_t *data, size_t len) {
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(s, data, len);
    }
    s[len] = '\0';
    return s;
}

static cJSON *make_item_from_selector(uint8_t selector, const uint8_t *data, size_t len) {
    switch (selector % 8) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateTrue();
        case 2:
            return cJSON_CreateFalse();
        case 3: {
            double num = 0.0;
            size_t i;
            for (i = 0; i < len; ++i) {
                num = (num * 131.0) + data[i];
            }
            return cJSON_CreateNumber(num);
        }
        case 4: {
            char *s = dup_bytes_as_cstring(data, len);
            cJSON *item;
            if (s == NULL) {
                return cJSON_CreateString("");
            }
            item = cJSON_CreateString(s);
            free(s);
            return item;
        }
        case 5: {
            cJSON *arr = cJSON_CreateArray();
            if (arr == NULL) {
                return NULL;
            }
            if (len > 0) {
                cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)data[0]));
            }
            if (len > 1) {
                cJSON_AddItemToArray(arr, cJSON_CreateBool((int)(data[1] & 1)));
            }
            return arr;
        }
        case 6: {
            cJSON *obj = cJSON_CreateObject();
            if (obj == NULL) {
                return NULL;
            }
            cJSON_AddItemToObject(obj, "nested", cJSON_CreateNumber((double)len));
            return obj;
        }
        default:
            return cJSON_CreateRaw("null");
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t quarter, k1_len, k2_len, json_len;
    char *key1 = NULL, *key2 = NULL, *json_buf = NULL;
    cJSON *root = NULL, *detached = NULL, *replacement1 = NULL, *replacement2 = NULL, *tmp = NULL;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    quarter = Size / 4;
    k1_len = quarter;
    k2_len = quarter;
    json_len = Size - k1_len - k2_len;

    key1 = dup_bytes_as_cstring(Data, k1_len);
    key2 = dup_bytes_as_cstring(Data + k1_len, k2_len);
    json_buf = dup_bytes_as_cstring(Data + k1_len + k2_len, json_len);

    if (key1 == NULL || key2 == NULL || json_buf == NULL) {
        free(key1);
        free(key2);
        free(json_buf);
        return 0;
    }

    root = cJSON_Parse(json_buf);
    if (root == NULL || !cJSON_IsObject(root)) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        root = cJSON_CreateObject();
        if (root == NULL) {
            free(key1);
            free(key2);
            free(json_buf);
            return 0;
        }

        cJSON_AddItemToObject(root, key1, cJSON_CreateString(key2));
        cJSON_AddItemToObject(root, key2, cJSON_CreateNumber((double)json_len));
        cJSON_AddItemToObject(root, "fixed", cJSON_CreateBool((int)(Size & 1)));
    }

    (void)cJSON_HasObjectItem(root, key1);
    (void)cJSON_HasObjectItem(root, key2);
    (void)cJSON_HasObjectItem(root, "");
    (void)cJSON_GetObjectItemCaseSensitive(root, key1);
    (void)cJSON_GetObjectItemCaseSensitive(root, key2);
    (void)cJSON_GetObjectItemCaseSensitive(root, "");

    replacement1 = make_item_from_selector(Size > 0 ? Data[0] : 0, Data, Size);
    if (replacement1 != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key1, replacement1)) {
            cJSON_Delete(replacement1);
        }
    }

    replacement2 = make_item_from_selector(Size > 1 ? Data[1] : 1, Data + (Size > 2 ? 2 : 0), Size > 2 ? Size - 2 : 0);
    if (replacement2 != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key2, replacement2)) {
            cJSON_Delete(replacement2);
        }
    }

    tmp = cJSON_GetObjectItemCaseSensitive(root, key1);
    if (tmp != NULL) {
        (void)cJSON_HasObjectItem(root, key1);
    }

    detached = cJSON_DetachItemFromObjectCaseSensitive(root, key1);
    if (detached != NULL) {
        cJSON_Delete(detached);
        detached = NULL;
    }

    cJSON_DeleteItemFromObjectCaseSensitive(root, key2);
    cJSON_DeleteItemFromObjectCaseSensitive(root, "");
    (void)cJSON_GetObjectItemCaseSensitive(root, key1);
    (void)cJSON_HasObjectItem(root, key2);

    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(json_buf);
    return 0;
}