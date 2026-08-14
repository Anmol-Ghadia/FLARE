// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1857:19 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *s1 = NULL;
    char *s2 = NULL;
    char *s3 = NULL;
    char *key = NULL;
    cJSON *item1 = NULL;
    cJSON *array = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON_Hooks hooks;
    size_t q1, q2, q3;
    size_t len1, len2, len3, len4;
    int use_custom_hooks;

    if (Data == NULL) {
        return 0;
    }

    q1 = (Size > 0) ? (Size / 4) : 0;
    q2 = (Size > 1) ? (Size / 2) : 0;
    q3 = (Size > 2) ? ((Size * 3) / 4) : 0;

    len1 = q1;
    len2 = (q2 > q1) ? (q2 - q1) : 0;
    len3 = (q3 > q2) ? (q3 - q2) : 0;
    len4 = (Size > q3) ? (Size - q3) : 0;

    s1 = (char *)malloc(len1 + 1);
    s2 = (char *)malloc(len2 + 1);
    s3 = (char *)malloc(len3 + 1);
    key = (char *)malloc(len4 + 1);

    if ((s1 == NULL) || (s2 == NULL) || (s3 == NULL) || (key == NULL)) {
        free(s1);
        free(s2);
        free(s3);
        free(key);
        return 0;
    }

    if (len1 > 0) memcpy(s1, Data, len1);
    if (len2 > 0) memcpy(s2, Data + q1, len2);
    if (len3 > 0) memcpy(s3, Data + q2, len3);
    if (len4 > 0) memcpy(key, Data + q3, len4);

    s1[len1] = '\0';
    s2[len2] = '\0';
    s3[len3] = '\0';
    key[len4] = '\0';

    item1 = cJSON_CreateString(s1);
    array = cJSON_CreateArray();
    item2 = cJSON_CreateString(s2);
    item3 = cJSON_CreateString(s3);
    item4 = cJSON_CreateString(key);

    use_custom_hooks = (Size > 0) ? (Data[0] & 1) : 0;
    if (use_custom_hooks) {
        hooks.malloc_fn = malloc;
        hooks.free_fn = free;
        cJSON_InitHooks(&hooks);
    } else {
        cJSON_InitHooks(NULL);
    }

    cJSON_Delete(item1);

    (void)cJSON_GetArraySize(array);

    if (array != NULL && item2 != NULL) {
        (void)cJSON_AddItemToArray(array, item2);
        item2 = NULL;
    }

    if (array != NULL && item3 != NULL) {
        (void)cJSON_AddItemToArray(array, item3);
        item3 = NULL;
    }

    if (array != NULL && item4 != NULL) {
        (void)cJSON_AddItemToObject(array, key, item4);
        item4 = NULL;
    }

    cJSON_Delete(array);
    cJSON_Delete(item2);
    cJSON_Delete(item3);
    cJSON_Delete(item4);

    cJSON_InitHooks(NULL);

    free(s1);
    free(s2);
    free(s3);
    free(key);

    return 0;
}