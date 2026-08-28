// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2294:23 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2304:20 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2417:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2294:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1232:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3042:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
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

static cJSON *make_item_from_byte(uint8_t b)
{
    switch (b % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((b >> 1) & 1);
        case 2:
            return cJSON_CreateNumber((double)(int8_t)b);
        case 3: {
            char s[2];
            s[0] = (char)b;
            s[1] = '\0';
            return cJSON_CreateString(s);
        }
        case 4:
            return cJSON_CreateArray();
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *detached = NULL;

    root = cJSON_ParseWithLength((const char *)Data, Size);
    if (root != NULL && cJSON_IsArray(root)) {
        array = root;
    } else {
        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        array = cJSON_CreateArray();
        if (array == NULL) {
            return 0;
        }
        root = array;

        const uint8_t *p = Data;
        size_t remaining = Size;
        size_t count = remaining > 64 ? 64 : remaining;
        for (size_t i = 0; i < count; ++i) {
            cJSON *item = make_item_from_byte(p[i]);
            if (item != NULL) {
                cJSON_AddItemToArray(array, item);
            }
        }
    }

    {
        const uint8_t *p = Data;
        size_t remaining = Size;

        int initial_size = cJSON_GetArraySize(array);
        (void)initial_size;

        for (int iter = 0; iter < 16 && remaining > 0; ++iter) {
            int op = (int)(*p++ % 6);
            remaining--;

            int idx = 0;
            if (remaining > 0) {
                idx = read_int(&p, &remaining);
            }

            switch (op) {
                case 0: {
                    cJSON *item = cJSON_GetArrayItem(array, idx);
                    (void)cJSON_IsObject(item);
                    break;
                }
                case 1: {
                    cJSON *tmp = cJSON_DetachItemFromArray(array, idx);
                    if (tmp != NULL) {
                        (void)cJSON_IsObject(tmp);
                        cJSON_Delete(tmp);
                    }
                    break;
                }
                case 2:
                    cJSON_DeleteItemFromArray(array, idx);
                    break;
                case 3: {
                    cJSON *newitem = NULL;
                    uint8_t selector = 0;
                    if (remaining > 0) {
                        selector = *p++;
                        remaining--;
                    }
                    newitem = make_item_from_byte(selector);
                    if (newitem != NULL) {
                        if (!cJSON_ReplaceItemInArray(array, idx, newitem)) {
                            cJSON_Delete(newitem);
                        }
                    }
                    break;
                }
                case 4: {
                    int sz = cJSON_GetArraySize(array);
                    if (sz > 0) {
                        int safe_idx = idx;
                        if (safe_idx < 0) {
                            safe_idx = -safe_idx;
                        }
                        safe_idx %= (sz + 1);
                        cJSON *item = cJSON_GetArrayItem(array, safe_idx);
                        (void)cJSON_IsObject(item);
                    } else {
                        cJSON *item = cJSON_GetArrayItem(array, idx);
                        (void)cJSON_IsObject(item);
                    }
                    break;
                }
                case 5: {
                    detached = cJSON_DetachItemFromArray(array, idx);
                    if (detached != NULL) {
                        cJSON_AddItemToArray(array, detached);
                        detached = NULL;
                    }
                    break;
                }
                default:
                    break;
            }

            (void)cJSON_GetArraySize(array);
            (void)cJSON_IsObject(array);
        }
    }

    if (detached != NULL) {
        cJSON_Delete(detached);
        detached = NULL;
    }

    cJSON_Delete(root);
    return 0;
}