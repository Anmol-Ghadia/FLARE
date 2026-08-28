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

static char *make_cstring(const uint8_t *data, size_t len) {
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(s, data, len);
    }
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t o1 = 0, l1 = 0, o2 = 0, l2 = 0, o3 = 0, l3 = 0, o4 = 0, l4 = 0, o5 = 0, l5 = 0;
    cJSON *s1 = NULL;
    cJSON *arr = NULL;
    cJSON *s2 = NULL;
    cJSON *s3 = NULL;
    cJSON *s4 = NULL;
    cJSON *obj = NULL;
    cJSON_Hooks hooks;
    char *buf1 = NULL, *buf2 = NULL, *buf3 = NULL, *buf4 = NULL, *buf5 = NULL;

    if (Size > 0) {
        o1 = Data[0] % (Size + 1);
    }
    if (Size > 1) {
        l1 = Data[1] % (Size - o1 + 1);
    }

    if (Size > 2) {
        o2 = Data[2] % (Size + 1);
    }
    if (o2 <= Size && Size > o2 && Size > 3) {
        l2 = Data[3] % (Size - o2 + 1);
    }

    if (Size > 4) {
        o3 = Data[4] % (Size + 1);
    }
    if (o3 <= Size && Size > o3 && Size > 5) {
        l3 = Data[5] % (Size - o3 + 1);
    }

    if (Size > 6) {
        o4 = Data[6] % (Size + 1);
    }
    if (o4 <= Size && Size > o4 && Size > 7) {
        l4 = Data[7] % (Size - o4 + 1);
    }

    if (Size > 8) {
        o5 = Data[8] % (Size + 1);
    }
    if (o5 <= Size && Size > o5 && Size > 9) {
        l5 = Data[9] % (Size - o5 + 1);
    }

    if (o1 > Size) o1 = Size;
    if (o2 > Size) o2 = Size;
    if (o3 > Size) o3 = Size;
    if (o4 > Size) o4 = Size;
    if (o5 > Size) o5 = Size;

    if (l1 > Size - o1) l1 = Size - o1;
    if (l2 > Size - o2) l2 = Size - o2;
    if (l3 > Size - o3) l3 = Size - o3;
    if (l4 > Size - o4) l4 = Size - o4;
    if (l5 > Size - o5) l5 = Size - o5;

    buf1 = make_cstring(Data + o1, l1);
    buf2 = make_cstring(Data + o2, l2);
    buf3 = make_cstring(Data + o3, l3);
    buf4 = make_cstring(Data + o4, l4);
    buf5 = make_cstring(Data + o5, l5);

    if (buf1 == NULL || buf2 == NULL || buf3 == NULL || buf4 == NULL || buf5 == NULL) {
        free(buf1);
        free(buf2);
        free(buf3);
        free(buf4);
        free(buf5);
        return 0;
    }

    /* Required call order begins here */
    s1 = cJSON_CreateString(buf1);
    arr = cJSON_CreateArray();
    s2 = cJSON_CreateString(buf2);
    s3 = cJSON_CreateString(buf3);
    s4 = cJSON_CreateString(buf4);

    if (Size > 10 && (Data[10] & 1)) {
        hooks.malloc_fn = malloc;
        hooks.free_fn = free;
        cJSON_InitHooks(&hooks);
    } else {
        cJSON_InitHooks(NULL);
    }

    cJSON_Delete(s1);

    (void)cJSON_GetArraySize(arr);

    if (arr != NULL && s2 != NULL) {
        (void)cJSON_AddItemToArray(arr, s2);
        s2 = NULL;
    } else if (s2 != NULL) {
        cJSON_Delete(s2);
        s2 = NULL;
    }

    if (arr != NULL && s3 != NULL) {
        (void)cJSON_AddItemToArray(arr, s3);
        s3 = NULL;
    } else if (s3 != NULL) {
        cJSON_Delete(s3);
        s3 = NULL;
    }

    obj = cJSON_CreateArray();
    if (obj != NULL && s4 != NULL) {
        (void)cJSON_AddItemToObject(obj, buf5, s4);
        s4 = NULL;
    } else if (s4 != NULL) {
        cJSON_Delete(s4);
        s4 = NULL;
    }

    cJSON_Delete(arr);
    cJSON_Delete(obj);

    free(buf1);
    free(buf2);
    free(buf3);
    free(buf4);
    free(buf5);

    return 0;
}