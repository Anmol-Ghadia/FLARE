// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2548:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *input = make_cstring(Data, Size);
    if (input == NULL) {
        return 0;
    }

    cJSON *root = cJSON_Parse(input);
    if (root == NULL) {
        root = cJSON_Parse("{}");
    }
    if (root == NULL) {
        free(input);
        return 0;
    }

    if ((root->type & 0xFF) != cJSON_Object) {
        cJSON_Delete(root);
        root = cJSON_Parse("{}");
        if (root == NULL) {
            free(input);
            return 0;
        }
    }

    size_t q1 = Size / 4;
    size_t q2 = Size / 2;
    size_t q3 = (Size * 3) / 4;

    char *s1 = make_cstring(Data, q1);
    char *s2 = make_cstring(Data + q1, q2 - q1);
    char *s3 = make_cstring(Data + q2, q3 - q2);
    char *s4 = make_cstring(Data + q3, Size - q3);

    if (s1 == NULL || s2 == NULL || s3 == NULL || s4 == NULL) {
        free(s1);
        free(s2);
        free(s3);
        free(s4);
        cJSON_Delete(root);
        free(input);
        return 0;
    }

    cJSON *item1 = cJSON_CreateString(s1);
    cJSON *item2 = cJSON_CreateStringReference(s2);

    if (item1 != NULL) {
        (void)cJSON_AddItemToObject(root, "k1", item1);
    } else {
        item1 = cJSON_GetObjectItem(root, "k1");
    }

    if (item2 != NULL) {
        (void)cJSON_AddItemToObject(root, "k2", item2);
    } else {
        item2 = cJSON_GetObjectItem(root, "k2");
    }

    (void)cJSON_SetValuestring(item1, s3);

    cJSON *g1 = cJSON_GetObjectItem(root, "k1");
    cJSON *g2 = cJSON_GetObjectItem(root, "k2");

    (void)cJSON_SetValuestring(g1, s4);

    cJSON *g3 = cJSON_GetObjectItem(root, "K1");
    cJSON *g4 = cJSON_GetObjectItem(root, "K2");

    (void)cJSON_SetValuestring(g3, s1);

    cJSON *g5 = cJSON_GetObjectItem(root, "");
    cJSON *g6 = cJSON_GetObjectItem(root, s2);

    (void)g2;
    (void)g4;
    (void)g5;
    (void)g6;

    cJSON_Delete(root);

    free(s1);
    free(s2);
    free(s3);
    free(s4);
    free(input);
    return 0;
}