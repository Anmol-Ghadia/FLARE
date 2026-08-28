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

static char *read_string(const uint8_t *data, size_t size, size_t *offset) {
    size_t remaining, len;
    char *out;

    if (*offset >= size) {
        out = (char *)malloc(1);
        if (out != NULL) {
            out[0] = '\0';
        }
        return out;
    }

    remaining = size - *offset;
    len = remaining ? (size_t)(data[(*offset)++] % (remaining + 1)) : 0;
    if (len > size - *offset) {
        len = size - *offset;
    }

    out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }

    memcpy(out, data + *offset, len);
    out[len] = '\0';
    *offset += len;
    return out;
}

static cJSON *make_item_from_byte(uint8_t b) {
    switch (b % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((b & 1) ? 1 : 0);
        case 2:
            return cJSON_CreateNumber((double)((int8_t)b));
        case 3:
            return cJSON_CreateString("fuzz");
        case 4:
            return cJSON_CreateArray();
        case 5:
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *extra_array = NULL;
    cJSON *invalid = NULL;
    cJSON *bool_item = NULL;
    char *name1 = NULL;
    char *name2 = NULL;
    int idx1, idx2, idx3;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    name1 = read_string(Data, Size, &offset);
    name2 = read_string(Data, Size, &offset);
    if (name1 == NULL || name2 == NULL) {
        free(name1);
        free(name2);
        cJSON_Delete(root);
        return 0;
    }

    extra_array = cJSON_AddArrayToObject(root, name1);
    if (extra_array != NULL) {
        (void)cJSON_IsArray(extra_array);
    }
    (void)cJSON_IsArray(root);
    (void)cJSON_IsArray(NULL);

    bool_item = cJSON_AddBoolToObject(root, name2, (offset < Size) ? (Data[offset++] & 1) : 0);
    (void)bool_item;

    array = cJSON_AddArrayToObject(root, "array");
    if (array == NULL) {
        free(name1);
        free(name2);
        cJSON_Delete(root);
        return 0;
    }

    (void)cJSON_IsArray(array);

    idx1 = read_int(Data, Size, &offset);
    idx2 = read_int(Data, Size, &offset);
    idx3 = read_int(Data, Size, &offset);

    {
        cJSON *item1 = (offset < Size) ? make_item_from_byte(Data[offset++]) : cJSON_CreateNull();
        if (item1 != NULL) {
            if (!cJSON_InsertItemInArray(array, idx1, item1)) {
                cJSON_Delete(item1);
            }
        }
    }

    {
        cJSON *item2 = (offset < Size) ? make_item_from_byte(Data[offset++]) : cJSON_CreateBool(1);
        if (item2 != NULL) {
            if (!cJSON_InsertItemInArray(array, idx2, item2)) {
                cJSON_Delete(item2);
            }
        }
    }

    {
        cJSON *item3 = (offset < Size) ? make_item_from_byte(Data[offset++]) : cJSON_CreateNumber(0);
        if (item3 != NULL) {
            if (!cJSON_InsertItemInArray(array, idx3, item3)) {
                cJSON_Delete(item3);
            }
        }
    }

    if (offset < Size) {
        cJSON *bad_item = make_item_from_byte(Data[offset++]);
        if (bad_item != NULL) {
            if (!cJSON_InsertItemInArray(root, idx1, bad_item)) {
                cJSON_Delete(bad_item);
            }
        }
    }

    (void)cJSON_IsInvalid(root);
    (void)cJSON_IsInvalid(array);
    (void)cJSON_IsInvalid(extra_array);
    (void)cJSON_IsInvalid(NULL);

    cJSON_DeleteItemFromArray(array, idx1);
    cJSON_DeleteItemFromArray(array, idx2);
    cJSON_DeleteItemFromArray(array, -1);

    invalid = cJSON_CreateNull();
    if (invalid != NULL) {
        invalid->type = cJSON_Invalid;
    }

    (void)cJSON_IsInvalid(invalid);
    (void)cJSON_IsArray(invalid);

    free(name1);
    free(name2);
    cJSON_Delete(invalid);
    cJSON_Delete(root);
    return 0;
}