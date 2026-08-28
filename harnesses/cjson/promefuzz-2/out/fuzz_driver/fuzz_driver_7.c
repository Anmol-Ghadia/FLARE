// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetArraySize at cJSON.c:1899:19 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

static char *dup_bytes_as_cstring(const uint8_t *data, size_t len) {
    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(out, data, len);
    }
    out[len] = '\0';
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t p0 = 0, p1 = 0, p2 = 0, p3 = 0;
    size_t off = 0;
    char *s0 = NULL, *s1 = NULL, *s2 = NULL, *s3 = NULL;
    cJSON *str0 = NULL;
    cJSON *array = NULL;
    cJSON *str1 = NULL;
    cJSON *str2 = NULL;
    cJSON *str3 = NULL;
    cJSON_Hooks hooks;

    if (Size == 0) {
        s0 = dup_bytes_as_cstring((const uint8_t *)"", 0);
        s1 = dup_bytes_as_cstring((const uint8_t *)"", 0);
        s2 = dup_bytes_as_cstring((const uint8_t *)"", 0);
        s3 = dup_bytes_as_cstring((const uint8_t *)"", 0);
    } else if (Size < 4) {
        p0 = Size;
        off = 0;
        s0 = dup_bytes_as_cstring(Data + off, p0);
        s1 = dup_bytes_as_cstring((const uint8_t *)"", 0);
        s2 = dup_bytes_as_cstring((const uint8_t *)"", 0);
        s3 = dup_bytes_as_cstring((const uint8_t *)"", 0);
    } else {
        size_t rem;

        p0 = Data[0] % (Size - 2);
        rem = Size - 3 - p0;

        p1 = (rem > 0) ? (Data[1] % (rem + 1)) : 0;
        rem -= p1;

        p2 = (rem > 0) ? (Data[2] % (rem + 1)) : 0;
        rem -= p2;

        p3 = rem;
        off = 3;

        s0 = dup_bytes_as_cstring(Data + off, p0);
        s1 = dup_bytes_as_cstring(Data + off + p0, p1);
        s2 = dup_bytes_as_cstring(Data + off + p0 + p1, p2);
        s3 = dup_bytes_as_cstring(Data + off + p0 + p1 + p2, p3);
    }

    if (s0 == NULL || s1 == NULL || s2 == NULL || s3 == NULL) {
        free(s0);
        free(s1);
        free(s2);
        free(s3);
        return 0;
    }

    str0 = cJSON_CreateString(s0);
    array = cJSON_CreateArray();
    str1 = cJSON_CreateString(s1);
    str2 = cJSON_CreateString(s2);
    str3 = cJSON_CreateString(s3);

    hooks.malloc_fn = NULL;
    hooks.free_fn = NULL;
    cJSON_InitHooks(&hooks);

    cJSON_Delete(str0);

    (void)cJSON_GetArraySize(array);

    if (array != NULL && str1 != NULL) {
        if (!cJSON_AddItemToArray(array, str1)) {
            cJSON_Delete(str1);
            str1 = NULL;
        }
    } else if (str1 != NULL) {
        cJSON_Delete(str1);
        str1 = NULL;
    }

    if (array != NULL && str2 != NULL) {
        if (!cJSON_AddItemToArray(array, str2)) {
            cJSON_Delete(str2);
            str2 = NULL;
        }
    } else if (str2 != NULL) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    if (array != NULL && str3 != NULL) {
        if (!cJSON_AddItemToObject(array, s0, str3)) {
            cJSON_Delete(str3);
            str3 = NULL;
        }
    } else if (str3 != NULL) {
        cJSON_Delete(str3);
        str3 = NULL;
    }

    (void)cJSON_GetArraySize(array);

    cJSON_Delete(array);
    cJSON_InitHooks(NULL);

    free(s0);
    free(s1);
    free(s2);
    free(s3);
    return 0;
}