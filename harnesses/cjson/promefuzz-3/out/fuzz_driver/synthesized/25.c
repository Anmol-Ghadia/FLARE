// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_HasObjectItem at cJSON.c:1951:26 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2413:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2098:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2098:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2083:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2083:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_ReplaceItemInObjectCaseSensitive at cJSON.c:2413:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2506:23 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2539:23 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2539:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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
#include <stdio.h>

#include "cJSON.h"

static char *make_cstring(const uint8_t *data, size_t size, size_t *consumed)
{
    if (size == 0) {
        char *s = (char *)malloc(1);
        if (s != NULL) {
            s[0] = '\0';
        }
        if (consumed != NULL) {
            *consumed = 0;
        }
        return s;
    }

    size_t len = data[0];
    size_t available = (size > 1) ? (size - 1) : 0;
    if (len > available) {
        len = available;
    }

    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        if (consumed != NULL) {
            *consumed = 1 + len;
        }
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        unsigned char c = data[1 + i];
        if (c == '\0') {
            c = 'A';
        }
        s[i] = (char)c;
    }
    s[len] = '\0';

    if (consumed != NULL) {
        *consumed = 1 + len;
    }
    return s;
}

static cJSON *make_item_from_byte(uint8_t tag, const char *text, const uint8_t *data, size_t size)
{
    switch (tag % 8) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((tag & 1) != 0);
        case 2:
            return cJSON_CreateNumber((double)tag);
        case 3:
            return cJSON_CreateString(text != NULL ? text : "");
        case 4:
            return cJSON_CreateArray();
        case 5:
            return cJSON_CreateObject();
        case 6:
            if (size > 0) {
                return cJSON_CreateStringReference(text != NULL ? text : "");
            }
            return cJSON_CreateRaw(text != NULL ? text : "");
        default:
            return cJSON_CreateRaw(text != NULL ? text : "");
    }
}

int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size)
{
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

    cJSON *root = cJSON_CreateObject();
    cJSON *aux = cJSON_CreateObject();
    if (root == NULL || aux == NULL) {
        cJSON_Delete(root);
        cJSON_Delete(aux);
        return 0;
    }

    size_t off1 = 0, off2 = 0, off3 = 0, off4 = 0;
    char *key1 = make_cstring(Data, Size, &off1);
    char *key2 = make_cstring((off1 < Size) ? Data + off1 : Data, (off1 < Size) ? (Size - off1) : 0, &off2);
    char *val1 = make_cstring((off1 + off2 < Size) ? Data + off1 + off2 : Data,
                              (off1 + off2 < Size) ? (Size - off1 - off2) : 0, &off3);
    char *val2 = make_cstring((off1 + off2 + off3 < Size) ? Data + off1 + off2 + off3 : Data,
                              (off1 + off2 + off3 < Size) ? (Size - off1 - off2 - off3) : 0, &off4);

    if (key1 == NULL || key2 == NULL || val1 == NULL || val2 == NULL) {
        free(key1);
        free(key2);
        free(val1);
        free(val2);
        cJSON_Delete(root);
        cJSON_Delete(aux);
        return 0;
    }

    uint8_t b0 = (Size > 0) ? Data[0] : 0;
    uint8_t b1 = (Size > 1) ? Data[1] : 1;
    uint8_t b2 = (Size > 2) ? Data[2] : 2;
    uint8_t b3 = (Size > 3) ? Data[3] : 3;

    cJSON *item1 = make_item_from_byte(b0, val1, Data, Size);
    cJSON *item2 = make_item_from_byte(b1, val2, Data, Size);
    cJSON *item3 = make_item_from_byte(b2, val1, Data, Size);
    cJSON *item4 = make_item_from_byte(b3, val2, Data, Size);

    if (item1 != NULL) {
        cJSON_AddItemToObject(root, key1, item1);
        item1 = NULL;
    }
    if (item2 != NULL) {
        cJSON_AddItemToObject(root, key2, item2);
        item2 = NULL;
    }

    (void)cJSON_HasObjectItem(root, key1);
    (void)cJSON_HasObjectItem(root, key2);
    (void)cJSON_HasObjectItem(root, val1);
    (void)cJSON_HasObjectItem(NULL, key1);
    (void)cJSON_HasObjectItem(root, NULL);

    if (item3 != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key1, item3)) {
            cJSON_Delete(item3);
        }
        item3 = NULL;
    }

    if (item4 != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key2, item4)) {
            cJSON_Delete(item4);
        }
        item4 = NULL;
    }

    cJSON *shared = cJSON_CreateString(val1);
    if (shared != NULL) {
        (void)cJSON_AddItemReferenceToObject(root, val2, shared);
        (void)cJSON_AddItemReferenceToObject(aux, key1, shared);
        cJSON_Delete(shared);
    }

    cJSON *const_item = cJSON_CreateNumber((double)(Size));
    if (const_item != NULL) {
        if (!cJSON_AddItemToObjectCS(root, "CONST_KEY", const_item)) {
            cJSON_Delete(const_item);
        }
    }

    cJSON *const_item2 = cJSON_CreateString(val2);
    if (const_item2 != NULL) {
        if (!cJSON_AddItemToObjectCS(root, key1, const_item2)) {
            cJSON_Delete(const_item2);
        }
    }

    cJSON_DeleteItemFromObject(root, key1);
    cJSON_DeleteItemFromObject(root, key2);
    cJSON_DeleteItemFromObject(root, val1);
    cJSON_DeleteItemFromObject(root, "CONST_KEY");
    cJSON_DeleteItemFromObject(root, NULL);

    cJSON *post = cJSON_CreateString(val2);
    if (post != NULL) {
        if (!cJSON_ReplaceItemInObject(root, key1, post)) {
            cJSON_Delete(post);
        }
    }

    cJSON *post_cs = cJSON_CreateBool(1);
    if (post_cs != NULL) {
        if (!cJSON_ReplaceItemInObjectCaseSensitive(root, key2, post_cs)) {
            cJSON_Delete(post_cs);
        }
    }

    cJSON_Delete(root);
    cJSON_Delete(aux);
    free(key1);
    free(key2);
    free(val1);
    free(val2);
    return 0;
}