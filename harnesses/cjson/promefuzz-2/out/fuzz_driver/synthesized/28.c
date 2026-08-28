// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1993:26 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2450:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2455:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2309:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2316:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2450:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2455:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2309:23 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2316:23 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2450:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2455:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2581:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2548:23 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static char *make_cstring(const uint8_t *data, size_t len) {
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
            return cJSON_CreateBool((int)(selector & 1));
        case 2: {
            double num = 0.0;
            size_t i;
            for (i = 0; i < len; i++) {
                num = (num * 131.0) + (double)data[i];
            }
            return cJSON_CreateNumber(num);
        }
        case 3: {
            char *s = make_cstring(data, len);
            cJSON *item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        case 4:
            return cJSON_CreateArray();
        case 5:
            return cJSON_CreateObject();
        case 6: {
            char *s = make_cstring(data, len);
            cJSON *item = cJSON_CreateRaw(s ? s : "");
            free(s);
            return item;
        }
        default:
            return cJSON_CreateStringReference("");
    }
}

int LLVMFuzzerTestOneInput_28(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint8_t key1_len, key2_len, key3_len, val1_len, val2_len;
    uint8_t sel1, sel2, sel3;
    char *key1 = NULL, *key2 = NULL, *key3 = NULL;
    const uint8_t *val1, *val2, *rest;
    size_t rest_len;
    cJSON *root = NULL;
    cJSON *item1 = NULL, *item2 = NULL, *item3 = NULL;
    cJSON *replacement1 = NULL, *replacement2 = NULL, *replacement3 = NULL, *replacement4 = NULL;
    cJSON *detached1 = NULL, *detached2 = NULL;
    cJSON *tmp = NULL;
    FILE *fp;

    if (Data == NULL) {
        return 0;
    }

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    key1_len = (Size > off) ? Data[off++] : 0;
    key2_len = (Size > off) ? Data[off++] : 0;
    key3_len = (Size > off) ? Data[off++] : 0;
    val1_len = (Size > off) ? Data[off++] : 0;
    val2_len = (Size > off) ? Data[off++] : 0;
    sel1 = (Size > off) ? Data[off++] : 0;
    sel2 = (Size > off) ? Data[off++] : 0;
    sel3 = (Size > off) ? Data[off++] : 0;

    if (key1_len > Size - off) key1_len = (uint8_t)(Size - off);
    key1 = make_cstring(Data + off, key1_len);
    off += key1_len;

    if (key2_len > Size - off) key2_len = (uint8_t)(Size - off);
    key2 = make_cstring(Data + off, key2_len);
    off += key2_len;

    if (key3_len > Size - off) key3_len = (uint8_t)(Size - off);
    key3 = make_cstring(Data + off, key3_len);
    off += key3_len;

    if (val1_len > Size - off) val1_len = (uint8_t)(Size - off);
    val1 = Data + off;
    off += val1_len;

    if (val2_len > Size - off) val2_len = (uint8_t)(Size - off);
    val2 = Data + off;
    off += val2_len;

    rest = Data + off;
    rest_len = Size - off;

    if (key1 == NULL || key2 == NULL || key3 == NULL) {
        free(key1);
        free(key2);
        free(key3);
        return 0;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        free(key1);
        free(key2);
        free(key3);
        return 0;
    }

    item1 = make_item_from_selector(sel1, val1, val1_len);
    item2 = make_item_from_selector(sel2, val2, val2_len);
    item3 = make_item_from_selector(sel3, rest, rest_len);

    if (item1 != NULL) {
        if (!cJSON_AddItemToObject(root, key1, item1)) {
            cJSON_Delete(item1);
        }
    }
    if (item2 != NULL) {
        if (!cJSON_AddItemToObject(root, key2, item2)) {
            cJSON_Delete(item2);
        }
    }
    if (item3 != NULL) {
        if (!cJSON_AddItemToObject(root, key3, item3)) {
            cJSON_Delete(item3);
        }
    }

    (void)cJSON_HasObjectItem(root, key1);
    (void)cJSON_HasObjectItem(root, key2);
    (void)cJSON_HasObjectItem(root, key3);
    (void)cJSON_HasObjectItem(root, "");
    (void)cJSON_HasObjectItem(NULL, key1);

    replacement1 = make_item_from_selector((uint8_t)(sel1 + 1), rest, rest_len);
    if (replacement1 != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key1, replacement1)) {
            cJSON_Delete(replacement1);
        }
    }

    replacement2 = make_item_from_selector((uint8_t)(sel2 + 2), val1, val1_len);
    if (replacement2 != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key2, replacement2)) {
            cJSON_Delete(replacement2);
        }
    }

    detached1 = cJSON_DetachItemFromObject(root, key1);
    if (detached1 != NULL) {
        cJSON_Delete(detached1);
    }

    detached2 = cJSON_DetachItemFromObjectCaseSensitive(root, key2);
    if (detached2 != NULL) {
        cJSON_Delete(detached2);
    }

    cJSON_DeleteItemFromObject(root, key3);
    cJSON_DeleteItemFromObject(root, key1);
    cJSON_DeleteItemFromObject(root, "");

    replacement3 = make_item_from_selector((uint8_t)(sel3 + 3), val2, val2_len);
    if (replacement3 != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key3, replacement3)) {
            cJSON_Delete(replacement3);
        }
    }

    replacement4 = make_item_from_selector((uint8_t)(sel1 + 4), Data, Size);
    if (replacement4 != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key1, replacement4)) {
            cJSON_Delete(replacement4);
        }
    }

    (void)cJSON_DetachItemFromObject(NULL, key1);
    (void)cJSON_DetachItemFromObjectCaseSensitive(NULL, key2);
    cJSON_DeleteItemFromObject(NULL, key3);

    tmp = cJSON_CreateNull();
    if (tmp != NULL) {
        if (!cJSON_ReplaceItemInObject(NULL, key1, tmp)) {
            cJSON_Delete(tmp);
        }
    }

    tmp = cJSON_CreateNull();
    if (tmp != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(NULL, key2, tmp)) {
            cJSON_Delete(tmp);
        }
    }

    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(key3);
    return 0;
}