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

static int read_int32(const uint8_t *data, size_t size, size_t *offset) {
    int value = 0;
    size_t i;
    for (i = 0; i < 4 && *offset < size; ++i, ++(*offset)) {
        value = (value << 8) | data[*offset];
    }
    return value;
}

static char *make_cstring(const uint8_t *data, size_t len) {
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *obj1 = NULL;
    cJSON *arr = NULL;
    cJSON *obj2 = NULL;
    cJSON *str1 = NULL;
    cJSON *obj3 = NULL;
    cJSON *str2 = NULL;
    cJSON *item = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *val1 = NULL;
    char *key3 = NULL;
    char *val2 = NULL;
    int *numbers = NULL;
    int count = 0;
    int index = 0;
    size_t i;

    if (Size == 0) {
        return 0;
    }

    count = (int)(Data[offset++] % 32);
    if (count > 0) {
        numbers = (int *)malloc((size_t)count * sizeof(int));
        if (numbers == NULL) {
            return 0;
        }
        for (i = 0; i < (size_t)count; ++i) {
            numbers[i] = read_int32(Data, Size, &offset);
        }
    }

    if (offset < Size) {
        index = (int)Data[offset++];
        if (count > 0 && (offset < Size) && (Data[offset++] & 1)) {
            index %= count;
        }
    }

    {
        size_t rem = (offset < Size) ? (Size - offset) : 0;
        size_t l1 = rem ? (Data[offset++] % (rem + 1)) : 0;
        rem = (offset < Size) ? (Size - offset) : 0;
        if (l1 > rem) l1 = rem;
        key1 = make_cstring(Data + offset, l1);
        offset += l1;

        rem = (offset < Size) ? (Size - offset) : 0;
        size_t l2 = rem ? (Data[offset++] % (rem + 1)) : 0;
        rem = (offset < Size) ? (Size - offset) : 0;
        if (l2 > rem) l2 = rem;
        key2 = make_cstring(Data + offset, l2);
        offset += l2;

        rem = (offset < Size) ? (Size - offset) : 0;
        size_t l3 = rem ? (Data[offset++] % (rem + 1)) : 0;
        rem = (offset < Size) ? (Size - offset) : 0;
        if (l3 > rem) l3 = rem;
        val1 = make_cstring(Data + offset, l3);
        offset += l3;

        rem = (offset < Size) ? (Size - offset) : 0;
        size_t l4 = rem ? (Data[offset++] % (rem + 1)) : 0;
        rem = (offset < Size) ? (Size - offset) : 0;
        if (l4 > rem) l4 = rem;
        key3 = make_cstring(Data + offset, l4);
        offset += l4;

        rem = (offset < Size) ? (Size - offset) : 0;
        size_t l5 = rem ? (Data[offset++] % (rem + 1)) : 0;
        rem = (offset < Size) ? (Size - offset) : 0;
        if (l5 > rem) l5 = rem;
        val2 = make_cstring(Data + offset, l5);
        offset += l5;
    }

    obj1 = cJSON_CreateObject();
    arr = cJSON_CreateIntArray(numbers, count);
    item = cJSON_GetArrayItem(arr, index);
    (void)item;
    if (obj1 != NULL && arr != NULL) {
        if (!cJSON_AddItemToObject(obj1, key1 ? key1 : "", arr)) {
            cJSON_Delete(arr);
            arr = NULL;
        } else {
            arr = NULL;
        }
    } else if (arr != NULL) {
        cJSON_Delete(arr);
        arr = NULL;
    }

    obj2 = cJSON_CreateObject();
    str1 = cJSON_CreateString(val1 ? val1 : "");
    if (obj2 != NULL && str1 != NULL) {
        if (!cJSON_AddItemToObject(obj2, key2 ? key2 : "", str1)) {
            cJSON_Delete(str1);
            str1 = NULL;
        } else {
            str1 = NULL;
        }
    } else if (str1 != NULL) {
        cJSON_Delete(str1);
        str1 = NULL;
    }

    obj3 = cJSON_CreateObject();
    str2 = cJSON_CreateString(val2 ? val2 : "");
    if (obj3 != NULL && str2 != NULL) {
        if (!cJSON_AddItemToObject(obj3, key3 ? key3 : "", str2)) {
            cJSON_Delete(str2);
            str2 = NULL;
        } else {
            str2 = NULL;
        }
    } else if (str2 != NULL) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    cJSON_Delete(obj3);
    cJSON_Delete(obj2);
    cJSON_Delete(obj1);

    free(numbers);
    free(key1);
    free(key2);
    free(val1);
    free(key3);
    free(val2);

    return 0;
}