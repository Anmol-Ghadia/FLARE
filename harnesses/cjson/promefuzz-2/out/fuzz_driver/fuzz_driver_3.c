// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_IsTrue at cJSON.c:2992:26 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
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

static char *make_cstring(const uint8_t *data, size_t size, size_t *consumed) {
    if (size == 0) {
        char *s = (char *)malloc(1);
        if (s != NULL) {
            s[0] = '\0';
        }
        if (consumed != NULL) {
            *consumed = 0;
        }
        return s;
    }

    size_t len = data[0] % (size);
    if (len > size - 1) {
        len = size - 1;
    }

    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        if (consumed != NULL) {
            *consumed = 1 + len;
        }
        return NULL;
    }

    if (len > 0) {
        memcpy(s, data + 1, len);
    }
    s[len] = '\0';

    if (consumed != NULL) {
        *consumed = 1 + len;
    }
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off1 = 0, off2 = 0, off3 = 0, off4 = 0;
    char *json_text = make_cstring(Data, Size, &off1);
    if (json_text == NULL) {
        return 0;
    }

    const uint8_t *p = Data + (off1 <= Size ? off1 : Size);
    size_t rem = (off1 <= Size) ? (Size - off1) : 0;

    char *key1 = make_cstring(p, rem, &off2);
    if (key1 == NULL) {
        free(json_text);
        return 0;
    }

    p += (off2 <= rem ? off2 : rem);
    rem -= (off2 <= rem ? off2 : rem);

    char *key2 = make_cstring(p, rem, &off3);
    if (key2 == NULL) {
        free(key1);
        free(json_text);
        return 0;
    }

    p += (off3 <= rem ? off3 : rem);
    rem -= (off3 <= rem ? off3 : rem);

    char *key3 = make_cstring(p, rem, &off4);
    if (key3 == NULL) {
        free(key2);
        free(key1);
        free(json_text);
        return 0;
    }

    p += (off4 <= rem ? off4 : rem);
    rem -= (off4 <= rem ? off4 : rem);

    char *key4 = make_cstring(p, rem, NULL);
    if (key4 == NULL) {
        free(key3);
        free(key2);
        free(key1);
        free(json_text);
        return 0;
    }

    cJSON *root = cJSON_Parse(json_text);
    if (root != NULL) {
        cJSON *item1 = cJSON_GetObjectItemCaseSensitive(root, key1);
        (void)cJSON_IsString(item1);

        cJSON *item2 = cJSON_GetObjectItemCaseSensitive(root, key2);
        (void)cJSON_IsTrue(item2);

        cJSON *item3 = cJSON_GetObjectItemCaseSensitive(root, key3);
        cJSON *item4 = cJSON_GetObjectItemCaseSensitive(root, key4);

        cJSON_bool recurse = (rem > 0) ? (cJSON_bool)(p[0] & 1) : 0;
        const cJSON *dup_src = (item3 != NULL) ? item3 : root;
        cJSON *dup = cJSON_Duplicate(dup_src, recurse);

        if (dup != NULL) {
            cJSON *dup_item1 = cJSON_GetObjectItemCaseSensitive(dup, key1);
            cJSON *dup_item2 = cJSON_GetObjectItemCaseSensitive(dup, key2);
            cJSON_bool case_sensitive = (rem > 1) ? (cJSON_bool)(p[1] & 1) : 1;
            (void)cJSON_Compare(
                dup_item1 != NULL ? dup_item1 : dup,
                dup_item2 != NULL ? dup_item2 : item4,
                case_sensitive
            );
            cJSON_Delete(dup);
        }

        cJSON_Delete(root);
    }

    free(key4);
    free(key3);
    free(key2);
    free(key1);
    free(json_text);
    return 0;
}