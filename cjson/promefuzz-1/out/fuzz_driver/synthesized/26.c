// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ReplaceItemInObject at cJSON.c:2408:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2216:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2216:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_CreateFalse at cJSON.c:2441:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
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

static char *read_string(const uint8_t **data, size_t *size) {
    if (*size == 0) {
        char *s = (char *)malloc(1);
        if (s) {
            s[0] = '\0';
        }
        return s;
    }

    size_t len = (size_t)((*data)[0] % (*size + 1));
    *data += 1;
    *size -= 1;

    if (len > *size) {
        len = *size;
    }

    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    memcpy(s, *data, len);
    s[len] = '\0';

    *data += len;
    *size -= len;
    return s;
}

static cJSON *make_item_from_byte(uint8_t b, const uint8_t **data, size_t *size) {
    switch (b % 6) {
        case 0:
            return cJSON_CreateTrue();
        case 1:
            return cJSON_CreateFalse();
        case 2:
            return cJSON_CreateNull();
        case 3:
            return cJSON_CreateNumber((double)read_int(data, size));
        case 4: {
            char *s = read_string(data, size);
            cJSON *item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        default:
            return cJSON_CreateArray();
    }
}

int LLVMFuzzerTestOneInput_26(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *object = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    cJSON *aux_array = cJSON_CreateArray();
    cJSON *owned_for_ref = cJSON_CreateTrue();

    if (object == NULL || array == NULL || aux_array == NULL || owned_for_ref == NULL) {
        cJSON_Delete(object);
        cJSON_Delete(array);
        cJSON_Delete(aux_array);
        cJSON_Delete(owned_for_ref);
        return 0;
    }

    if (remaining == 0) {
        cJSON_AddItemToObject(object, "default", cJSON_CreateTrue());
        cJSON_AddItemReferenceToArray(array, owned_for_ref);
        cJSON_DeleteItemFromArray(array, 0);
        cJSON_ReplaceItemInObject(object, "default", cJSON_CreateTrue());
        cJSON_Delete(object);
        cJSON_Delete(array);
        cJSON_Delete(aux_array);
        cJSON_Delete(owned_for_ref);
        return 0;
    }

    size_t iterations = remaining > 64 ? 64 : remaining;
    for (size_t i = 0; i < iterations && remaining > 0; ++i) {
        uint8_t op = *ptr++;
        remaining--;

        switch (op % 6) {
            case 0: {
                char *key = read_string(&ptr, &remaining);
                cJSON *item = make_item_from_byte(op, &ptr, &remaining);
                if (item != NULL) {
                    cJSON_AddItemToObject(object, key ? key : "", item);
                } else {
                    cJSON_Delete(item);
                }
                free(key);
                break;
            }

            case 1: {
                char *key = read_string(&ptr, &remaining);
                cJSON *newitem = make_item_from_byte(op ^ 0x5a, &ptr, &remaining);
                if (newitem != NULL) {
                    if (!cJSON_ReplaceItemInObject(object, key ? key : "", newitem)) {
                        cJSON_Delete(newitem);
                    }
                }
                free(key);
                break;
            }

            case 2: {
                cJSON *item = make_item_from_byte(op ^ 0xa5, &ptr, &remaining);
                if (item != NULL) {
                    cJSON_AddItemToArray(array, item);
                }
                if (owned_for_ref != NULL) {
                    cJSON_AddItemReferenceToArray(aux_array, owned_for_ref);
                }
                break;
            }

            case 3: {
                int which = read_int(&ptr, &remaining);
                cJSON_DeleteItemFromArray(array, which);
                break;
            }

            case 4: {
                cJSON *child = array->child;
                int steps = remaining > 0 ? (int)(*ptr++ % 8) : 0;
                if (remaining > 0) {
                    remaining--;
                }
                while (child != NULL && steps-- > 0) {
                    child = child->next;
                }
                cJSON *detached = cJSON_DetachItemViaPointer(array, child);
                if (detached != NULL) {
                    cJSON_Delete(detached);
                }

                cJSON *obj_child = object->child;
                steps = remaining > 0 ? (int)(*ptr++ % 8) : 0;
                if (remaining > 0) {
                    remaining--;
                }
                while (obj_child != NULL && steps-- > 0) {
                    obj_child = obj_child->next;
                }
                detached = cJSON_DetachItemViaPointer(object, obj_child);
                if (detached != NULL) {
                    cJSON_Delete(detached);
                }
                break;
            }

            case 5: {
                cJSON *t = cJSON_CreateTrue();
                if (t != NULL) {
                    char *key = read_string(&ptr, &remaining);
                    if (!cJSON_AddItemToObject(object, key ? key : "t", t)) {
                        cJSON_Delete(t);
                    }
                    free(key);
                }
                break;
            }
        }
    }

    cJSON_Delete(object);
    cJSON_Delete(array);
    cJSON_Delete(aux_array);
    cJSON_Delete(owned_for_ref);
    return 0;
}