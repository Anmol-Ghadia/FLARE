// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2506:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_SetValuestring at cJSON.c:403:21 in cJSON.h
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
    char *input = NULL;
    char *s1 = NULL;
    char *s2 = NULL;
    char *k1 = NULL;
    char *k2 = NULL;
    char *nv1 = NULL;
    char *nv2 = NULL;
    char *nv3 = NULL;
    cJSON *root = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *got1 = NULL;
    cJSON *got2 = NULL;
    cJSON *got3 = NULL;
    cJSON *got4 = NULL;

    if (Data == NULL) {
        return 0;
    }

    input = make_cstring(Data, Size);
    if (input == NULL) {
        return 0;
    }

    root = cJSON_Parse(input);

    s1 = make_cstring(Data, Size > 0 ? Size / 4 : 0);
    s2 = make_cstring(Data + (Size > 0 ? Size / 4 : 0), Size > 1 ? Size / 4 : 0);
    k1 = make_cstring(Data + (Size > 1 ? Size / 2 : 0), Size > 2 ? Size / 8 : 0);
    k2 = make_cstring(Data + (Size > 2 ? (Size * 5) / 8 : 0), Size > 3 ? Size / 8 : 0);
    nv1 = make_cstring(Data + (Size > 3 ? (Size * 3) / 4 : 0), Size > 4 ? Size / 16 : 0);
    nv2 = make_cstring(Data + (Size > 4 ? (Size * 13) / 16 : 0), Size > 5 ? Size / 16 : 0);
    nv3 = make_cstring(Data + (Size > 5 ? (Size * 7) / 8 : 0), Size > 6 ? Size / 16 : 0);

    if (s1 == NULL || s2 == NULL || k1 == NULL || k2 == NULL || nv1 == NULL || nv2 == NULL || nv3 == NULL) {
        cJSON_Delete(root);
        free(input);
        free(s1);
        free(s2);
        free(k1);
        free(k2);
        free(nv1);
        free(nv2);
        free(nv3);
        return 0;
    }

    item1 = cJSON_CreateString(s1);
    item2 = cJSON_CreateStringReference(s2);

    if (root != NULL && (root->type & 0xFF) == cJSON_Object) {
        if (item1 != NULL) {
            if (!cJSON_AddItemToObject(root, k1, item1)) {
                cJSON_Delete(item1);
                item1 = NULL;
            }
        }

        if (item2 != NULL) {
            if (!cJSON_AddItemToObject(root, k2, item2)) {
                cJSON_Delete(item2);
                item2 = NULL;
            }
        }

        if (item1 != NULL) {
            (void)cJSON_SetValuestring(item1, nv1);
        }

        got1 = cJSON_GetObjectItem(root, k1);
        got2 = cJSON_GetObjectItem(root, k2);

        if (got1 != NULL) {
            (void)cJSON_SetValuestring(got1, nv2);
        }

        got3 = cJSON_GetObjectItem(root, k1);
        got4 = cJSON_GetObjectItem(root, k2);

        if (got3 != NULL) {
            (void)cJSON_SetValuestring(got3, nv3);
        }

        (void)cJSON_GetObjectItem(root, "");
        (void)cJSON_GetObjectItem(root, input);
    } else {
        if (item1 != NULL) {
            (void)cJSON_SetValuestring(item1, nv1);
        }

        got1 = cJSON_GetObjectItem(root, k1);
        got2 = cJSON_GetObjectItem(root, k2);

        if (got1 != NULL) {
            (void)cJSON_SetValuestring(got1, nv2);
        }

        got3 = cJSON_GetObjectItem(root, k1);
        got4 = cJSON_GetObjectItem(root, k2);

        if (got3 != NULL) {
            (void)cJSON_SetValuestring(got3, nv3);
        }

        cJSON_Delete(item1);
        cJSON_Delete(item2);
    }

    cJSON_Delete(root);
    free(input);
    free(s1);
    free(s2);
    free(k1);
    free(k2);
    free(nv1);
    free(nv2);
    free(nv3);
    return 0;
}