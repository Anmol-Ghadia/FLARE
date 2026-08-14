// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2267:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2286:20 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2274:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2267:23 in cJSON.h
// cJSON_IsArray at cJSON.c:3000:26 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
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
    if (*offset < size) v |= (uint32_t)data[(*offset)++];
    if (*offset < size) v |= (uint32_t)data[(*offset)++] << 8;
    if (*offset < size) v |= (uint32_t)data[(*offset)++] << 16;
    if (*offset < size) v |= (uint32_t)data[(*offset)++] << 24;
    return v;
}

static int read_int(const uint8_t *data, size_t size, size_t *offset) {
    return (int)read_u32(data, size, offset);
}

static char *read_string(const uint8_t *data, size_t size, size_t *offset) {
    if (*offset >= size) {
        char *s = (char *)malloc(1);
        if (s) s[0] = '\0';
        return s;
    }

    size_t remaining = size - *offset;
    size_t len = read_u32(data, size, offset);
    if (remaining == 0) {
        char *s = (char *)malloc(1);
        if (s) s[0] = '\0';
        return s;
    }

    len %= (remaining + 1);
    if (*offset + len > size) {
        len = size - *offset;
    }

    char *out = (char *)malloc(len + 1);
    if (!out) {
        return NULL;
    }

    if (len > 0) {
        memcpy(out, data + *offset, len);
        for (size_t i = 0; i < len; i++) {
            if (out[i] == '\0') {
                out[i] = 'A';
            }
        }
    }
    out[len] = '\0';
    *offset += len;
    return out;
}

static cJSON *make_item_from_data(const uint8_t *data, size_t size, size_t *offset) {
    int selector = read_int(data, size, offset) % 6;
    if (selector < 0) selector = -selector;

    switch (selector) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool(read_int(data, size, offset) & 1);
        case 2:
            return cJSON_CreateNumber((double)read_int(data, size, offset));
        case 3: {
            char *s = read_string(data, size, offset);
            cJSON *item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        case 4:
            return cJSON_CreateArray();
        case 5:
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    if (root == NULL || array == NULL) {
        cJSON_Delete(root);
        cJSON_Delete(array);
        return 0;
    }

    cJSON_AddItemToObject(root, "array", array);

    int initial_count = 1 + (Size > 0 ? (int)(Data[0] % 8) : 0);
    for (int i = 0; i < initial_count; i++) {
        cJSON *item = make_item_from_data(Data, Size, &offset);
        if (item != NULL) {
            cJSON_AddItemToArray(array, item);
        }
    }

    int object_count = 1 + (Size > 1 ? (int)(Data[1] % 8) : 0);
    for (int i = 0; i < object_count; i++) {
        char *key = read_string(Data, Size, &offset);
        cJSON *item = make_item_from_data(Data, Size, &offset);
        if (key != NULL && item != NULL) {
            if (!cJSON_AddItemToObject(root, key, item)) {
                cJSON_Delete(item);
            }
        } else if (item != NULL) {
            cJSON_Delete(item);
        }
        free(key);
    }

    int operations = 1 + (Size > 2 ? (int)(Data[2] % 32) : 0);
    for (int i = 0; i < operations; i++) {
        int op = read_int(Data, Size, &offset) % 8;
        if (op < 0) op = -op;

        char *key = read_string(Data, Size, &offset);
        int index = read_int(Data, Size, &offset);

        switch (op) {
            case 0: {
                cJSON *detached = cJSON_DetachItemFromObject(root, key ? key : "");
                if (detached != NULL) {
                    char *newkey = read_string(Data, Size, &offset);
                    if (newkey != NULL) {
                        if (!cJSON_AddItemToObject(root, newkey, detached)) {
                            cJSON_Delete(detached);
                        }
                        free(newkey);
                    } else {
                        cJSON_Delete(detached);
                    }
                }
                break;
            }
            case 1:
                cJSON_DeleteItemFromObjectCaseSensitive(root, key ? key : "");
                break;
            case 2:
                cJSON_DeleteItemFromArray(array, index);
                break;
            case 3: {
                cJSON *item = make_item_from_data(Data, Size, &offset);
                if (item != NULL) {
                    if (!cJSON_AddItemToObject(root, key ? key : "", item)) {
                        cJSON_Delete(item);
                    }
                }
                break;
            }
            case 4:
                cJSON_DeleteItemFromObject(root, key ? key : "");
                break;
            case 5: {
                cJSON *detached = cJSON_DetachItemFromObjectCaseSensitive(root, key ? key : "");
                if (detached != NULL) {
                    cJSON_AddItemToArray(array, detached);
                }
                break;
            }
            case 6: {
                cJSON *item = make_item_from_data(Data, Size, &offset);
                if (item != NULL) {
                    cJSON_AddItemToArray(array, item);
                }
                break;
            }
            case 7: {
                cJSON *detached = cJSON_DetachItemFromObject(root, "array");
                if (detached != NULL) {
                    if (cJSON_IsArray(detached)) {
                        cJSON_DeleteItemFromArray(detached, index);
                    }
                    if (!cJSON_AddItemToObject(root, "array", detached)) {
                        cJSON_Delete(detached);
                    }
                }
                break;
            }
        }

        free(key);
    }

    cJSON_Delete(root);
    remove("./dummy_file");
    return 0;
}