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

static char *make_cstring(const uint8_t *data, size_t size) {
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
    cJSON *root = NULL;
    cJSON *dup = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON *item5 = NULL;
    char *json_buf = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *key4 = NULL;

    size_t o1 = 0, l1 = 0;
    size_t o2 = 0, l2 = 0;
    size_t o3 = 0, l3 = 0;
    size_t o4 = 0, l4 = 0;
    cJSON_bool recurse = 1;
    cJSON_bool case_sensitive = 1;

    json_buf = make_cstring(Data, Size);
    if (json_buf == NULL) {
        return 0;
    }

    root = cJSON_Parse(json_buf);
    if (root == NULL) {
        root = cJSON_CreateObject();
        if (root == NULL) {
            free(json_buf);
            return 0;
        }
    }

    if (Size > 0) {
        o1 = 0;
        l1 = Data[0] % (Size + 1);

        if (Size > 1) {
            o2 = Data[1] % (Size + 1);
            l2 = (Size - o2 > 0) ? (Data[1] % (Size - o2 + 1)) : 0;
        }

        if (Size > 2) {
            o3 = Data[2] % (Size + 1);
            l3 = (Size - o3 > 0) ? (Data[2] % (Size - o3 + 1)) : 0;
        }

        if (Size > 3) {
            o4 = Data[3] % (Size + 1);
            l4 = (Size - o4 > 0) ? (Data[3] % (Size - o4 + 1)) : 0;
        }

        recurse = (Size > 4) ? (Data[4] & 1) : 1;
        case_sensitive = (Size > 5) ? (Data[5] & 1) : 1;
    }

    key1 = make_cstring(Data + o1, l1);
    key2 = make_cstring(Data + o2, l2);
    key3 = make_cstring(Data + o3, l3);
    key4 = make_cstring(Data + o4, l4);

    if (key1 == NULL || key2 == NULL || key3 == NULL || key4 == NULL) {
        free(key1);
        free(key2);
        free(key3);
        free(key4);
        cJSON_Delete(root);
        free(json_buf);
        return 0;
    }

    item1 = cJSON_GetObjectItemCaseSensitive(root, key1);
    (void)cJSON_IsString(item1);

    item2 = cJSON_GetObjectItemCaseSensitive(root, key2);
    (void)cJSON_IsTrue(item2);

    item3 = cJSON_GetObjectItemCaseSensitive(root, key3);
    item4 = cJSON_GetObjectItemCaseSensitive(root, key4);

    dup = cJSON_Duplicate(root, recurse);

    item5 = cJSON_GetObjectItemCaseSensitive(dup, key1);
    {
        cJSON *item6 = cJSON_GetObjectItemCaseSensitive(dup, key2);
        (void)item3;
        (void)item4;
        (void)item5;
        (void)item6;
    }

    (void)cJSON_Compare(root, dup, case_sensitive);

    cJSON_Delete(dup);
    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(key3);
    free(key4);
    free(json_buf);
    return 0;
}