#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "cJSON.h"

static uint16_t read_u16(const uint8_t *data, size_t size, size_t *offset) {
    uint16_t v = 0;
    if (*offset < size) {
        v = (uint16_t)data[*offset];
        (*offset)++;
    }
    if (*offset < size) {
        v |= (uint16_t)data[*offset] << 8;
        (*offset)++;
    }
    return v;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *offset < size; i++, (*offset)++) {
        v |= ((uint64_t)data[*offset]) << (8 * i);
    }
    return v;
}

static double read_double(const uint8_t *data, size_t size, size_t *offset) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = read_u64(data, size, offset);
    return conv.d;
}

static char *make_cstring(const uint8_t *data, size_t size, size_t *offset) {
    uint16_t len = read_u16(data, size, offset);
    size_t remaining = (*offset < size) ? (size - *offset) : 0;
    size_t actual = len;
    char *out;

    if (actual > remaining) {
        actual = remaining;
    }

    out = (char *)malloc(actual + 1);
    if (out == NULL) {
        return NULL;
    }

    if (actual > 0) {
        memcpy(out, data + *offset, actual);
        *offset += actual;
    }
    out[actual] = '\0';
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *root = NULL;
    cJSON *num = NULL;
    cJSON *arr = NULL;
    cJSON *added = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *str1 = NULL;
    char *str2 = NULL;
    double d1, d2, d3;
    double r;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    key1 = make_cstring(Data, Size, &offset);
    key2 = make_cstring(Data, Size, &offset);
    key3 = make_cstring(Data, Size, &offset);
    str1 = make_cstring(Data, Size, &offset);
    str2 = make_cstring(Data, Size, &offset);

    d1 = read_double(Data, Size, &offset);
    d2 = read_double(Data, Size, &offset);
    d3 = read_double(Data, Size, &offset);

    root = cJSON_CreateObject();
    if (root == NULL) {
        free(key1);
        free(key2);
        free(key3);
        free(str1);
        free(str2);
        return 0;
    }

    num = cJSON_CreateNumber(d1);
    if (num != NULL) {
        r = cJSON_GetNumberValue(num);
        (void)r;

        r = cJSON_SetNumberHelper(num, d2);
        (void)r;

        r = cJSON_GetNumberValue(num);
        (void)r;

        cJSON_Delete(num);
        num = NULL;
    }

    added = cJSON_AddNumberToObject(root, key1 ? key1 : "", d1);
    if (added != NULL) {
        r = cJSON_GetNumberValue(added);
        (void)r;

        r = cJSON_SetNumberHelper(added, d3);
        (void)r;

        r = cJSON_GetNumberValue(added);
        (void)r;
    }

    added = cJSON_AddStringToObject(root, key2 ? key2 : "", str1 ? str1 : "");
    if (added != NULL) {
        r = cJSON_GetNumberValue(added);
        (void)r;

        r = cJSON_SetNumberHelper(added, d2);
        (void)r;
    }

    arr = cJSON_AddArrayToObject(root, key3 ? key3 : "");
    if (arr != NULL) {
        cJSON *elem1 = cJSON_CreateNumber(d2);
        cJSON *elem2 = cJSON_CreateNumber(d3);

        if (elem1 != NULL) {
            cJSON_AddItemToArray(arr, elem1);
            r = cJSON_GetNumberValue(elem1);
            (void)r;
            r = cJSON_SetNumberHelper(elem1, d1);
            (void)r;
        }

        if (elem2 != NULL) {
            cJSON_AddItemToArray(arr, elem2);
            r = cJSON_GetNumberValue(elem2);
            (void)r;
        }

        if (str2 != NULL) {
            cJSON_AddItemToArray(arr, cJSON_CreateString(str2));
        }
    }

    r = cJSON_GetNumberValue(NULL);
    (void)r;
    r = cJSON_SetNumberHelper(NULL, d1);
    (void)r;

    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(key3);
    free(str1);
    free(str2);
    return 0;
}