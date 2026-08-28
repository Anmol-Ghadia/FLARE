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

static char *make_string(const uint8_t **data, size_t *size, size_t max_len) {
    if (*size == 0) {
        char *s = (char *)malloc(1);
        if (s) {
            s[0] = '\0';
        }
        return s;
    }

    size_t len = read_u32(data, size) % (max_len + 1);
    if (len > *size) {
        len = *size;
    }

    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (*data)[i];
        if (c == '\0') {
            c = 'A';
        }
        s[i] = (char)c;
    }
    s[len] = '\0';

    *data += len;
    *size -= len;
    return s;
}

static cJSON *make_item(const uint8_t **data, size_t *size) {
    if (*size == 0) {
        return cJSON_CreateNull();
    }

    uint32_t kind = read_u32(data, size) % 7;
    switch (kind) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((int)(read_u32(data, size) & 1));
        case 2:
            return cJSON_CreateNumber((double)(int32_t)read_u32(data, size));
        case 3: {
            char *s = make_string(data, size, 64);
            cJSON *item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        case 4:
            return cJSON_CreateArray();
        case 5:
            return cJSON_CreateObject();
        case 6: {
            char *s = make_string(data, size, 64);
            cJSON *item = cJSON_CreateRaw(s ? s : "");
            free(s);
            return item;
        }
        default:
            return cJSON_CreateNull();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    cJSON *object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    static const char *const_keys[] = {
        "A", "a", "Key", "key", "CONST", "", "dummy", "CaseSensitive"
    };
    const size_t const_key_count = sizeof(const_keys) / sizeof(const_keys[0]);

    size_t initial_items = (remaining > 0) ? (read_u32(&ptr, &remaining) % 8) : 0;
    for (size_t i = 0; i < initial_items; ++i) {
        char *dyn_key = make_string(&ptr, &remaining, 32);
        cJSON *item = make_item(&ptr, &remaining);
        if (item == NULL) {
            free(dyn_key);
            continue;
        }

        if ((read_u32(&ptr, &remaining) & 1) == 0) {
            if (!cJSON_AddItemToObject(object, dyn_key ? dyn_key : "", item)) {
                cJSON_Delete(item);
            }
        } else {
            const char *ckey = const_keys[read_u32(&ptr, &remaining) % const_key_count];
            if (!cJSON_AddItemToObjectCS(object, ckey, item)) {
                cJSON_Delete(item);
            }
        }
        free(dyn_key);
    }

    size_t ops = (remaining > 0) ? (read_u32(&ptr, &remaining) % 32) : 0;
    for (size_t i = 0; i < ops; ++i) {
        uint32_t op = read_u32(&ptr, &remaining) % 6;
        char *dyn_key = make_string(&ptr, &remaining, 32);
        const char *key = ((read_u32(&ptr, &remaining) & 1) == 0)
            ? (dyn_key ? dyn_key : "")
            : const_keys[read_u32(&ptr, &remaining) % const_key_count];

        switch (op) {
            case 0: {
                cJSON *item = make_item(&ptr, &remaining);
                if (item != NULL) {
                    if (!cJSON_AddItemToObject(object, key, item)) {
                        cJSON_Delete(item);
                    }
                }
                break;
            }
            case 1: {
                cJSON *item = make_item(&ptr, &remaining);
                if (item != NULL) {
                    if (!cJSON_AddItemToObjectCS(object, key, item)) {
                        cJSON_Delete(item);
                    }
                }
                break;
            }
            case 2: {
                cJSON *detached = cJSON_DetachItemFromObject(object, key);
                if (detached != NULL) {
                    if ((read_u32(&ptr, &remaining) & 1) == 0) {
                        cJSON_Delete(detached);
                    } else {
                        const char *reattach_key = const_keys[read_u32(&ptr, &remaining) % const_key_count];
                        if (!cJSON_AddItemToObjectCS(object, reattach_key, detached)) {
                            cJSON_Delete(detached);
                        }
                    }
                }
                break;
            }
            case 3: {
                cJSON *detached = cJSON_DetachItemFromObjectCaseSensitive(object, key);
                if (detached != NULL) {
                    if ((read_u32(&ptr, &remaining) & 1) == 0) {
                        cJSON_Delete(detached);
                    } else {
                        char *new_key = make_string(&ptr, &remaining, 32);
                        if (!cJSON_AddItemToObject(object, new_key ? new_key : "", detached)) {
                            cJSON_Delete(detached);
                        }
                        free(new_key);
                    }
                }
                break;
            }
            case 4:
                cJSON_DeleteItemFromObject(object, key);
                break;
            case 5:
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
                break;
            default:
                break;
        }

        free(dyn_key);
    }

    cJSON_Delete(object);
    return 0;
}