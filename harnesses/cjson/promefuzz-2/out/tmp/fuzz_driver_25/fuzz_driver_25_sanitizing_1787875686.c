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

static int consume_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(int)) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static cJSON *make_manual_node(const uint8_t *data, size_t size)
{
    cJSON *item = (cJSON *)calloc(1, sizeof(cJSON));
    if (item == NULL) {
        return NULL;
    }

    if (size > 0) {
        switch (data[0] % 6) {
            case 0:
                item->type = cJSON_True;
                break;
            case 1:
                item->type = cJSON_False;
                break;
            case 2:
                item->type = cJSON_Raw;
                break;
            case 3:
                item->type = cJSON_NULL;
                break;
            case 4:
                item->type = cJSON_Number;
                break;
            default:
                item->type = (int)data[0];
                break;
        }
    }

    if (size > 1) {
        item->type |= ((int)data[1] << 8);
    }

    if (size > 2) {
        size_t len = size - 2;
        item->valuestring = (char *)malloc(len + 1);
        if (item->valuestring != NULL) {
            memcpy(item->valuestring, data + 2, len);
            item->valuestring[len] = '\0';
        }
    }

    return item;
}

static void free_manual_node(cJSON *item)
{
    if (item == NULL) {
        return;
    }
    free(item->valuestring);
    free(item->string);
    free(item);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (remaining > 0) {
            fwrite(ptr, 1, remaining, fp);
        }
        fclose(fp);
    }

    cJSON_IsBool(NULL);
    cJSON_IsRaw(NULL);
    cJSON_IsTrue(NULL);
    cJSON_IsFalse(NULL);

    cJSON *created_false = cJSON_CreateFalse();
    if (created_false != NULL) {
        cJSON_IsBool(created_false);
        cJSON_IsRaw(created_false);
        cJSON_IsTrue(created_false);
        cJSON_IsFalse(created_false);
        cJSON_Delete(created_false);
    }

    cJSON *created_bool_0 = cJSON_CreateBool(0);
    if (created_bool_0 != NULL) {
        cJSON_IsBool(created_bool_0);
        cJSON_IsRaw(created_bool_0);
        cJSON_IsTrue(created_bool_0);
        cJSON_IsFalse(created_bool_0);
        cJSON_Delete(created_bool_0);
    }

    cJSON *created_bool_1 = cJSON_CreateBool(1);
    if (created_bool_1 != NULL) {
        cJSON_IsBool(created_bool_1);
        cJSON_IsRaw(created_bool_1);
        cJSON_IsTrue(created_bool_1);
        cJSON_IsFalse(created_bool_1);
        cJSON_Delete(created_bool_1);
    }

    if (remaining > 0) {
        int fuzz_bool = consume_int(&ptr, &remaining);
        cJSON *dynamic_bool = cJSON_CreateBool((cJSON_bool)fuzz_bool);
        if (dynamic_bool != NULL) {
            cJSON_IsBool(dynamic_bool);
            cJSON_IsRaw(dynamic_bool);
            cJSON_IsTrue(dynamic_bool);
            cJSON_IsFalse(dynamic_bool);
            cJSON_Delete(dynamic_bool);
        }
    }

    cJSON *manual = make_manual_node(ptr, remaining);
    if (manual != NULL) {
        cJSON_IsBool(manual);
        cJSON_IsRaw(manual);
        cJSON_IsTrue(manual);
        cJSON_IsFalse(manual);
        free_manual_node(manual);
    }

    if (Size > 0) {
        cJSON *parsed = cJSON_ParseWithLength((const char *)Data, Size);
        if (parsed != NULL) {
            cJSON_IsBool(parsed);
            cJSON_IsRaw(parsed);
            cJSON_IsTrue(parsed);
            cJSON_IsFalse(parsed);

            cJSON *child = parsed->child;
            while (child != NULL) {
                cJSON_IsBool(child);
                cJSON_IsRaw(child);
                cJSON_IsTrue(child);
                cJSON_IsFalse(child);
                child = child->next;
            }

            cJSON_Delete(parsed);
        }
    }

    return 0;
}