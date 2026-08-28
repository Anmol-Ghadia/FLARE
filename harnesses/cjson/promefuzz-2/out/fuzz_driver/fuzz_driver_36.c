// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2252:23 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2292:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2292:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2326:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2375:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3000:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
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

static int consume_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(int)) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static cJSON *make_item_from_bytes(const uint8_t **data, size_t *size)
{
    if (*size == 0) {
        return cJSON_CreateNull();
    }

    uint8_t tag = **data;
    (*data)++;
    (*size)--;

    switch (tag % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool(tag & 1);
        case 2:
            return cJSON_CreateNumber((double)consume_int(data, size));
        case 3: {
            size_t len = (*size > 32) ? 32 : *size;
            char *buf = (char *)malloc(len + 1);
            if (buf == NULL) {
                return cJSON_CreateNull();
            }
            if (len > 0) {
                memcpy(buf, *data, len);
                *data += len;
                *size -= len;
            }
            buf[len] = '\0';
            cJSON *item = cJSON_CreateString(buf);
            free(buf);
            return item ? item : cJSON_CreateNull();
        }
        case 4:
            return cJSON_CreateArray();
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *root = NULL;
    char *text = (char *)malloc(Size + 1);
    if (text != NULL) {
        if (Size > 0) {
            memcpy(text, Data, Size);
        }
        text[Size] = '\0';
        root = cJSON_Parse(text);
        free(text);
    }

    if (root == NULL || !cJSON_IsArray(root)) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        root = cJSON_CreateArray();
    }

    const uint8_t *ptr = Data;
    size_t remaining = Size;

    int initial_count = 0;
    if (remaining > 0) {
        initial_count = (int)(remaining % 8);
    }

    for (int i = 0; i < initial_count; i++) {
        cJSON *item = make_item_from_bytes(&ptr, &remaining);
        if (item != NULL) {
            cJSON_AddItemToArray(root, item);
        }
    }

    (void)cJSON_GetArraySize(NULL);
    (void)cJSON_GetArraySize(root);

    int operations = 1;
    if (remaining > 0) {
        operations = (int)(remaining % 32) + 1;
    }

    for (int i = 0; i < operations; i++) {
        int op = consume_int(&ptr, &remaining);
        int idx = consume_int(&ptr, &remaining);

        switch ((unsigned int)op % 6U) {
            case 0: {
                cJSON *detached = cJSON_DetachItemFromArray(root, idx);
                if (detached != NULL) {
                    int reinsertion_index = consume_int(&ptr, &remaining);
                    if (!cJSON_InsertItemInArray(root, reinsertion_index, detached)) {
                        cJSON_Delete(detached);
                    }
                }
                break;
            }
            case 1: {
                cJSON *newitem = make_item_from_bytes(&ptr, &remaining);
                if (newitem != NULL) {
                    if (!cJSON_InsertItemInArray(root, idx, newitem)) {
                        cJSON_Delete(newitem);
                    }
                }
                break;
            }
            case 2: {
                (void)cJSON_GetArraySize(root);
                break;
            }
            case 3: {
                int sz = cJSON_GetArraySize(root);
                cJSON *target = NULL;
                if (sz > 0) {
                    int which = idx;
                    if (which < 0) {
                        which = -which;
                    }
                    which %= sz;
                    target = cJSON_GetArrayItem(root, which);
                }
                cJSON *replacement = make_item_from_bytes(&ptr, &remaining);
                if (replacement != NULL) {
                    if (!cJSON_ReplaceItemViaPointer(root, target, replacement)) {
                        cJSON_Delete(replacement);
                    }
                }
                break;
            }
            case 4: {
                cJSON *newitem = make_item_from_bytes(&ptr, &remaining);
                if (newitem != NULL) {
                    if (!cJSON_ReplaceItemInArray(root, idx, newitem)) {
                        cJSON_Delete(newitem);
                    }
                }
                break;
            }
            case 5:
            default:
                cJSON_DeleteItemFromArray(root, idx);
                break;
        }
    }

    cJSON_Delete(root);
    return 0;
}