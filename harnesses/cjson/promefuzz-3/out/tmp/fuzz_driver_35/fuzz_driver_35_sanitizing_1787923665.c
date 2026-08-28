#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static void *fuzz_malloc(size_t sz) {
    return malloc(sz);
}

static void fuzz_free(void *ptr) {
    free(ptr);
}

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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON_Hooks hooks;
    cJSON *object1 = NULL;
    cJSON *object2 = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    char *str1 = NULL;
    char *str2 = NULL;
    char *str3 = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *printed1 = NULL;
    char *printed2 = NULL;
    size_t a, b, c;

    (void)cJSON_Version();

    if (Size == 0) {
        cJSON_InitHooks(NULL);
        return 0;
    }

    hooks.malloc_fn = fuzz_malloc;
    hooks.free_fn = fuzz_free;

    if ((Data[0] & 1) != 0) {
        cJSON_InitHooks(&hooks);
    } else {
        cJSON_InitHooks(NULL);
    }

    a = (Size > 1) ? (size_t)(Data[0] % Size) : 0;
    b = (Size > 2) ? (size_t)(Data[1] % Size) : 0;
    c = (Size > 3) ? (size_t)(Data[2] % Size) : 0;

    if (a > Size) a = Size;
    if (b > Size) b = Size;
    if (c > Size) c = Size;

    str1 = make_cstring(Data, a);
    str2 = make_cstring(Data + a, (a <= Size) ? (Size - a) : 0);
    str3 = make_cstring(Data, Size);
    key1 = make_cstring(Data + b, (b <= Size) ? (Size - b) : 0);
    key2 = make_cstring(Data + c, (c <= Size) ? (Size - c) : 0);

    item1 = cJSON_CreateString(str1 ? str1 : "");
    item2 = cJSON_CreateString(str2 ? str2 : "");
    item3 = cJSON_CreateString(str3 ? str3 : "");

    object1 = cJSON_CreateObject();
    object2 = cJSON_CreateObject();

    if (object1 != NULL && item1 != NULL) {
        cJSON_AddItemToObject(object1, key1 ? key1 : "", item1);
        item1 = NULL;
    }
    if (object1 != NULL && item2 != NULL) {
        cJSON_AddItemToObjectCS(object1, "const_key", item2);
        item2 = NULL;
    }
    if (object2 != NULL && item3 != NULL) {
        cJSON_AddItemToObject(object2, key2 ? key2 : "k", item3);
        item3 = NULL;
    }
    if (object1 != NULL && object2 != NULL) {
        cJSON_AddItemToObjectCS(object1, "nested", object2);
        object2 = NULL;
    }

    if (object1 != NULL) {
        printed1 = cJSON_Print(object1);
        if (printed1 != NULL) {
            free(printed1);
            printed1 = NULL;
        }
    }

    (void)cJSON_Version();

    cJSON_Delete(object1);
    cJSON_Delete(object2);
    cJSON_Delete(item1);
    cJSON_Delete(item2);
    cJSON_Delete(item3);

    free(str1);
    free(str2);
    free(str3);
    free(key1);
    free(key2);

    cJSON_InitHooks(NULL);

    (void)printed2;
    return 0;
}