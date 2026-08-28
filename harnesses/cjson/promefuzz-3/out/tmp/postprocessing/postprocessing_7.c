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

static char *copy_as_cstring(const uint8_t *data, size_t size) {
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
    size_t o1 = 0, l1 = 0;
    size_t o2 = 0, l2 = 0;
    size_t o3 = 0, l3 = 0;
    size_t o4 = 0, l4 = 0;

    if (Size > 0) {
        o1 = 0;
        l1 = Data[0] % (Size + 1);
    }
    if (Size > 1) {
        o2 = 1 % Size;
        l2 = Data[1] % (Size - o2 + 1);
    }
    if (Size > 2) {
        o3 = 2 % Size;
        l3 = Data[2] % (Size - o3 + 1);
    }
    if (Size > 3) {
        o4 = 3 % Size;
        l4 = Data[3] % (Size - o4 + 1);
    }

    char *s1 = copy_as_cstring((o1 < Size) ? (Data + o1) : Data, l1);
    char *s2 = copy_as_cstring((o2 < Size) ? (Data + o2) : Data, l2);
    char *s3 = copy_as_cstring((o3 < Size) ? (Data + o3) : Data, l3);
    char *s4 = copy_as_cstring((o4 < Size) ? (Data + o4) : Data, l4);

    if (s1 == NULL || s2 == NULL || s3 == NULL || s4 == NULL) {
        free(s1);
        free(s2);
        free(s3);
        free(s4);
        return 0;
    }

    cJSON *str1 = cJSON_CreateString(s1);
    cJSON *array = cJSON_CreateArray();
    cJSON *str2 = cJSON_CreateString(s2);
    cJSON *str3 = cJSON_CreateString(s3);
    cJSON *str4 = cJSON_CreateString(s4);

    cJSON_Hooks hooks;
    if (Size > 0 && (Data[0] & 1)) {
        hooks.malloc_fn = malloc;
        hooks.free_fn = free;
        cJSON_InitHooks(&hooks);
    } else {
        cJSON_InitHooks(NULL);
    }

    cJSON_Delete(str1);

    (void)cJSON_GetArraySize(array);

    if (array != NULL && str2 != NULL) {
        if (!cJSON_AddItemToArray(array, str2)) {
            cJSON_Delete(str2);
            str2 = NULL;
        } else {
            str2 = NULL;
        }
    } else if (str2 != NULL) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    if (array != NULL && str3 != NULL) {
        if (!cJSON_AddItemToArray(array, str3)) {
            cJSON_Delete(str3);
            str3 = NULL;
        } else {
            str3 = NULL;
        }
    } else if (str3 != NULL) {
        cJSON_Delete(str3);
        str3 = NULL;
    }

    if (array != NULL && str4 != NULL) {
        if (!cJSON_AddItemToObject(array, s1, str4)) {
            cJSON_Delete(str4);
            str4 = NULL;
        } else {
            str4 = NULL;
        }
    } else if (str4 != NULL) {
        cJSON_Delete(str4);
        str4 = NULL;
    }

    cJSON_Delete(array);

    free(s1);
    free(s2);
    free(s3);
    free(s4);

    cJSON_InitHooks(NULL);
    return 0;
}