// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2130:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2130:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_DeleteItemFromArray at cJSON.c:2304:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_Minify at cJSON.c:2924:20 in cJSON.h
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

static char *make_mutable_buffer(const uint8_t *Data, size_t Size)
{
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *buf = make_mutable_buffer(Data, Size);
    if (buf == NULL) {
        return 0;
    }

    size_t q1 = Size / 4;
    size_t q2 = Size / 2;
    size_t q3 = (Size * 3) / 4;

    cJSON *object1 = cJSON_CreateObject();
    cJSON *array1 = cJSON_CreateArray();
    cJSON *object2 = cJSON_CreateObject();

    cJSON *item1 = cJSON_CreateString((const char *)buf);
    cJSON *item2 = cJSON_CreateNumber((double)Size);
    cJSON *item3 = cJSON_CreateString((const char *)(buf + q1));
    cJSON *item4 = cJSON_CreateBool((Size > 0) ? (Data[0] & 1) : 0);
    cJSON *item5 = cJSON_CreateNull();

    const char *k1 = (Size > 0) ? (const char *)buf : "a";
    const char *k2 = (Size > q1) ? (const char *)(buf + q1) : "b";
    const char *k3 = (Size > q2) ? (const char *)(buf + q2) : "c";
    const char *k4 = (Size > q3) ? (const char *)(buf + q3) : "d";
    const char *k5 = "CONST_KEY";
    const char *k6 = "RefKey1";
    const char *k7 = "refkey1";
    const char *k8 = "REFKEY2";

    if (object1 == NULL || array1 == NULL || object2 == NULL ||
        item1 == NULL || item2 == NULL || item3 == NULL || item4 == NULL || item5 == NULL) {
        cJSON_Delete(item1);
        cJSON_Delete(item2);
        cJSON_Delete(item3);
        cJSON_Delete(item4);
        cJSON_Delete(item5);
        cJSON_Delete(object1);
        cJSON_Delete(array1);
        cJSON_Delete(object2);
        free(buf);
        return 0;
    }

    (void)cJSON_AddItemToObject(object1, k1, item1);
    (void)cJSON_AddItemToObject(object1, k2, item2);
    (void)cJSON_AddItemToObjectCS(object1, "literal1", item3);
    (void)cJSON_AddItemToObjectCS(object1, "literal2", item4);
    (void)cJSON_AddItemToObjectCS(object1, k5, item5);

    cJSON *ref_target1 = cJSON_GetObjectItem(object1, k1);
    cJSON *ref_target2 = cJSON_GetObjectItem(object1, k2);
    cJSON *ref_target3 = cJSON_GetObjectItem(object1, "literal1");

    (void)cJSON_AddItemReferenceToArray(array1, ref_target1);
    (void)cJSON_AddItemReferenceToArray(array1, ref_target2);
    (void)cJSON_AddItemReferenceToObject(object2, k6, ref_target1);
    (void)cJSON_AddItemReferenceToObject(object2, k7, ref_target2);
    (void)cJSON_AddItemReferenceToObject(object2, k8, ref_target3);

    cJSON_DeleteItemFromArray(array1, (int)(Size % 4));
    cJSON_DeleteItemFromObject(object1, k1);
    cJSON_DeleteItemFromObject(object1, k2);
    cJSON_DeleteItemFromObjectCaseSensitive(object2, k6);
    cJSON_DeleteItemFromObjectCaseSensitive(object2, k7);

    cJSON_Minify(buf);

    cJSON_Delete(array1);
    cJSON_Delete(object2);
    cJSON_Delete(object1);

    free(buf);
    return 0;
}