#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static int consume_u8(const uint8_t **data, size_t *size)
{
    if (*size == 0) {
        return 0;
    }
    {
        int v = **data;
        (*data)++;
        (*size)--;
        return v;
    }
}

static char *make_nul_terminated_copy(const uint8_t *data, size_t size)
{
    char *buf = (char *)malloc(size + 1);
    if (buf == NULL) {
        return NULL;
    }

    if (size > 0) {
        memcpy(buf, data, size);
    }
    buf[size] = '\0';
    return buf;
}

static cJSON *make_manual_item(int selector, const uint8_t *data, size_t size)
{
    cJSON *item = NULL;
    char *tmp = NULL;

    switch (selector % 8) {
        case 0:
            item = cJSON_CreateNull();
            break;
        case 1:
            item = cJSON_CreateNumber((double)(size ? data[0] : 0));
            break;
        case 2:
            item = cJSON_CreateArray();
            if (item != NULL) {
                cJSON_AddItemToArray(item, cJSON_CreateNull());
                cJSON_AddItemToArray(item, cJSON_CreateNumber((double)size));
            }
            break;
        case 3:
            item = cJSON_CreateObject();
            if (item != NULL) {
                cJSON_AddItemToObject(item, "k", cJSON_CreateNumber((double)(size ? data[0] : 0)));
                cJSON_AddItemToObject(item, "n", cJSON_CreateNull());
            }
            break;
        case 4:
            item = cJSON_CreateBool(selector & 1);
            break;
        case 5:
            tmp = make_nul_terminated_copy(data, size);
            if (tmp != NULL) {
                item = cJSON_CreateString(tmp);
                free(tmp);
            }
            break;
        case 6:
            item = cJSON_CreateIntArray((const int[]){1, 2, 3}, 3);
            break;
        default:
            item = cJSON_CreateObject();
            if (item != NULL) {
                cJSON *arr = cJSON_CreateArray();
                if (arr != NULL) {
                    cJSON_AddItemToArray(arr, cJSON_CreateNumber(1.0));
                    cJSON_AddItemToArray(arr, cJSON_CreateNumber(2.0));
                    cJSON_AddItemToObject(item, "arr", arr);
                }
            }
            break;
    }

    return item;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;
    char *input = NULL;

    cJSON *parsed = NULL;
    cJSON *parsed_len = NULL;
    cJSON *dup_shallow = NULL;
    cJSON *dup_deep = NULL;
    cJSON *other = NULL;
    cJSON *other_dup = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    input = make_nul_terminated_copy(Data, Size);
    if (input == NULL) {
        return 0;
    }

    parsed = cJSON_Parse(input);
    parsed_len = cJSON_ParseWithLength(input, Size);

    (void)cJSON_IsArray(NULL);
    (void)cJSON_IsNumber(NULL);
    (void)cJSON_IsNull(NULL);
    (void)cJSON_IsObject(NULL);
    (void)cJSON_Duplicate(NULL, 0);
    (void)cJSON_Duplicate(NULL, 1);
    (void)cJSON_Compare(NULL, NULL, 0);
    (void)cJSON_Compare(NULL, parsed, 1);
    (void)cJSON_Compare(parsed, NULL, 0);

    if (parsed != NULL) {
        (void)cJSON_IsArray(parsed);
        (void)cJSON_IsNumber(parsed);
        (void)cJSON_IsNull(parsed);
        (void)cJSON_IsObject(parsed);

        dup_shallow = cJSON_Duplicate(parsed, 0);
        dup_deep = cJSON_Duplicate(parsed, 1);

        (void)cJSON_Compare(parsed, parsed, 0);
        (void)cJSON_Compare(parsed, parsed, 1);

        if (dup_shallow != NULL) {
            (void)cJSON_IsArray(dup_shallow);
            (void)cJSON_IsNumber(dup_shallow);
            (void)cJSON_IsNull(dup_shallow);
            (void)cJSON_IsObject(dup_shallow);
            (void)cJSON_Compare(parsed, dup_shallow, 0);
            (void)cJSON_Compare(parsed, dup_shallow, 1);
        }

        if (dup_deep != NULL) {
            (void)cJSON_IsArray(dup_deep);
            (void)cJSON_IsNumber(dup_deep);
            (void)cJSON_IsNull(dup_deep);
            (void)cJSON_IsObject(dup_deep);
            (void)cJSON_Compare(parsed, dup_deep, 0);
            (void)cJSON_Compare(parsed, dup_deep, 1);
        }

        if (cJSON_IsArray(parsed)) {
            cJSON *child = parsed->child;
            if (child != NULL) {
                (void)cJSON_IsArray(child);
                (void)cJSON_IsNumber(child);
                (void)cJSON_IsNull(child);
                (void)cJSON_IsObject(child);

                other = cJSON_Duplicate(child, consume_u8(&ptr, &remaining) & 1);
                if (other != NULL) {
                    (void)cJSON_Compare(child, other, 0);
                    (void)cJSON_Compare(child, other, 1);
                }
                cJSON_Delete(other);
                other = NULL;
            }
        }

        if (cJSON_IsObject(parsed)) {
            cJSON *child = parsed->child;
            if (child != NULL) {
                (void)cJSON_IsArray(child);
                (void)cJSON_IsNumber(child);
                (void)cJSON_IsNull(child);
                (void)cJSON_IsObject(child);

                other = cJSON_Duplicate(child, consume_u8(&ptr, &remaining) & 1);
                if (other != NULL) {
                    (void)cJSON_Compare(child, other, 0);
                    (void)cJSON_Compare(child, other, 1);
                }
                cJSON_Delete(other);
                other = NULL;
            }
        }
    }

    if (parsed_len != NULL) {
        (void)cJSON_IsArray(parsed_len);
        (void)cJSON_IsNumber(parsed_len);
        (void)cJSON_IsNull(parsed_len);
        (void)cJSON_IsObject(parsed_len);

        (void)cJSON_Compare(parsed_len, parsed_len, 0);
        (void)cJSON_Compare(parsed_len, parsed_len, 1);

        if (parsed != NULL) {
            (void)cJSON_Compare(parsed, parsed_len, 0);
            (void)cJSON_Compare(parsed, parsed_len, 1);
        }
    }

    other = make_manual_item(consume_u8(&ptr, &remaining), ptr, remaining);
    if (other != NULL) {
        (void)cJSON_IsArray(other);
        (void)cJSON_IsNumber(other);
        (void)cJSON_IsNull(other);
        (void)cJSON_IsObject(other);

        if (parsed != NULL) {
            (void)cJSON_Compare(parsed, other, 0);
            (void)cJSON_Compare(parsed, other, 1);
        }
        if (parsed_len != NULL) {
            (void)cJSON_Compare(parsed_len, other, 0);
            (void)cJSON_Compare(parsed_len, other, 1);
        }

        other_dup = cJSON_Duplicate(other, consume_u8(&ptr, &remaining) & 1);
        if (other_dup != NULL) {
            (void)cJSON_Compare(other, other_dup, 0);
            (void)cJSON_Compare(other, other_dup, 1);
        }
    }

    cJSON_Delete(other_dup);
    cJSON_Delete(other);
    cJSON_Delete(dup_deep);
    cJSON_Delete(dup_shallow);
    cJSON_Delete(parsed_len);
    cJSON_Delete(parsed);
    free(input);

    return 0;
}