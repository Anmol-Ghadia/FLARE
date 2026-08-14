// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateIntArray at cJSON.c:2579:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateStringArray at cJSON.c:2699:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2180:22 in cJSON.h
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

static int consume_u32(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        value = (value << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return value;
}

static size_t consume_size(const uint8_t **data, size_t *size, size_t max_value)
{
    if (max_value == 0) {
        return 0;
    }
    unsigned int v = (unsigned int)consume_u32(data, size);
    return (size_t)(v % (max_value + 1));
}

static char *make_cstring(const uint8_t **data, size_t *size, size_t max_len)
{
    size_t len = consume_size(data, size, max_len);
    if (len > *size) {
        len = *size;
    }

    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(out, *data, len);
        *data += len;
        *size -= len;
    }
    out[len] = '\0';
    return out;
}

int LLVMFuzzerTestOneInput_28(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    char *string_value = make_cstring(&ptr, &remaining, 256);
    char *object_key1 = make_cstring(&ptr, &remaining, 64);
    char *object_key2 = make_cstring(&ptr, &remaining, 64);
    char *raw_value = make_cstring(&ptr, &remaining, 256);

    if (string_value == NULL || object_key1 == NULL || object_key2 == NULL || raw_value == NULL) {
        free(string_value);
        free(object_key1);
        free(object_key2);
        free(raw_value);
        return 0;
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        free(string_value);
        free(object_key1);
        free(object_key2);
        free(raw_value);
        return 0;
    }

    cJSON *str_item = cJSON_CreateString(string_value);
    if (str_item != NULL) {
        if (!cJSON_AddItemToObject(root, object_key1, str_item)) {
            cJSON_Delete(str_item);
        }
    }

    (void)cJSON_AddRawToObject(root, object_key2, raw_value);

    int numbers[32];
    size_t num_count = consume_size(&ptr, &remaining, 32);
    for (size_t i = 0; i < num_count; ++i) {
        if (remaining > 0) {
            numbers[i] = (int)(int8_t)(*ptr);
            ptr++;
            remaining--;
        } else {
            numbers[i] = 0;
        }
    }

    cJSON *int_array = cJSON_CreateIntArray(numbers, (int)num_count);
    if (int_array != NULL) {
        if (!cJSON_AddItemToObject(root, "int_array", int_array)) {
            cJSON_Delete(int_array);
        }
    }

    const char *string_ptrs[16];
    char *owned_strings[16];
    memset(string_ptrs, 0, sizeof(string_ptrs));
    memset(owned_strings, 0, sizeof(owned_strings));

    size_t str_count = consume_size(&ptr, &remaining, 16);
    for (size_t i = 0; i < str_count; ++i) {
        owned_strings[i] = make_cstring(&ptr, &remaining, 64);
        if (owned_strings[i] == NULL) {
            str_count = i;
            break;
        }
        string_ptrs[i] = owned_strings[i];
    }

    cJSON *str_array = cJSON_CreateStringArray(string_ptrs, (int)str_count);
    if (str_array != NULL) {
        if (!cJSON_AddItemToObject(root, "str_array", str_array)) {
            cJSON_Delete(str_array);
        }
    }

    size_t buf_len = consume_size(&ptr, &remaining, 1024);
    char *print_buf = NULL;
    if (buf_len > 0) {
        print_buf = (char *)malloc(buf_len);
    }

    cJSON_bool format = (remaining > 0) ? (cJSON_bool)(ptr[0] & 1) : 0;
    if (print_buf != NULL) {
        (void)cJSON_PrintPreallocated(root, print_buf, (int)buf_len, format);
        free(print_buf);
    } else {
        (void)cJSON_PrintPreallocated(root, NULL, (int)buf_len, format);
    }

    cJSON *standalone = cJSON_CreateString(string_value);
    if (standalone != NULL) {
        char small_buf[8];
        (void)cJSON_PrintPreallocated(standalone, small_buf, (int)sizeof(small_buf), format);
        cJSON_Delete(standalone);
    }

    cJSON_Delete(root);

    for (size_t i = 0; i < str_count; ++i) {
        free(owned_strings[i]);
    }
    free(string_value);
    free(object_key1);
    free(object_key2);
    free(raw_value);

    return 0;
}