// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2292:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2375:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2252:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2292:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2326:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2326:26 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2252:23 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2375:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2292:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2326:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_CreateFalse at cJSON.c:2441:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size) {
    return (int)read_u32(data, size);
}

static cJSON *make_item_from_byte(uint8_t b, const uint8_t **data, size_t *size) {
    switch (b % 8) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((b >> 3) & 1);
        case 2:
            return cJSON_CreateNumber((double)(int8_t)b);
        case 3: {
            size_t len = (*size > 0) ? ((*data)[0] % (*size)) : 0;
            if (*size > 0) {
                (*data)++;
                (*size)--;
            }
            char *buf = (char *)malloc(len + 1);
            cJSON *s;
            if (buf == NULL) {
                return cJSON_CreateString("");
            }
            if (len > *size) {
                len = *size;
            }
            memcpy(buf, *data, len);
            buf[len] = '\0';
            *data += len;
            *size -= len;
            s = cJSON_CreateString(buf);
            free(buf);
            return s;
        }
        case 4:
            return cJSON_CreateArray();
        case 5:
            return cJSON_CreateObject();
        case 6:
            return cJSON_CreateTrue();
        default:
            return cJSON_CreateFalse();
    }
}

static void populate_array(cJSON *array, const uint8_t **data, size_t *size) {
    if (array == NULL || size == NULL || data == NULL) {
        return;
    }

    size_t count = (*size > 0) ? ((*data)[0] % 8) : 0;
    if (*size > 0) {
        (*data)++;
        (*size)--;
    }

    for (size_t i = 0; i < count && *size > 0; ++i) {
        uint8_t tag = **data;
        cJSON *item;
        (*data)++;
        (*size)--;

        item = make_item_from_byte(tag, data, size);
        if (item == NULL) {
            item = cJSON_CreateNull();
        }
        if (item == NULL) {
            continue;
        }

        cJSON_AddItemToArray(array, item);
    }
}

int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *array = cJSON_CreateArray();
    cJSON *array2 = cJSON_CreateArray();
    cJSON *object = cJSON_CreateObject();
    if (array == NULL || array2 == NULL || object == NULL) {
        cJSON_Delete(array);
        cJSON_Delete(array2);
        cJSON_Delete(object);
        return 0;
    }

    populate_array(array, &ptr, &remaining);
    populate_array(array2, &ptr, &remaining);

    {
        cJSON *dup = cJSON_Duplicate(array, 1);
        if (dup != NULL) {
            cJSON_AddItemToObject(object, "arr", dup);
        }
    }
    {
        cJSON *dup = cJSON_Duplicate(array2, 1);
        if (dup != NULL) {
            cJSON_AddItemToObject(object, "arr2", dup);
        }
    }

    {
        cJSON *dup_shallow = cJSON_Duplicate(array, 0);
        cJSON *dup_deep = cJSON_Duplicate(array, 1);
        cJSON_Delete(dup_shallow);
        cJSON_Delete(dup_deep);
    }

    {
        int idx1 = (remaining >= 4) ? read_int(&ptr, &remaining) : 0;
        int idx2 = (remaining >= 4) ? read_int(&ptr, &remaining) : 0;
        int idx3 = (remaining >= 4) ? read_int(&ptr, &remaining) : 0;
        int idx4 = (remaining >= 4) ? read_int(&ptr, &remaining) : 0;

        cJSON *newitem1 = make_item_from_byte((remaining > 0) ? ptr[0] : 0, &ptr, &remaining);
        if (newitem1 != NULL) {
            if (!cJSON_InsertItemInArray(array, idx1, newitem1)) {
                cJSON_Delete(newitem1);
            }
        }

        {
            cJSON *newitem2 = make_item_from_byte((remaining > 0) ? ptr[0] : 1, &ptr, &remaining);
            if (newitem2 != NULL) {
                if (!cJSON_ReplaceItemInArray(array, idx2, newitem2)) {
                    cJSON_Delete(newitem2);
                }
            }
        }

        {
            cJSON *detached = cJSON_DetachItemFromArray(array, idx3);
            if (detached != NULL) {
                cJSON *dup_detached = cJSON_Duplicate(detached, (remaining > 0) ? (ptr[0] & 1) : 0);
                if (remaining > 0) {
                    ptr++;
                    remaining--;
                }
                if (dup_detached != NULL) {
                    if (!cJSON_InsertItemInArray(array2, idx4, dup_detached)) {
                        cJSON_Delete(dup_detached);
                    }
                }
                cJSON_Delete(detached);
            }
        }

        cJSON_DeleteItemFromArray(array2, idx1);

        {
            cJSON *target = cJSON_GetArrayItem(array, (idx2 >= 0) ? idx2 : 0);
            cJSON *replacement = make_item_from_byte((remaining > 0) ? ptr[0] : 2, &ptr, &remaining);
            if (replacement != NULL) {
                if (!cJSON_ReplaceItemViaPointer(array, target, replacement)) {
                    cJSON_Delete(replacement);
                }
            }
        }
    }

    {
        cJSON *same_item = cJSON_GetArrayItem(array, 0);
        (void)cJSON_ReplaceItemViaPointer(array, same_item, same_item);
    }

    (void)cJSON_DetachItemFromArray(array, -1);
    cJSON_DeleteItemFromArray(array, -1);

    {
        cJSON *tmp = cJSON_CreateNull();
        if (tmp != NULL) {
            if (!cJSON_ReplaceItemInArray(array, -1, tmp)) {
                cJSON_Delete(tmp);
            }
        }
    }

    {
        cJSON *tmp = cJSON_CreateNull();
        if (tmp != NULL) {
            if (!cJSON_InsertItemInArray(array, -1, tmp)) {
                cJSON_Delete(tmp);
            }
        }
    }

    (void)cJSON_Duplicate(NULL, 0);
    (void)cJSON_ReplaceItemViaPointer(NULL, NULL, NULL);

    cJSON_Delete(object);
    cJSON_Delete(array);
    cJSON_Delete(array2);
    return 0;
}