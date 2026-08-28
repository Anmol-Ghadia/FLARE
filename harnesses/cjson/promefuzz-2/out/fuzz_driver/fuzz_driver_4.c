// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2506:23 in cJSON.h
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

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *root = cJSON_Parse(input);

    if (root == NULL) {
        root = cJSON_Parse("{}");
        if (root == NULL) {
            free(input);
            return 0;
        }
    }

    if ((root->type & 0xFF) != cJSON_Object) {
        cJSON_Delete(root);
        root = cJSON_Parse("{}");
        if (root == NULL) {
            free(input);
            return 0;
        }
    }

    size_t split1 = Size / 3;
    size_t split2 = (Size * 2) / 3;

    char *s1 = make_cstring(Data, split1);
    char *s2 = make_cstring(Data + split1, split2 - split1);
    char *s3 = make_cstring(Data + split2, Size - split2);

    if (s1 == NULL || s2 == NULL || s3 == NULL) {
        free(s1);
        free(s2);
        free(s3);
        cJSON_Delete(root);
        free(input);
        return 0;
    }

    cJSON *item1 = cJSON_CreateString(s1);
    cJSON *item2 = cJSON_CreateStringReference(s2);

    if (item1 == NULL) {
        item1 = cJSON_CreateString("");
    }
    if (item2 == NULL) {
        item2 = cJSON_CreateStringReference("");
    }

    cJSON_AddItemToObject(root, "key1", item1);
    cJSON_AddItemToObject(root, "key2", item2);

    cJSON_SetValuestring(item1, s3);

    cJSON *g1 = cJSON_GetObjectItem(root, "key1");
    cJSON *g2 = cJSON_GetObjectItem(root, "key2");
    cJSON_SetValuestring(g1, s1);

    cJSON *g3 = cJSON_GetObjectItem(root, "KEY1");
    cJSON *g4 = cJSON_GetObjectItem(root, "KEY2");
    cJSON_SetValuestring(g3, s2);

    cJSON *g5 = cJSON_GetObjectItem(root, "");
    cJSON *g6 = cJSON_GetObjectItem(root, s3);
    (void)g2;
    (void)g4;
    (void)g5;
    (void)g6;

    cJSON_Delete(root);

    free(s1);
    free(s2);
    free(s3);
    free(input);
    return 0;
}