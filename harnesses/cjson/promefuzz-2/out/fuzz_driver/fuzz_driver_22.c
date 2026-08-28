// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2216:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2216:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2267:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2274:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2267:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
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

static char *make_string_from_bytes(const uint8_t *data, size_t size, size_t *consumed)
{
    size_t len;
    char *out;

    if (size == 0) {
        out = (char *)malloc(1);
        if (out != NULL) {
            out[0] = '\0';
        }
        if (consumed != NULL) {
            *consumed = 0;
        }
        return out;
    }

    len = data[0] % (size + 1);
    out = (char *)malloc(len + 1);
    if (out == NULL) {
        if (consumed != NULL) {
            *consumed = 1 + len;
        }
        return NULL;
    }

    if (len > 0 && size > 1) {
        size_t copy_len = len;
        if (copy_len > size - 1) {
            copy_len = size - 1;
        }
        memcpy(out, data + 1, copy_len);
        if (copy_len < len) {
            memset(out + copy_len, 'A', len - copy_len);
        }
    }
    out[len] = '\0';

    if (consumed != NULL) {
        *consumed = 1 + len;
    }
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *tmp = NULL;
    cJSON *detached = NULL;
    cJSON *detached2 = NULL;
    cJSON *arr = NULL;
    cJSON *child = NULL;
    char *name1 = NULL;
    char *name2 = NULL;
    char *name3 = NULL;
    char *name4 = NULL;
    char *name5 = NULL;
    size_t off = 0, used = 0;
    cJSON_bool b1 = 0, b2 = 0;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    name1 = make_string_from_bytes(Data + off, (off < Size) ? (Size - off) : 0, &used);
    off += used;
    name2 = make_string_from_bytes(Data + off, (off < Size) ? (Size - off) : 0, &used);
    off += used;
    name3 = make_string_from_bytes(Data + off, (off < Size) ? (Size - off) : 0, &used);
    off += used;
    name4 = make_string_from_bytes(Data + off, (off < Size) ? (Size - off) : 0, &used);
    off += used;
    name5 = make_string_from_bytes(Data + off, (off < Size) ? (Size - off) : 0, &used);
    off += used;

    if (name1 == NULL || name2 == NULL || name3 == NULL || name4 == NULL || name5 == NULL) {
        free(name1);
        free(name2);
        free(name3);
        free(name4);
        free(name5);
        cJSON_Delete(root);
        return 0;
    }

    if (off < Size) {
        b1 = (Data[off] & 1) ? 1 : 0;
        off++;
    }
    if (off < Size) {
        b2 = (Data[off] & 1) ? 1 : 0;
        off++;
    }

    tmp = cJSON_AddBoolToObject(root, name1, b1);
    (void)tmp;

    tmp = cJSON_AddBoolToObject(root, name2, b2);
    (void)tmp;

    arr = cJSON_AddArrayToObject(root, name3);
    if (arr != NULL) {
        cJSON_AddItemToArray(arr, cJSON_CreateBool(b1));
        cJSON_AddItemToArray(arr, cJSON_CreateBool(b2));

        child = arr->child;
        if (child != NULL) {
            detached = cJSON_DetachItemViaPointer(arr, child);
            if (detached != NULL) {
                cJSON_Delete(detached);
            }
        }

        child = arr->child;
        if (child != NULL && child->next != NULL) {
            detached2 = cJSON_DetachItemViaPointer(arr, child->next);
            if (detached2 != NULL) {
                cJSON_Delete(detached2);
            }
        }
    }

    detached = cJSON_DetachItemFromObject(root, name1);
    if (detached != NULL) {
        cJSON_Delete(detached);
    }

    detached = cJSON_DetachItemFromObjectCaseSensitive(root, name2);
    if (detached != NULL) {
        cJSON_Delete(detached);
    }

    cJSON_DeleteItemFromObject(root, name3);
    cJSON_DeleteItemFromObject(root, name4);
    cJSON_DeleteItemFromObject(root, name5);

    tmp = cJSON_AddBoolToObject(root, name4, b1);
    (void)tmp;
    arr = cJSON_AddArrayToObject(root, name5);
    if (arr != NULL) {
        cJSON_AddItemToArray(arr, cJSON_CreateBool(1));
    }

    detached = cJSON_DetachItemFromObject(root, name5);
    if (detached != NULL) {
        cJSON_Delete(detached);
    }

    free(name1);
    free(name2);
    free(name3);
    free(name4);
    free(name5);
    cJSON_Delete(root);
    return 0;
}