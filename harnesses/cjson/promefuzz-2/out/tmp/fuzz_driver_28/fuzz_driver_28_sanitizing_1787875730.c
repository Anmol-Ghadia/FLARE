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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == NULL) {
        return 0;
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    size_t off = 0;
    uint8_t key1_len = (Size > off) ? Data[off++] : 0;
    uint8_t key2_len = (Size > off) ? Data[off++] : 0;
    uint8_t key3_len = (Size > off) ? Data[off++] : 0;
    uint8_t val1_len = (Size > off) ? Data[off++] : 0;
    uint8_t val2_len = (Size > off) ? Data[off++] : 0;
    uint8_t sel1 = (Size > off) ? Data[off++] : 0;
    uint8_t sel2 = (Size > off) ? Data[off++] : 0;
    uint8_t sel3 = (Size > off) ? Data[off++] : 0;

    if (key1_len > Size - off) key1_len = (uint8_t)(Size - off);
    char *key1 = make_cstring(Data + off, key1_len);
    off += key1_len;

    if (key2_len > Size - off) key2_len = (uint8_t)(Size - off);
    char *key2 = make_cstring(Data + off, key2_len);
    off += key2_len;

    if (key3_len > Size - off) key3_len = (uint8_t)(Size - off);
    char *key3 = make_cstring(Data + off, key3_len);
    off += key3_len;

    if (val1_len > Size - off) val1_len = (uint8_t)(Size - off);
    const uint8_t *val1 = Data + off;
    off += val1_len;

    if (val2_len > Size - off) val2_len = (uint8_t)(Size - off);
    const uint8_t *val2 = Data + off;
    off += val2_len;

    const uint8_t *rest = Data + off;
    size_t rest_len = Size - off;

    if (key1 == NULL || key2 == NULL || key3 == NULL) {
        free(key1);
        free(key2);
        free(key3);
        return 0;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        free(key1);
        free(key2);
        free(key3);
        return 0;
    }

    cJSON *item1 = make_item_from_selector(sel1, val1, val1_len);
    cJSON *item2 = make_item_from_selector(sel2, val2, val2_len);
    cJSON *item3 = make_item_from_selector(sel3, rest, rest_len);

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

    cJSON *replacement1 = make_item_from_selector((uint8_t)(sel1 + 1), rest, rest_len);
    if (replacement1 != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key1, replacement1)) {
            cJSON_Delete(replacement1);
        }
    }

    cJSON *replacement2 = make_item_from_selector((uint8_t)(sel2 + 2), val1, val1_len);
    if (replacement2 != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key2, replacement2)) {
            cJSON_Delete(replacement2);
        }
    }

    cJSON *detached1 = cJSON_DetachItemFromObject(root, key1);
    if (detached1 != NULL) {
        cJSON_Delete(detached1);
    }

    cJSON *detached2 = cJSON_DetachItemFromObjectCaseSensitive(root, key2);
    if (detached2 != NULL) {
        cJSON_Delete(detached2);
    }

    cJSON_DeleteItemFromObject(root, key3);
    cJSON_DeleteItemFromObject(root, key1);
    cJSON_DeleteItemFromObject(root, "");

    cJSON *replacement3 = make_item_from_selector((uint8_t)(sel3 + 3), val2, val2_len);
    if (replacement3 != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key3, replacement3)) {
            cJSON_Delete(replacement3);
        }
    }

    cJSON *replacement4 = make_item_from_selector((uint8_t)(sel1 + 4), Data, Size);
    if (replacement4 != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key1, replacement4)) {
            cJSON_Delete(replacement4);
        }
    }

    (void)cJSON_DetachItemFromObject(NULL, key1);
    (void)cJSON_DetachItemFromObjectCaseSensitive(NULL, key2);
    cJSON_DeleteItemFromObject(NULL, key3);
    (void)cJSON_ReplaceItemInObject(NULL, key1, cJSON_CreateNull());
    (void)cJSON_ReplaceItemInObjectCaseSensitive(NULL, key2, cJSON_CreateNull());

    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(key3);
    return 0;
}