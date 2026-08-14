// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2579:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
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

int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size) {
    cJSON *obj1 = NULL;
    cJSON *arr = NULL;
    cJSON *obj2 = NULL;
    cJSON *str1 = NULL;
    cJSON *obj3 = NULL;
    cJSON *str2 = NULL;

    obj1 = cJSON_CreateObject();

    int count = 0;
    if (Size > 0) {
        count = (int)(Data[0] % 16);
    }

    int *numbers = NULL;
    if (count > 0) {
        numbers = (int *)malloc((size_t)count * sizeof(int));
        if (numbers == NULL) {
            cJSON_Delete(obj1);
            return 0;
        }

        size_t offset = 1;
        for (int i = 0; i < count; i++) {
            int v = 0;
            for (size_t j = 0; j < sizeof(int); j++) {
                v <<= 8;
                if (offset < Size) {
                    v |= Data[offset++];
                }
            }
            numbers[i] = v;
        }
    }

    arr = cJSON_CreateIntArray(numbers, count);

    int index = 0;
    if (Size > 1) {
        index = (int)(Data[Size - 1] % (size_t)(count + 2));
    }
    cJSON *item = cJSON_GetArrayItem(arr, index);

    if (obj1 != NULL && arr != NULL) {
        if (!cJSON_AddItemToObject(obj1, "array", arr)) {
            cJSON_Delete(arr);
            arr = NULL;
        } else {
            arr = NULL;
        }
    } else if (arr != NULL) {
        cJSON_Delete(arr);
        arr = NULL;
    }

    obj2 = cJSON_CreateObject();

    size_t s1_len = 0;
    if (Size > 2) {
        s1_len = Data[1] % (Size - 1);
    }
    char *buf1 = (char *)malloc(s1_len + 1);
    if (buf1 != NULL) {
        if (s1_len > 0) {
            memcpy(buf1, Data + 2, s1_len);
        }
        buf1[s1_len] = '\0';
        str1 = cJSON_CreateString(buf1);
        free(buf1);
        buf1 = NULL;
    }

    if (obj2 != NULL && str1 != NULL) {
        if (!cJSON_AddItemToObject(obj2, "str1", str1)) {
            cJSON_Delete(str1);
            str1 = NULL;
        } else {
            str1 = NULL;
        }
    } else if (str1 != NULL) {
        cJSON_Delete(str1);
        str1 = NULL;
    }

    obj3 = cJSON_CreateObject();

    size_t start2 = Size / 2;
    size_t s2_len = (Size > start2) ? (Size - start2) : 0;
    char *buf2 = (char *)malloc(s2_len + 1);
    if (buf2 != NULL) {
        if (s2_len > 0) {
            memcpy(buf2, Data + start2, s2_len);
        }
        buf2[s2_len] = '\0';
        str2 = cJSON_CreateString(buf2);
        free(buf2);
        buf2 = NULL;
    }

    if (obj3 != NULL && str2 != NULL) {
        if (!cJSON_AddItemToObject(obj3, "str2", str2)) {
            cJSON_Delete(str2);
            str2 = NULL;
        } else {
            str2 = NULL;
        }
    } else if (str2 != NULL) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    (void)item;

    cJSON_Delete(obj3);
    cJSON_Delete(obj2);
    cJSON_Delete(obj1);
    free(numbers);
    return 0;
}