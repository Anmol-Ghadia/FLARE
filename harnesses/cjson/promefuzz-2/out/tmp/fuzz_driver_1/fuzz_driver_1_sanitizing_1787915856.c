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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char key1[32], key2[32], key3[32], key4[32], key5[32];
    char del1[32], del2[32], del3[32], del4[32], del5[32];
    cJSON *object1 = NULL;
    cJSON *object2 = NULL;
    cJSON *array1 = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON *item5 = NULL;
    cJSON *refsrc1 = NULL;
    cJSON *refsrc2 = NULL;
    cJSON *refsrc3 = NULL;
    char *minify_buf = NULL;

    snprintf(key1, sizeof(key1), "k1_%02x", (unsigned)(Size > 0 ? Data[0] : 0));
    snprintf(key2, sizeof(key2), "k2_%02x", (unsigned)(Size > 1 ? Data[1] : 0));
    snprintf(key3, sizeof(key3), "k3_%02x", (unsigned)(Size > 2 ? Data[2] : 0));
    snprintf(key4, sizeof(key4), "k4_%02x", (unsigned)(Size > 3 ? Data[3] : 0));
    snprintf(key5, sizeof(key5), "k5_%02x", (unsigned)(Size > 4 ? Data[4] : 0));

    snprintf(del1, sizeof(del1), "%s", (Size > 5 && (Data[5] & 1)) ? key1 : "missing1");
    snprintf(del2, sizeof(del2), "%s", (Size > 6 && (Data[6] & 1)) ? key2 : "missing2");
    snprintf(del3, sizeof(del3), "%s", (Size > 7 && (Data[7] & 1)) ? key3 : "missing3");
    snprintf(del4, sizeof(del4), "%s", (Size > 8 && (Data[8] & 1)) ? key4 : "missing4");
    snprintf(del5, sizeof(del5), "%s", (Size > 9 && (Data[9] & 1)) ? key5 : "missing5");

    object1 = cJSON_CreateObject();
    object2 = cJSON_CreateObject();
    array1 = cJSON_CreateArray();
    if (object1 == NULL || object2 == NULL || array1 == NULL) {
        cJSON_Delete(object1);
        cJSON_Delete(object2);
        cJSON_Delete(array1);
        return 0;
    }

    item1 = cJSON_CreateString((const char *)"value1");
    item2 = cJSON_CreateNumber((double)(Size > 10 ? Data[10] : 0));
    item3 = cJSON_CreateBool((Size > 11 ? (Data[11] & 1) : 0));
    item4 = cJSON_CreateNull();
    item5 = cJSON_CreateString((const char *)"value5");

    refsrc1 = cJSON_CreateString((const char *)"ref1");
    refsrc2 = cJSON_CreateNumber((double)(Size > 12 ? Data[12] : 1));
    refsrc3 = cJSON_CreateArray();
    if (refsrc3 != NULL) {
        cJSON_AddItemToArray(refsrc3, cJSON_CreateString((const char *)"nested"));
    }

    /* Required call order */
    cJSON_AddItemToObject(object1, key1, item1);
    cJSON_AddItemToObject(object1, key2, item2);
    cJSON_AddItemToObjectCS(object1, "const_key_1", item3);
    cJSON_AddItemToObjectCS(object1, "const_key_2", item4);
    cJSON_AddItemToObjectCS(object1, "const_key_3", item5);
    cJSON_AddItemReferenceToArray(array1, refsrc1);
    cJSON_AddItemReferenceToArray(array1, refsrc2);
    cJSON_AddItemReferenceToObject(object2, key3, refsrc1);
    cJSON_AddItemReferenceToObject(object2, key4, refsrc2);
    cJSON_AddItemReferenceToObject(object2, key5, refsrc3);
    cJSON_DeleteItemFromArray(array1, (int)(Size > 13 ? (Data[13] % 4) : 0));
    cJSON_DeleteItemFromObject(object1, del1);
    cJSON_DeleteItemFromObject(object1, del2);
    cJSON_DeleteItemFromObjectCaseSensitive(object1, del3);
    cJSON_DeleteItemFromObjectCaseSensitive(object2, del4);

    minify_buf = make_mutable_string(Data, Size);
    cJSON_Minify(minify_buf);

    cJSON_Delete(object1);
    cJSON_Delete(object2);
    cJSON_Delete(array1);

    free(minify_buf);
    cJSON_Delete(refsrc1);
    cJSON_Delete(refsrc2);
    cJSON_Delete(refsrc3);

    return 0;
}