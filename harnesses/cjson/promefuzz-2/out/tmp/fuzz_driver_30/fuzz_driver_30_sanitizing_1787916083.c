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

static char *make_cstring(const uint8_t *data, size_t size)
{
    char *out = (char *)malloc(size + 1);
    if (out == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(out, data, size);
    }
    out[size] = '\0';
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    if (Data == NULL) {
        return 0;
    }

    size_t split1 = (Size > 0) ? (size_t)(Data[0] % (Size + 1)) : 0;
    size_t split2 = (Size > 1) ? (size_t)(Data[1] % (Size + 1)) : 0;

    if (split2 < split1) {
        size_t tmp = split1;
        split1 = split2;
        split2 = tmp;
    }

    char *buf_all = make_cstring(Data, Size);
    char *buf_name = make_cstring(Data, split1);
    char *buf_value = make_cstring(Data + split1, (split2 > split1) ? (split2 - split1) : 0);
    char *buf_extra = make_cstring(Data + split2, (Size > split2) ? (Size - split2) : 0);

    if (buf_all == NULL || buf_name == NULL || buf_value == NULL || buf_extra == NULL) {
        free(buf_all);
        free(buf_name);
        free(buf_value);
        free(buf_extra);
        return 0;
    }

    cJSON *obj = cJSON_CreateObject();
    if (obj != NULL) {
        cJSON *added = cJSON_AddStringToObject(obj, buf_name, buf_value);
        if (added != NULL) {
            (void)cJSON_GetStringValue(added);
            (void)cJSON_SetValuestring(added, buf_extra);
            (void)cJSON_GetStringValue(added);
        }

        cJSON *found_exact = cJSON_GetObjectItemCaseSensitive(obj, buf_name);
        if (found_exact != NULL) {
            (void)cJSON_GetStringValue(found_exact);
            (void)cJSON_SetValuestring(found_exact, buf_all);
        }

        (void)cJSON_GetObjectItemCaseSensitive(obj, buf_extra);
        (void)cJSON_GetObjectItemCaseSensitive(NULL, buf_name);
        (void)cJSON_GetObjectItemCaseSensitive(obj, NULL);
        (void)cJSON_GetStringValue(obj);
    }

    cJSON *str_item = cJSON_CreateString(buf_value);
    if (str_item != NULL) {
        (void)cJSON_GetStringValue(str_item);
        (void)cJSON_SetValuestring(str_item, buf_name);
        (void)cJSON_GetStringValue(str_item);
    }

    cJSON *ref_item = cJSON_CreateStringReference(buf_extra);
    if (ref_item != NULL) {
        (void)cJSON_GetStringValue(ref_item);
        (void)cJSON_SetValuestring(ref_item, buf_name);
        (void)cJSON_GetStringValue(ref_item);
    }

    cJSON *null_ref_item = cJSON_CreateStringReference(NULL);
    if (null_ref_item != NULL) {
        (void)cJSON_GetStringValue(null_ref_item);
        (void)cJSON_SetValuestring(null_ref_item, buf_name);
    }

    cJSON *null_str_item = cJSON_CreateString(NULL);
    if (null_str_item != NULL) {
        (void)cJSON_GetStringValue(null_str_item);
        (void)cJSON_SetValuestring(null_str_item, buf_value);
    }

    if (str_item != NULL && str_item->valuestring != NULL) {
        (void)cJSON_SetValuestring(str_item, str_item->valuestring);
    }

    cJSON_Delete(obj);
    cJSON_Delete(str_item);
    cJSON_Delete(ref_item);
    cJSON_Delete(null_ref_item);
    cJSON_Delete(null_str_item);

    free(buf_all);
    free(buf_name);
    free(buf_value);
    free(buf_extra);
    return 0;
}