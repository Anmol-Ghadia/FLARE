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

static char *dup_data_as_cstring(const uint8_t *data, size_t size) {
    char *s = (char *)malloc(size + 1);
    if (s == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(s, data, size);
    }
    s[size] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *buf1 = NULL;
    char *buf2 = NULL;
    char *buf3 = NULL;
    char *key = NULL;

    cJSON *str1 = NULL;
    cJSON *array = NULL;
    cJSON *str2 = NULL;
    cJSON *str3 = NULL;
    cJSON *str4 = NULL;
    cJSON *object_item = NULL;

    cJSON_Hooks hooks;

    size_t q1, q2, q3;
    size_t s1, s2, s3, s4;

    q1 = Size / 4;
    q2 = Size / 2;
    q3 = (Size * 3) / 4;

    s1 = q1;
    s2 = (q2 > q1) ? (q2 - q1) : 0;
    s3 = (q3 > q2) ? (q3 - q2) : 0;
    s4 = (Size > q3) ? (Size - q3) : 0;

    buf1 = dup_data_as_cstring(Data, s1);
    buf2 = dup_data_as_cstring(Data + q1, s2);
    buf3 = dup_data_as_cstring(Data + q2, s3);
    key = dup_data_as_cstring(Data + q3, s4);

    if ((s1 && !buf1) || (s2 && !buf2) || (s3 && !buf3) || (s4 && !key)) {
        free(buf1);
        free(buf2);
        free(buf3);
        free(key);
        return 0;
    }

    /* Required call order begins here */
    str1 = cJSON_CreateString(buf1 ? buf1 : "");
    array = cJSON_CreateArray();
    str2 = cJSON_CreateString(buf2 ? buf2 : "");
    str3 = cJSON_CreateString(buf3 ? buf3 : "");
    str4 = cJSON_CreateString(key ? key : "");

    if ((Size & 1) == 0) {
        hooks.malloc_fn = NULL;
        hooks.free_fn = NULL;
        cJSON_InitHooks(&hooks);
    } else {
        cJSON_InitHooks(NULL);
    }

    cJSON_Delete(str1);
    str1 = NULL;

    (void)cJSON_GetArraySize(array);

    if (!cJSON_AddItemToArray(array, str2)) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    if (!cJSON_AddItemToArray(array, str3)) {
        cJSON_Delete(str3);
        str3 = NULL;
    }

    object_item = cJSON_CreateArray();
    if (object_item != NULL) {
        if (!cJSON_AddItemToObject(object_item, key ? key : "", str4)) {
            cJSON_Delete(str4);
            str4 = NULL;
        }
        cJSON_Delete(object_item);
        object_item = NULL;
    } else {
        cJSON_Delete(str4);
        str4 = NULL;
        (void)cJSON_AddItemToObject(NULL, key ? key : "", NULL);
    }
    /* Required call order ends here */

    cJSON_Delete(array);

    free(buf1);
    free(buf2);
    free(buf3);
    free(key);

    return 0;
}