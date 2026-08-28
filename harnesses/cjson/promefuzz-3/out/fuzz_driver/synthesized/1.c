// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2556:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateFalse at cJSON.c:2441:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2083:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2083:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2083:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2088:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2098:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2098:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2098:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2098:26 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2262:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2286:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2286:20 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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

static char *make_mutable_string(const uint8_t *Data, size_t Size) {
    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return NULL;
    }
    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';
    return buf;
}

static char *make_key_from_data(const uint8_t *Data, size_t Size, size_t offset, size_t max_len) {
    size_t i;
    size_t len = 0;
    char *key;

    if (max_len == 0) {
        max_len = 1;
    }

    key = (char *)malloc(max_len + 1);
    if (key == NULL) {
        return NULL;
    }

    if (Size == 0) {
        key[0] = 'A';
        key[1] = '\0';
        return key;
    }

    for (i = 0; i < max_len && (offset + i) < Size; i++) {
        unsigned char c = Data[offset + i];
        if (c == '\0') {
            break;
        }
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' || c == '-') {
            key[len++] = (char)c;
        } else {
            key[len++] = (char)('a' + (c % 26));
        }
    }

    if (len == 0) {
        key[len++] = 'K';
    }
    key[len] = '\0';
    return key;
}

int LLVMFuzzerTestOneInput_1(const uint8_t *Data, size_t Size) {
    cJSON *object1 = NULL;
    cJSON *object2 = NULL;
    cJSON *array1 = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON *item5 = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *key4 = NULL;
    char *key5 = NULL;
    char *key6 = NULL;
    char *key7 = NULL;
    char *minify_buf = NULL;
    int idx = 0;

    static const char *cs_key1 = "const_key_1";
    static const char *cs_key2 = "const_key_2";
    static const char *cs_key3 = "CONST_KEY_3";
    static const char *ref_key1 = "ref_key_1";
    static const char *ref_key2 = "Ref_Key_2";
    static const char *ref_key3 = "REF_KEY_3";

    key1 = make_key_from_data(Data, Size, 0, 16);
    key2 = make_key_from_data(Data, Size, 16, 16);
    key3 = make_key_from_data(Data, Size, 32, 16);
    key4 = make_key_from_data(Data, Size, 48, 16);
    key5 = make_key_from_data(Data, Size, 64, 16);
    key6 = make_key_from_data(Data, Size, 80, 16);
    key7 = make_key_from_data(Data, Size, 96, 16);
    minify_buf = make_mutable_string(Data, Size);

    object1 = cJSON_CreateObject();
    object2 = cJSON_CreateObject();
    array1 = cJSON_CreateArray();

    if (Size > 0) {
        item1 = cJSON_CreateNumber((double)Data[0]);
    } else {
        item1 = cJSON_CreateNull();
    }

    if (Size > 1) {
        item2 = cJSON_CreateBool((Data[1] & 1) ? 1 : 0);
    } else {
        item2 = cJSON_CreateFalse();
    }

    if (Size > 2) {
        item3 = cJSON_CreateString((const char *)minify_buf);
    } else {
        item3 = cJSON_CreateString("");
    }

    if (Size > 3) {
        item4 = cJSON_CreateNumber((double)((signed char)Data[2]));
    } else {
        item4 = cJSON_CreateTrue();
    }

    if (Size > 4) {
        item5 = cJSON_CreateString((const char *)minify_buf);
    } else {
        item5 = cJSON_CreateString("x");
    }

    if (object1 != NULL && key1 != NULL && item1 != NULL) {
        (void)cJSON_AddItemToObject(object1, key1, item1);
        item1 = NULL;
    }

    if (object1 != NULL && key2 != NULL && item2 != NULL) {
        (void)cJSON_AddItemToObject(object1, key2, item2);
        item2 = NULL;
    }

    if (object1 != NULL && item3 != NULL) {
        (void)cJSON_AddItemToObjectCS(object1, cs_key1, item3);
        item3 = NULL;
    }

    if (object1 != NULL && item4 != NULL) {
        (void)cJSON_AddItemToObjectCS(object1, cs_key2, item4);
        item4 = NULL;
    }

    if (object1 != NULL && item5 != NULL) {
        (void)cJSON_AddItemToObjectCS(object1, cs_key3, item5);
        item5 = NULL;
    }

    if (object1 != NULL && object1->child != NULL && array1 != NULL) {
        (void)cJSON_AddItemReferenceToArray(array1, object1->child);
    }

    if (object1 != NULL && object1->child != NULL && object1->child->next != NULL && array1 != NULL) {
        (void)cJSON_AddItemReferenceToArray(array1, object1->child->next);
    } else if (object1 != NULL && object1->child != NULL && array1 != NULL) {
        (void)cJSON_AddItemReferenceToArray(array1, object1->child);
    }

    if (object2 != NULL && object1 != NULL && object1->child != NULL) {
        (void)cJSON_AddItemReferenceToObject(object2, ref_key1, object1->child);
    }

    if (object2 != NULL && object1 != NULL && object1->child != NULL && object1->child->next != NULL) {
        (void)cJSON_AddItemReferenceToObject(object2, ref_key2, object1->child->next);
    } else if (object2 != NULL && object1 != NULL && object1->child != NULL) {
        (void)cJSON_AddItemReferenceToObject(object2, ref_key2, object1->child);
    }

    if (object2 != NULL && object1 != NULL && object1->child != NULL) {
        cJSON *last = object1->child;
        while (last->next != NULL) {
            last = last->next;
        }
        (void)cJSON_AddItemReferenceToObject(object2, ref_key3, last);
    }

    if (array1 != NULL) {
        if (Size > 5) {
            idx = (int)(Data[5] % 4);
        } else {
            idx = 0;
        }
        cJSON_DeleteItemFromArray(array1, idx);
    }

    if (object1 != NULL && key1 != NULL) {
        cJSON_DeleteItemFromObject(object1, key1);
    }

    if (object1 != NULL && key3 != NULL) {
        cJSON_DeleteItemFromObject(object1, key3);
    }

    if (object2 != NULL) {
        cJSON_DeleteItemFromObjectCaseSensitive(object2, ref_key1);
    }

    if (object2 != NULL) {
        cJSON_DeleteItemFromObjectCaseSensitive(object2, ref_key2);
    }

    if (minify_buf != NULL) {
        cJSON_Minify(minify_buf);
    }

    cJSON_Delete(array1);
    cJSON_Delete(object2);
    cJSON_Delete(object1);

    free(item1);
    free(item2);
    free(item3);
    free(item4);
    free(item5);

    free(key1);
    free(key2);
    free(key3);
    free(key4);
    free(key5);
    free(key6);
    free(key7);
    free(minify_buf);

    return 0;
}