#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*Offset < Size) {
            v |= Data[*Offset];
            (*Offset)++;
        }
    }
    return v;
}

static int read_int(const uint8_t *Data, size_t Size, size_t *Offset) {
    return (int)read_u32(Data, Size, Offset);
}

static size_t read_len(const uint8_t *Data, size_t Size, size_t *Offset, size_t max_len) {
    if (max_len == 0) {
        return 0;
    }
    return (size_t)(read_u32(Data, Size, Offset) % (max_len + 1));
}

static char *make_string(const uint8_t *Data, size_t Size, size_t *Offset) {
    size_t remaining;
    size_t len;
    char *s;

    if (*Offset > Size) {
        *Offset = Size;
    }

    remaining = Size - *Offset;
    len = read_len(Data, Size, Offset, remaining > 64 ? 64 : remaining);

    if (*Offset > Size) {
        *Offset = Size;
    }
    remaining = Size - *Offset;
    if (len > remaining) {
        len = remaining;
    }

    s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(s, Data + *Offset, len);
        *Offset += len;
    }
    s[len] = '\0';
    return s;
}

static cJSON *make_item_from_bytes(const uint8_t *Data, size_t Size, size_t *Offset, int depth) {
    cJSON *item;
    int selector;

    if (depth <= 0) {
        selector = 0;
    } else {
        selector = read_int(Data, Size, Offset) & 7;
    }

    switch (selector) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool(read_int(Data, Size, Offset) & 1);
        case 2:
            return cJSON_CreateNumber((double)(int32_t)read_u32(Data, Size, Offset));
        case 3: {
            char *s = make_string(Data, Size, Offset);
            item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        case 4: {
            char *s = make_string(Data, Size, Offset);
            item = cJSON_CreateRaw(s ? s : "");
            free(s);
            return item;
        }
        case 5: {
            cJSON *arr = cJSON_CreateArray();
            int count = read_int(Data, Size, Offset) & 7;
            int i;
            if (arr == NULL) {
                return NULL;
            }
            for (i = 0; i < count; ++i) {
                cJSON *child = make_item_from_bytes(Data, Size, Offset, depth - 1);
                if (child == NULL) {
                    child = cJSON_CreateNull();
                }
                if (child != NULL) {
                    cJSON_AddItemToArray(arr, child);
                }
            }
            return arr;
        }
        case 6: {
            cJSON *obj = cJSON_CreateObject();
            int count = read_int(Data, Size, Offset) & 7;
            int i;
            if (obj == NULL) {
                return NULL;
            }
            for (i = 0; i < count; ++i) {
                char *key = make_string(Data, Size, Offset);
                cJSON *val = make_item_from_bytes(Data, Size, Offset, depth - 1);
                if (key == NULL) {
                    key = (char *)malloc(2);
                    if (key != NULL) {
                        key[0] = 'k';
                        key[1] = '\0';
                    }
                }
                if (val == NULL) {
                    val = cJSON_CreateNull();
                }
                if (key != NULL && val != NULL) {
                    cJSON_AddItemToObject(obj, key, val);
                } else if (val != NULL) {
                    cJSON_Delete(val);
                }
                free(key);
            }
            return obj;
        }
        default:
            return cJSON_CreateArray();
    }
}

static cJSON *get_nth_child(cJSON *parent, int index) {
    cJSON *child;
    int i = 0;

    if (parent == NULL || index < 0) {
        return NULL;
    }

    child = parent->child;
    while (child != NULL && i < index) {
        child = child->next;
        i++;
    }
    return child;
}

static int count_children(cJSON *parent, int max_count) {
    int count = 0;
    cJSON *child;

    if (parent == NULL) {
        return 0;
    }

    child = parent->child;
    while (child != NULL && count < max_count) {
        count++;
        child = child->next;
    }
    return count;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *array = NULL;
    cJSON *object = NULL;
    cJSON *parsed = NULL;
    cJSON *detached = NULL;
    cJSON *tmp = NULL;
    char *json_text = NULL;
    FILE *fp = NULL;
    int i, ops;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    json_text = make_string(Data, Size, &offset);
    if (json_text != NULL) {
        parsed = cJSON_Parse(json_text);
        free(json_text);
        json_text = NULL;
    }

    array = cJSON_CreateArray();
    object = cJSON_CreateObject();
    if (array == NULL || object == NULL) {
        cJSON_Delete(parsed);
        cJSON_Delete(array);
        cJSON_Delete(object);
        return 0;
    }

    if (parsed != NULL) {
        cJSON_IsObject(parsed);
        tmp = cJSON_Duplicate(parsed, 1);
        if (tmp != NULL) {
            cJSON_AddItemToArray(array, tmp);
        }
        if (cJSON_IsObject(parsed)) {
            tmp = cJSON_Duplicate(parsed, 1);
            if (tmp != NULL) {
                cJSON_AddItemToArray(array, tmp);
            }
        }
    }

    ops = (int)(Size > 128 ? 128 : Size);
    for (i = 0; i < ops; ++i) {
        int action = (offset < Size) ? (Data[offset++] % 10) : (i % 10);
        int child_count = count_children(array, 1024);
        int idx = child_count > 0 ? (read_int(Data, Size, &offset) % (child_count + 2)) : 0;
        cJSON *item;
        cJSON *replacement;

        switch (action) {
            case 0:
                item = make_item_from_bytes(Data, Size, &offset, 3);
                if (item != NULL) {
                    cJSON_AddItemToArray(array, item);
                }
                break;

            case 1:
                item = make_item_from_bytes(Data, Size, &offset, 3);
                if (item == NULL) {
                    item = cJSON_CreateNull();
                }
                if (item != NULL) {
                    if (!cJSON_InsertItemInArray(array, idx, item)) {
                        cJSON_Delete(item);
                    }
                }
                break;

            case 2:
                replacement = make_item_from_bytes(Data, Size, &offset, 3);
                if (replacement == NULL) {
                    replacement = cJSON_CreateNull();
                }
                if (replacement != NULL) {
                    if (!cJSON_ReplaceItemInArray(array, idx, replacement)) {
                        cJSON_Delete(replacement);
                    }
                }
                break;

            case 3:
                item = get_nth_child(array, idx);
                replacement = make_item_from_bytes(Data, Size, &offset, 3);
                if (replacement == NULL) {
                    replacement = cJSON_CreateNull();
                }
                if (item != NULL && replacement != NULL) {
                    if (!cJSON_ReplaceItemViaPointer(array, item, replacement)) {
                        cJSON_Delete(replacement);
                    }
                } else if (replacement != NULL) {
                    cJSON_Delete(replacement);
                }
                break;

            case 4:
                item = get_nth_child(array, idx);
                if (item != NULL) {
                    detached = cJSON_DetachItemViaPointer(array, item);
                    if (detached != NULL) {
                        if (!cJSON_AddItemToArray(array, detached)) {
                            cJSON_Delete(detached);
                        }
                        detached = NULL;
                    }
                }
                break;

            case 5:
                item = get_nth_child(array, idx);
                if (item != NULL) {
                    cJSON_ReplaceItemViaPointer(array, item, item);
                }
                break;

            case 6:
                cJSON_IsObject(object);
                cJSON_IsObject(array);
                cJSON_IsObject(parsed);
                item = get_nth_child(array, idx);
                cJSON_IsObject(item);
                break;

            case 7:
                tmp = make_item_from_bytes(Data, Size, &offset, 2);
                if (tmp != NULL) {
                    char keybuf[32];
                    snprintf(keybuf, sizeof(keybuf), "k%d", idx);
                    if (!cJSON_AddItemToObject(object, keybuf, tmp)) {
                        cJSON_Delete(tmp);
                    }
                }
                cJSON_IsObject(object);
                break;

            case 8:
                item = get_nth_child(array, idx);
                if (item != NULL) {
                    detached = cJSON_DetachItemViaPointer(array, item);
                    if (detached != NULL) {
                        cJSON_Delete(detached);
                        detached = NULL;
                    }
                }
                break;

            case 9:
            default:
                item = make_item_from_bytes(Data, Size, &offset, 2);
                if (item != NULL) {
                    if (!cJSON_AddItemToArray(array, item)) {
                        cJSON_Delete(item);
                    }
                }
                break;
        }
    }

    cJSON_Delete(parsed);
    cJSON_Delete(object);
    cJSON_Delete(array);
    return 0;
}