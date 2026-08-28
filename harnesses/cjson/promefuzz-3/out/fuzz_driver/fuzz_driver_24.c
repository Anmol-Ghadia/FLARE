// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2334:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2417:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2294:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2368:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2368:26 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2368:26 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_DetachItemFromArray at cJSON.c:2294:23 in cJSON.h
// cJSON_ReplaceItemInArray at cJSON.c:2417:26 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2334:26 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2368:26 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2334:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemViaPointer at cJSON.c:2368:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_InsertItemInArray at cJSON.c:2334:26 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
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

static int read_int(const uint8_t *data, size_t size, size_t *offset) {
    return (int)read_u32(data, size, offset);
}

static cJSON *make_item_from_byte(uint8_t b) {
    switch (b % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((b >> 1) & 1);
        case 2:
            return cJSON_CreateNumber((double)((int8_t)b));
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

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        (void)fwrite(data, 1, size, fp);
        fclose(fp);
    }
}

static int item_belongs_to_parent(cJSON *parent, cJSON *item) {
    cJSON *cur;
    if (parent == NULL || item == NULL) {
        return 0;
    }
    cur = parent->child;
    while (cur != NULL) {
        if (cur == item) {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    int i;

    write_dummy_file(Data, Size);

    cJSON *array = cJSON_CreateArray();
    if (array == NULL) {
        return 0;
    }

    int initial = (Size > 0) ? (int)(Data[offset++] % 16) : 0;
    for (i = 0; i < initial; i++) {
        uint8_t b = (offset < Size) ? Data[offset++] : (uint8_t)i;
        cJSON *it = make_item_from_byte(b);
        if (it == NULL) {
            continue;
        }
        if (!cJSON_InsertItemInArray(array, cJSON_GetArraySize(array), it)) {
            cJSON_Delete(it);
        }
    }

    {
        int ops = (Size > offset) ? (int)(Data[offset++] % 64) : 0;
        for (i = 0; i < ops; i++) {
            uint8_t op = (offset < Size) ? Data[offset++] : 0;
            int idx = read_int(Data, Size, &offset);
            cJSON *newitem = NULL;
            cJSON *detached = NULL;
            int arrsz;

            switch (op % 8) {
                case 0:
                    arrsz = cJSON_GetArraySize(array);
                    (void)arrsz;
                    break;

                case 1:
                    newitem = make_item_from_byte((offset < Size) ? Data[offset++] : op);
                    if (newitem != NULL) {
                        if (!cJSON_InsertItemInArray(array, idx, newitem)) {
                            cJSON_Delete(newitem);
                        }
                    }
                    break;

                case 2:
                    newitem = make_item_from_byte((offset < Size) ? Data[offset++] : op);
                    if (newitem != NULL) {
                        if (!cJSON_ReplaceItemInArray(array, idx, newitem)) {
                            cJSON_Delete(newitem);
                        }
                    }
                    break;

                case 3:
                    detached = cJSON_DetachItemFromArray(array, idx);
                    if (detached != NULL) {
                        cJSON_Delete(detached);
                    }
                    break;

                case 4: {
                    cJSON *target = cJSON_GetArrayItem(array, idx);
                    newitem = make_item_from_byte((offset < Size) ? Data[offset++] : op);
                    if (newitem != NULL) {
                        if (target != NULL && item_belongs_to_parent(array, target)) {
                            if (!cJSON_ReplaceItemViaPointer(array, target, newitem)) {
                                cJSON_Delete(newitem);
                            }
                        } else {
                            cJSON_Delete(newitem);
                        }
                    }
                    break;
                }

                case 5: {
                    cJSON *target = cJSON_GetArrayItem(array, idx);
                    if (target != NULL && item_belongs_to_parent(array, target)) {
                        (void)cJSON_ReplaceItemViaPointer(array, target, target);
                    } else {
                        (void)cJSON_ReplaceItemViaPointer(NULL, NULL, NULL);
                    }
                    break;
                }

                case 6:
                    (void)cJSON_GetArraySize(NULL);
                    (void)cJSON_DetachItemFromArray(array, -1);
                    (void)cJSON_ReplaceItemInArray(array, -1, NULL);
                    (void)cJSON_InsertItemInArray(array, -1, NULL);
                    (void)cJSON_ReplaceItemViaPointer(NULL, NULL, NULL);
                    break;

                case 7: {
                    cJSON *other_parent = cJSON_CreateArray();
                    cJSON *foreign = make_item_from_byte((offset < Size) ? Data[offset++] : op);
                    cJSON *replacement = make_item_from_byte((offset < Size) ? Data[offset++] : (uint8_t)(op + 1));
                    if (other_parent != NULL && foreign != NULL) {
                        if (!cJSON_InsertItemInArray(other_parent, 0, foreign)) {
                            cJSON_Delete(foreign);
                            foreign = NULL;
                        }
                    }
                    if (replacement != NULL) {
                        if (foreign != NULL && item_belongs_to_parent(array, foreign)) {
                            if (!cJSON_ReplaceItemViaPointer(array, foreign, replacement)) {
                                cJSON_Delete(replacement);
                            }
                        } else {
                            cJSON_Delete(replacement);
                        }
                    }
                    if (other_parent != NULL) {
                        cJSON_Delete(other_parent);
                    } else if (foreign != NULL) {
                        cJSON_Delete(foreign);
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }

    cJSON_Delete(array);
    return 0;
}