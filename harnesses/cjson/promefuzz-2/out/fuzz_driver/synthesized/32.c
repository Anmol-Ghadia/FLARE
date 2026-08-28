// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_IsObject at cJSON.c:3010:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2413:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
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

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; i++) {
        v <<= 8;
        if (*offset < size) {
            v |= data[*offset];
            (*offset)++;
        }
    }
    return v;
}

static char *make_string_from_bytes(const uint8_t *data, size_t size) {
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

static cJSON *make_item_from_selector(uint8_t selector, const uint8_t *data, size_t size) {
    switch (selector % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((int)(selector & 1));
        case 2: {
            double num = 0.0;
            size_t i;
            for (i = 0; i < size; i++) {
                num = (num * 131.0) + (double)data[i];
            }
            return cJSON_CreateNumber(num);
        }
        case 3: {
            char *s = make_string_from_bytes(data, size);
            cJSON *item;
            if (s == NULL) {
                return cJSON_CreateString("");
            }
            item = cJSON_CreateString(s);
            free(s);
            return item;
        }
        case 4:
            return cJSON_CreateArray();
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput_32(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *root = NULL;
    cJSON *parsed = NULL;
    char *json_text = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *dummy_buf = NULL;
    FILE *fp = NULL;

    if (Data == NULL) {
        return 0;
    }

    json_text = make_string_from_bytes(Data, Size);
    if (json_text == NULL) {
        return 0;
    }

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    parsed = cJSON_Parse(json_text);
    if (parsed != NULL && cJSON_IsObject(parsed)) {
        root = parsed;
        parsed = NULL;
    } else {
        if (parsed != NULL) {
            cJSON_Delete(parsed);
            parsed = NULL;
        }
        root = cJSON_CreateObject();
        if (root == NULL) {
            free(json_text);
            return 0;
        }
    }

    {
        uint32_t k1_len = read_u32(Data, Size, &offset) % 32;
        uint32_t k2_len = read_u32(Data, Size, &offset) % 32;
        uint32_t k3_len = read_u32(Data, Size, &offset) % 32;

        if (offset + k1_len > Size) {
            k1_len = (uint32_t)((offset < Size) ? (Size - offset) : 0);
        }
        key1 = make_string_from_bytes(Data + offset, k1_len);
        offset += k1_len;

        if (offset + k2_len > Size) {
            k2_len = (uint32_t)((offset < Size) ? (Size - offset) : 0);
        }
        key2 = make_string_from_bytes(Data + offset, k2_len);
        offset += k2_len;

        if (offset + k3_len > Size) {
            k3_len = (uint32_t)((offset < Size) ? (Size - offset) : 0);
        }
        key3 = make_string_from_bytes(Data + offset, k3_len);
        offset += k3_len;
    }

    if (key1 == NULL) key1 = make_string_from_bytes((const uint8_t *)"", 0);
    if (key2 == NULL) key2 = make_string_from_bytes((const uint8_t *)"", 0);
    if (key3 == NULL) key3 = make_string_from_bytes((const uint8_t *)"", 0);

    if (key1 == NULL || key2 == NULL || key3 == NULL) {
        cJSON_Delete(root);
        free(json_text);
        free(key1);
        free(key2);
        free(key3);
        return 0;
    }

    {
        cJSON *item1 = make_item_from_selector((offset < Size) ? Data[offset++] : 0, Data + offset, (offset < Size) ? (Size - offset) : 0);
        cJSON *item2 = make_item_from_selector((offset < Size) ? Data[offset++] : 1, Data + offset, (offset < Size) ? (Size - offset) : 0);
        cJSON *item3 = make_item_from_selector((offset < Size) ? Data[offset++] : 2, Data + offset, (offset < Size) ? (Size - offset) : 0);

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
    }

    (void)cJSON_GetObjectItem(root, key1);
    (void)cJSON_GetObjectItem(root, key2);
    (void)cJSON_GetObjectItem(root, key3);
    (void)cJSON_HasObjectItem(root, key1);
    (void)cJSON_HasObjectItem(root, key2);
    (void)cJSON_HasObjectItem(root, key3);

    {
        cJSON *replacement1 = make_item_from_selector((offset < Size) ? Data[offset++] : 3, Data + offset, (offset < Size) ? (Size - offset) : 0);
        cJSON *replacement2 = make_item_from_selector((offset < Size) ? Data[offset++] : 4, Data + offset, (offset < Size) ? (Size - offset) : 0);

        if (replacement1 != NULL) {
            if (!cJSON_ReplaceItemInObject(root, key1, replacement1)) {
                cJSON_Delete(replacement1);
            }
        }
        if (replacement2 != NULL) {
            if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key2, replacement2)) {
                cJSON_Delete(replacement2);
            }
        }
    }

    (void)cJSON_GetObjectItem(root, key1);
    (void)cJSON_GetObjectItem(root, key2);
    (void)cJSON_HasObjectItem(root, key1);
    (void)cJSON_HasObjectItem(root, key2);

    cJSON_DeleteItemFromObject(root, key1);
    cJSON_DeleteItemFromObject(root, key2);
    cJSON_DeleteItemFromObject(root, key3);
    cJSON_DeleteItemFromObject(root, "nonexistent");

    (void)cJSON_GetObjectItem(root, key1);
    (void)cJSON_HasObjectItem(root, key2);

    dummy_buf = cJSON_PrintUnformatted(root);
    if (dummy_buf != NULL) {
        free(dummy_buf);
        dummy_buf = NULL;
    }

    cJSON_Delete(root);
    free(json_text);
    free(key1);
    free(key2);
    free(key3);
    return 0;
}