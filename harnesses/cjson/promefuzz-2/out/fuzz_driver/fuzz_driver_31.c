// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2579:23 in cJSON.h
// cJSON_CreateStringArray at cJSON.c:2699:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_CreateObjectReference at cJSON.c:2518:23 in cJSON.h
// cJSON_CreateObjectReference at cJSON.c:2518:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
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

static uint32_t consume_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int consume_int_bounded(const uint8_t **data, size_t *size, int max_value) {
    if (max_value <= 0) {
        return 0;
    }
    return (int)(consume_u32(data, size) % (uint32_t)max_value);
}

static char *consume_cstring(const uint8_t **data, size_t *size, size_t max_len) {
    size_t len = 0;
    if (*size > 0) {
        len = (*data)[0] % (max_len + 1);
        (*data)++;
        (*size)--;
    }
    if (len > *size) {
        len = *size;
    }

    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (*data)[i];
        if (c == '\0') {
            c = 'A';
        }
        out[i] = (char)c;
    }
    out[len] = '\0';

    *data += len;
    *size -= len;
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    enum { MAX_INTS = 32, MAX_STRS = 16 };
    int numbers[MAX_INTS];
    const char *string_ptrs[MAX_STRS];
    char *allocated_strings[MAX_STRS];
    memset(allocated_strings, 0, sizeof(allocated_strings));

    int int_count = consume_int_bounded(&ptr, &remaining, MAX_INTS + 1);
    for (int i = 0; i < int_count; ++i) {
        numbers[i] = (int)consume_u32(&ptr, &remaining);
    }

    int str_count = consume_int_bounded(&ptr, &remaining, MAX_STRS + 1);
    for (int i = 0; i < str_count; ++i) {
        allocated_strings[i] = consume_cstring(&ptr, &remaining, 32);
        string_ptrs[i] = allocated_strings[i] ? allocated_strings[i] : "";
    }

    char *key1 = consume_cstring(&ptr, &remaining, 32);
    char *key2 = consume_cstring(&ptr, &remaining, 32);
    char *key3 = consume_cstring(&ptr, &remaining, 32);

    cJSON *root = cJSON_CreateObject();
    cJSON *array_holder = cJSON_CreateObject();
    cJSON *array1 = NULL;
    cJSON *int_array = NULL;
    cJSON *str_array = NULL;
    cJSON *dup_shallow = NULL;
    cJSON *dup_deep = NULL;
    cJSON *ref_obj = NULL;
    cJSON *ref_arr = NULL;

    if (root != NULL) {
        array1 = cJSON_AddArrayToObject(root, key1 ? key1 : "a");
        (void)cJSON_AddArrayToObject(root, key2 ? key2 : "b");
    }

    int_array = cJSON_CreateIntArray(numbers, int_count);
    str_array = cJSON_CreateStringArray(string_ptrs, str_count);

    if (array1 != NULL && int_array != NULL) {
        cJSON *dup_for_insert = cJSON_Duplicate(int_array, 1);
        if (dup_for_insert != NULL) {
            (void)cJSON_AddItemToArray(array1, dup_for_insert);
        } else {
            cJSON_Delete(dup_for_insert);
        }
    }

    if (array1 != NULL && str_array != NULL) {
        cJSON *dup_for_insert = cJSON_Duplicate(str_array, 1);
        if (dup_for_insert != NULL) {
            (void)cJSON_AddItemToArray(array1, dup_for_insert);
        } else {
            cJSON_Delete(dup_for_insert);
        }
    }

    if (array_holder != NULL) {
        cJSON *nested = cJSON_AddArrayToObject(array_holder, key3 ? key3 : "nested");
        if (nested != NULL) {
            cJSON *num_item = cJSON_CreateNumber((double)(consume_u32(&ptr, &remaining)));
            if (num_item != NULL) {
                (void)cJSON_AddItemToArray(nested, num_item);
            }
        }
    }

    dup_shallow = cJSON_Duplicate(root, 0);
    dup_deep = cJSON_Duplicate(root, 1);

    if (root != NULL) {
        ref_obj = cJSON_CreateObjectReference(root->child);
    }
    if (array1 != NULL) {
        ref_arr = cJSON_CreateObjectReference(array1);
    }

    if (dup_deep != NULL && array1 != NULL) {
        cJSON *dup_arr = cJSON_Duplicate(array1, consume_int_bounded(&ptr, &remaining, 2));
        if (dup_arr != NULL) {
            cJSON *target = cJSON_AddArrayToObject(dup_deep, "extra");
            if (target != NULL) {
                (void)cJSON_AddItemToArray(target, dup_arr);
            } else {
                cJSON_Delete(dup_arr);
            }
        }
    }

    cJSON_Delete(ref_arr);
    cJSON_Delete(ref_obj);
    cJSON_Delete(dup_deep);
    cJSON_Delete(dup_shallow);
    cJSON_Delete(str_array);
    cJSON_Delete(int_array);
    cJSON_Delete(array_holder);
    cJSON_Delete(root);

    free(key1);
    free(key2);
    free(key3);

    for (int i = 0; i < str_count; ++i) {
        free(allocated_strings[i]);
    }

    return 0;
}