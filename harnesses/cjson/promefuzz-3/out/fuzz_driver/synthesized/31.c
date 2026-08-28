// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2548:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
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

int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
    if (Data == NULL) {
        return 0;
    }

    size_t o1 = 0, l1 = 0, o2 = 0, l2 = 0, o3 = 0, l3 = 0, o4 = 0, l4 = 0;

    if (Size >= 8) {
        o1 = Data[0] % (Size + 1);
        l1 = Data[1] % (Size - o1 + 1);

        o2 = Data[2] % (Size + 1);
        l2 = Data[3] % (Size - o2 + 1);

        o3 = Data[4] % (Size + 1);
        l3 = Data[5] % (Size - o3 + 1);

        o4 = Data[6] % (Size + 1);
        l4 = Data[7] % (Size - o4 + 1);
    } else {
        o1 = 0; l1 = Size;
        o2 = 0; l2 = Size;
        o3 = 0; l3 = Size;
        o4 = 0; l4 = Size;
    }

    char *key1 = make_cstring(Data + o1, l1);
    char *val1 = make_cstring(Data + o2, l2);
    char *key2 = make_cstring(Data + o3, l3);
    char *val2 = make_cstring(Data + o4, l4);

    if (key1 == NULL || val1 == NULL || key2 == NULL || val2 == NULL) {
        free(key1);
        free(val1);
        free(key2);
        free(val2);
        return 0;
    }

    cJSON *object = cJSON_CreateObject();
    if (object == NULL) {
        free(key1);
        free(val1);
        free(key2);
        free(val2);
        return 0;
    }

    cJSON *added1 = cJSON_AddStringToObject(object, key1, val1);
    cJSON *added2 = cJSON_AddStringToObject(object, key2, val2);

    if (added1 != NULL) {
        (void)cJSON_GetStringValue(added1);
        (void)cJSON_SetValuestring(added1, val2);
        (void)cJSON_GetStringValue(added1);
    }

    if (added2 != NULL) {
        (void)cJSON_GetStringValue(added2);
        (void)cJSON_SetValuestring(added2, val1);
        (void)cJSON_GetStringValue(added2);
    }

    cJSON *found1 = cJSON_GetObjectItem(object, key1);
    if (found1 != NULL) {
        (void)cJSON_GetStringValue(found1);
        (void)cJSON_SetValuestring(found1, val1);
        (void)cJSON_GetStringValue(found1);
    }

    cJSON *found2 = cJSON_GetObjectItem(object, key2);
    if (found2 != NULL) {
        (void)cJSON_GetStringValue(found2);
        (void)cJSON_SetValuestring(found2, val2);
        (void)cJSON_GetStringValue(found2);
    }

    cJSON *standalone = cJSON_CreateString(val1);
    if (standalone != NULL) {
        (void)cJSON_GetStringValue(standalone);
        (void)cJSON_SetValuestring(standalone, val2);
        (void)cJSON_GetStringValue(standalone);
        cJSON_Delete(standalone);
    }

    cJSON *ref = cJSON_CreateStringReference(val1);
    if (ref != NULL) {
        (void)cJSON_GetStringValue(ref);
        (void)cJSON_SetValuestring(ref, val2);
        (void)cJSON_GetStringValue(ref);
        cJSON_Delete(ref);
    }

    cJSON_Delete(object);
    free(key1);
    free(val1);
    free(key2);
    free(val2);
    return 0;
}