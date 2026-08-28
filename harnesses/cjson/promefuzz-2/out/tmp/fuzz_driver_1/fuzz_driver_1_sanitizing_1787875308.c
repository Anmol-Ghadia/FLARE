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

static char *make_mutable_string(const uint8_t *Data, size_t Size) {
    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return NULL;
    }
    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';
    return buf;
}

static char *make_key_from_data(const uint8_t *Data, size_t Size, size_t offset, size_t max_len) {
    size_t i, len;
    char *key;

    if (offset >= Size) {
        len = 0;
    } else {
        len = Size - offset;
        if (len > max_len) {
            len = max_len;
        }
    }

    key = (char *)malloc(len + 1);
    if (key == NULL) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        unsigned char c = Data[offset + i];
        if (c == '\0') {
            c = '_';
        }
        key[i] = (char)c;
    }
    key[len] = '\0';
    return key;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    cJSON *obj2 = cJSON_CreateObject();
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON *item5 = NULL;
    char *dynkey1 = NULL;
    char *dynkey2 = NULL;
    char *delkey1 = NULL;
    char *delkey2 = NULL;
    char *delkey3 = NULL;
    char *minify_buf = NULL;
    int idx = 0;

    if (root == NULL || array == NULL || obj2 == NULL) {
        cJSON_Delete(root);
        cJSON_Delete(array);
        cJSON_Delete(obj2);
        return 0;
    }

    dynkey1 = make_key_from_data(Data, Size, 0, 32);
    dynkey2 = make_key_from_data(Data, Size, 32, 32);
    delkey1 = make_key_from_data(Data, Size, 64, 32);
    delkey2 = make_key_from_data(Data, Size, 96, 32);
    delkey3 = make_key_from_data(Data, Size, 128, 32);
    minify_buf = make_mutable_string(Data, Size);

    item1 = cJSON_CreateString((const char *)(dynkey1 ? dynkey1 : ""));
    item2 = cJSON_CreateNumber((Size > 0) ? (double)Data[0] : 0.0);
    item3 = cJSON_CreateBool((Size > 1) ? (Data[1] & 1) : 0);
    item4 = cJSON_CreateNull();
    item5 = cJSON_CreateString((const char *)(dynkey2 ? dynkey2 : ""));

    if (item1 == NULL || item2 == NULL || item3 == NULL || item4 == NULL || item5 == NULL) {
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        cJSON_Delete(item3);
        cJSON_Delete(item4);
        cJSON_Delete(item5);
        free(dynkey1);
        free(dynkey2);
        free(delkey1);
        free(delkey2);
        free(delkey3);
        free(minify_buf);
        cJSON_Delete(root);
        cJSON_Delete(array);
        cJSON_Delete(obj2);
        return 0;
    }

    cJSON_AddItemToObject(root, dynkey1 ? dynkey1 : "", item1);
    cJSON_AddItemToObject(root, dynkey2 ? dynkey2 : "", item2);
    cJSON_AddItemToObjectCS(root, "const_key_1", item3);
    cJSON_AddItemToObjectCS(root, "const_key_2", item4);
    cJSON_AddItemToObjectCS(root, "const_key_3", item5);

    cJSON_AddItemReferenceToArray(array, item1);
    cJSON_AddItemReferenceToArray(array, item3);

    cJSON_AddItemReferenceToObject(obj2, "ref_key_1", item2);
    cJSON_AddItemReferenceToObject(obj2, "ref_key_2", item4);
    cJSON_AddItemReferenceToObject(obj2, dynkey1 ? dynkey1 : "", item5);

    if (Size > 2) {
        idx = (int)(Data[2] % 8);
    } else {
        idx = 0;
    }
    cJSON_DeleteItemFromArray(array, idx);

    cJSON_DeleteItemFromObject(root, delkey1 ? delkey1 : "");
    cJSON_DeleteItemFromObject(root, "const_key_2");
    cJSON_DeleteItemFromObject(obj2, delkey2 ? delkey2 : "");
    cJSON_DeleteItemFromObjectCaseSensitive(root, "const_key_3");
    cJSON_DeleteItemFromObjectCaseSensitive(obj2, delkey3 ? delkey3 : "");

    cJSON_Minify(minify_buf);

    free(dynkey1);
    free(dynkey2);
    free(delkey1);
    free(delkey2);
    free(delkey3);
    free(minify_buf);

    cJSON_Delete(obj2);
    cJSON_Delete(array);
    cJSON_Delete(root);

    return 0;
}