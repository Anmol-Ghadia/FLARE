// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2621:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1941:23 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "cJSON.h"

static int read_int32(const uint8_t *data, size_t size, size_t *offset)
{
    int value = 0;
    size_t i;

    for (i = 0; i < 4; i++) {
        value <<= 8;
        if (*offset < size) {
            value |= data[*offset];
            (*offset)++;
        }
    }

    return value;
}

static char *make_string_from_bytes(const uint8_t *data, size_t len)
{
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(s, data, len);
    }
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size)
{
    size_t offset = 0;
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *obj2 = NULL;
    cJSON *obj3 = NULL;
    cJSON *str1 = NULL;
    cJSON *str2 = NULL;
    cJSON *item = NULL;
    cJSON *item_copy = NULL;
    int *numbers = NULL;
    int count = 0;
    int index = 0;
    char *key1 = NULL;
    char *val1 = NULL;
    char *key2 = NULL;
    char *val2 = NULL;
    size_t key1_len, val1_len, key2_len, val2_len;
    cJSON_bool added;

    root = cJSON_CreateObject();
    if (root == NULL) {
        goto cleanup;
    }

    if (Size > 0) {
        count = (int)(Data[offset] % 32);
        offset++;
    }

    if (count > 0) {
        int i;
        numbers = (int *)malloc((size_t)count * sizeof(int));
        if (numbers == NULL) {
            count = 0;
        } else {
            for (i = 0; i < count; i++) {
                numbers[i] = read_int32(Data, Size, &offset);
            }
        }
    }

    array = cJSON_CreateIntArray(numbers, count);

    if (Size > offset) {
        index = (int)((int8_t)Data[offset]);
        offset++;
    }

    item = cJSON_GetArrayItem(array, index);

    key1_len = (Size > offset) ? (Data[offset] % 16) : 0;
    if (Size > offset) {
        offset++;
    }
    if (key1_len > Size - offset) {
        key1_len = Size - offset;
    }
    key1 = make_string_from_bytes(Data + offset, key1_len);
    offset += key1_len;

    if (item != NULL) {
        item_copy = cJSON_Duplicate(item, 1);
        if (item_copy != NULL) {
            added = cJSON_AddItemToObject(root, key1 != NULL ? key1 : "", item_copy);
            if (added) {
                item_copy = NULL;
            }
        }
    }

    obj2 = cJSON_CreateObject();

    val1_len = (Size > offset) ? (Data[offset] % 32) : 0;
    if (Size > offset) {
        offset++;
    }
    if (val1_len > Size - offset) {
        val1_len = Size - offset;
    }
    val1 = make_string_from_bytes(Data + offset, val1_len);
    offset += val1_len;

    str1 = cJSON_CreateString(val1 != NULL ? val1 : "");

    key2_len = (Size > offset) ? (Data[offset] % 16) : 0;
    if (Size > offset) {
        offset++;
    }
    if (key2_len > Size - offset) {
        key2_len = Size - offset;
    }
    key2 = make_string_from_bytes(Data + offset, key2_len);
    offset += key2_len;

    if (obj2 != NULL && str1 != NULL) {
        added = cJSON_AddItemToObject(obj2, key2 != NULL ? key2 : "", str1);
        if (added) {
            str1 = NULL;
        }
    }

    obj3 = cJSON_CreateObject();

    val2_len = (Size > offset) ? (Data[offset] % 32) : 0;
    if (Size > offset) {
        offset++;
    }
    if (val2_len > Size - offset) {
        val2_len = Size - offset;
    }
    val2 = make_string_from_bytes(Data + offset, val2_len);
    offset += val2_len;

    str2 = cJSON_CreateString(val2 != NULL ? val2 : "");

    if (obj3 != NULL && str2 != NULL) {
        added = cJSON_AddItemToObject(obj3, "fixed", str2);
        if (added) {
            str2 = NULL;
        }
    }

cleanup:
    cJSON_Delete(item_copy);
    cJSON_Delete(obj3);
    cJSON_Delete(obj2);
    cJSON_Delete(root);
    cJSON_Delete(array);
    cJSON_Delete(str1);
    cJSON_Delete(str2);

    free(numbers);
    free(key1);
    free(val1);
    free(key2);
    free(val2);

    return 0;
}