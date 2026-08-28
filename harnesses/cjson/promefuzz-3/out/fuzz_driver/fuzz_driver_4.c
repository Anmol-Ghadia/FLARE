// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2506:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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
    char *input = make_cstring(Data, Size);
    if (input == NULL) {
        return 0;
    }

    cJSON *root = cJSON_Parse(input);
    if (root == NULL) {
        root = cJSON_Parse("{}");
    }
    free(input);

    if (root == NULL) {
        return 0;
    }

    if (!(root->type & cJSON_Object)) {
        cJSON_Delete(root);
        root = cJSON_Parse("{}");
        if (root == NULL) {
            return 0;
        }
    }

    size_t part1_len = Size > 0 ? (Size / 3) : 0;
    size_t part2_len = Size > part1_len ? ((Size - part1_len) / 2) : 0;
    size_t part3_off = part1_len + part2_len;
    size_t part3_len = Size > part3_off ? (Size - part3_off) : 0;

    char *val1 = make_cstring(Data, part1_len);
    char *val2 = make_cstring(Data + part1_len, part2_len);
    char *val3 = make_cstring(Data + part3_off, part3_len);

    if (val1 == NULL || val2 == NULL || val3 == NULL) {
        free(val1);
        free(val2);
        free(val3);
        cJSON_Delete(root);
        return 0;
    }

    cJSON *item1 = cJSON_CreateString(val1);
    cJSON *item2 = cJSON_CreateStringReference(val2);

    cJSON_AddItemToObject(root, "k1", item1);
    cJSON_AddItemToObject(root, "k2", item2);

    if (item1 != NULL) {
        cJSON_SetValuestring(item1, val3);
    }

    cJSON *obj1 = cJSON_GetObjectItem(root, "k1");
    cJSON *obj2 = cJSON_GetObjectItem(root, "k2");
    if (obj1 != NULL) {
        cJSON_SetValuestring(obj1, val1);
    }

    cJSON *obj3 = cJSON_GetObjectItem(root, "k1");
    cJSON *obj4 = cJSON_GetObjectItem(root, "k2");
    if (obj2 != NULL) {
        cJSON_SetValuestring(obj2, val2);
    }

    cJSON *obj5 = cJSON_GetObjectItem(root, "k1");
    cJSON *obj6 = cJSON_GetObjectItem(root, "k2");
    (void)obj3;
    (void)obj4;
    (void)obj5;
    (void)obj6;

    free(val1);
    free(val2);
    free(val3);
    cJSON_Delete(root);
    return 0;
}