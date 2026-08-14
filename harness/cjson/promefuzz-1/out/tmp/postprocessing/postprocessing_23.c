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

static uint32_t read_u32(const uint8_t **data, size_t *size)
{
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size)
{
    return (int)read_u32(data, size);
}

static cJSON *make_item_from_bytes(const uint8_t **data, size_t *size)
{
    if (*size == 0) {
        return cJSON_CreateNull();
    }

    uint8_t kind = *(*data)++;
    (*size)--;

    switch (kind % 8) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((int)(kind & 1));
        case 2:
            return cJSON_CreateNumber((double)read_int(data, size));
        case 3: {
            size_t len = (*size > 32) ? 32 : *size;
            char *buf = (char *)malloc(len + 1);
            if (buf == NULL) {
                return cJSON_CreateNull();
            }
            memcpy(buf, *data, len);
            buf[len] = '\0';
            *data += len;
            *size -= len;
            cJSON *item = cJSON_CreateString(buf);
            free(buf);
            return item ? item : cJSON_CreateNull();
        }
        case 4: {
            cJSON *arr = cJSON_CreateArray();
            if (arr == NULL) {
                return cJSON_CreateNull();
            }
            int count = (int)((*size > 0) ? ((*data)[0] % 4) : 0);
            if (*size > 0) {
                (*data)++;
                (*size)--;
            }
            for (int i = 0; i < count; ++i) {
                cJSON *child = make_item_from_bytes(data, size);
                if (child != NULL) {
                    cJSON_AddItemToArray(arr, child);
                }
            }
            return arr;
        }
        case 5: {
            cJSON *obj = cJSON_CreateObject();
            if (obj == NULL) {
                return cJSON_CreateNull();
            }
            int count = (int)((*size > 0) ? ((*data)[0] % 4) : 0);
            if (*size > 0) {
                (*data)++;
                (*size)--;
            }
            for (int i = 0; i < count; ++i) {
                size_t klen = (*size > 8) ? 8 : *size;
                char key[9];
                memcpy(key, *data, klen);
                key[klen] = '\0';
                *data += klen;
                *size -= klen;

                cJSON *child = make_item_from_bytes(data, size);
                if (child != NULL) {
                    cJSON_AddItemToObject(obj, key, child);
                }
            }
            return obj;
        }
        case 6:
            return cJSON_CreateArray();
        case 7:
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    char *input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }
    memcpy(input, Data, Size);
    input[Size] = '\0';

    cJSON *root1 = cJSON_ParseWithLength(input, Size);
    cJSON *root2 = cJSON_ParseWithLength(input, Size);

    if (root1 == NULL) {
        root1 = make_item_from_bytes(&ptr, &remaining);
    }
    if (root2 == NULL) {
        root2 = make_item_from_bytes(&ptr, &remaining);
    }

    (void)cJSON_GetArraySize(NULL);
    (void)cJSON_GetArraySize(root1);
    (void)cJSON_GetArraySize(root2);

    cJSON *obj = cJSON_IsObject(root1) ? root1 : cJSON_CreateObject();
    cJSON *owned_obj = NULL;
    if (obj != root1) {
        owned_obj = obj;
        cJSON_AddItemToObject(obj, "seed", cJSON_CreateString("value"));
        cJSON_AddItemToObject(obj, "Key", cJSON_CreateNumber(1));
        cJSON_AddItemToObject(obj, "key", cJSON_CreateNumber(2));
    }

    size_t key_len = remaining > 16 ? 16 : remaining;
    char keybuf[17];
    memcpy(keybuf, ptr, key_len);
    keybuf[key_len] = '\0';

    cJSON *newitem1 = make_item_from_bytes(&ptr, &remaining);
    cJSON *newitem2 = make_item_from_bytes(&ptr, &remaining);

    if (newitem1 == NULL) {
        newitem1 = cJSON_CreateNull();
    }
    if (newitem2 == NULL) {
        newitem2 = cJSON_CreateNull();
    }

    (void)cJSON_HasObjectItem(NULL, keybuf);
    (void)cJSON_HasObjectItem(obj, NULL);
    (void)cJSON_HasObjectItem(obj, keybuf);
    (void)cJSON_HasObjectItem(obj, "seed");
    (void)cJSON_HasObjectItem(obj, "Key");
    (void)cJSON_HasObjectItem(obj, "key");

    if (!cJSON_ReplaceItemInObject(obj, keybuf, newitem1)) {
        cJSON_Delete(newitem1);
    }
    if (!cJSON_ReplaceItemInObjectCaseSensitive(obj, keybuf, newitem2)) {
        cJSON_Delete(newitem2);
    }

    cJSON *tmp1 = cJSON_CreateString("x");
    if (!cJSON_ReplaceItemInObject(obj, "seed", tmp1)) {
        cJSON_Delete(tmp1);
    }

    cJSON *tmp2 = cJSON_CreateString("y");
    if (!cJSON_ReplaceItemInObjectCaseSensitive(obj, "Key", tmp2)) {
        cJSON_Delete(tmp2);
    }

    (void)cJSON_Compare(NULL, root1, 0);
    (void)cJSON_Compare(root1, NULL, 1);
    (void)cJSON_Compare(root1, root1, 0);
    (void)cJSON_Compare(root1, root2, 0);
    (void)cJSON_Compare(root1, root2, 1);

    int prebuffer = read_int(&ptr, &remaining);
    if (prebuffer < 0) {
        prebuffer = -(prebuffer % 1024);
    } else {
        prebuffer = prebuffer % 1024;
    }

    char *printed1 = cJSON_PrintBuffered(root1, prebuffer, 0);
    if (printed1 != NULL) {
        free(printed1);
    }

    char *printed2 = cJSON_PrintBuffered(root1, prebuffer, 1);
    if (printed2 != NULL) {
        free(printed2);
    }

    char *printed3 = cJSON_PrintBuffered(root2, 0, (cJSON_bool)(Size & 1));
    if (printed3 != NULL) {
        free(printed3);
    }

    if (owned_obj != NULL) {
        cJSON_Delete(owned_obj);
    }
    cJSON_Delete(root2);
    cJSON_Delete(root1);
    free(input);
    return 0;
}