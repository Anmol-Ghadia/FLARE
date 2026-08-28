#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cJSON.h"

static char *make_key_string(const uint8_t *data, size_t size, size_t *consumed)
{
    size_t i;
    char *out;

    if (consumed == NULL) {
        return NULL;
    }

    if (size == 0) {
        *consumed = 0;
        out = (char *)malloc(1);
        if (out != NULL) {
            out[0] = '\0';
        }
        return out;
    }

    *consumed = size > 32 ? 32 : size;
    out = (char *)malloc(*consumed + 1);
    if (out == NULL) {
        *consumed = 0;
        return NULL;
    }

    for (i = 0; i < *consumed; i++) {
        unsigned char c = data[i];
        if (c == '\0') {
            c = 'A';
        }
        out[i] = (char)c;
    }
    out[*consumed] = '\0';
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *dup = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON *dup_item1 = NULL;
    cJSON *dup_item2 = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *key4 = NULL;
    size_t used1 = 0, used2 = 0, used3 = 0, used4 = 0;
    cJSON_bool recurse = 0;
    cJSON_bool case_sensitive = 0;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    root = cJSON_ParseWithLength((const char *)Data, Size);
    if (root == NULL) {
        root = cJSON_CreateObject();
        if (root == NULL) {
            return 0;
        }

        if (Size > 0) {
            cJSON_AddStringToObject(root, "a", (const char *)Data);
        }
        if (Size > 1) {
            cJSON_AddBoolToObject(root, "b", (Data[0] & 1) ? 1 : 0);
        }
        if (Size > 2) {
            cJSON_AddNumberToObject(root, "c", (double)Data[1]);
        }
        if (Size > 3) {
            cJSON *nested = cJSON_CreateObject();
            if (nested != NULL) {
                cJSON_AddBoolToObject(nested, "inner_true", (Data[2] & 1) ? 1 : 0);
                cJSON_AddStringToObject(nested, "inner_str", (const char *)Data);
                cJSON_AddItemToObject(root, "d", nested);
            }
        }
    }

    key1 = make_key_string(Data, Size, &used1);
    key2 = make_key_string(Data + (used1 < Size ? used1 : Size), Size > used1 ? Size - used1 : 0, &used2);
    key3 = make_key_string(Data + ((used1 + used2) < Size ? (used1 + used2) : Size),
                           Size > (used1 + used2) ? Size - (used1 + used2) : 0, &used3);
    key4 = make_key_string(Data + ((used1 + used2 + used3) < Size ? (used1 + used2 + used3) : Size),
                           Size > (used1 + used2 + used3) ? Size - (used1 + used2 + used3) : 0, &used4);

    if (key1 == NULL || key2 == NULL || key3 == NULL || key4 == NULL) {
        free(key1);
        free(key2);
        free(key3);
        free(key4);
        cJSON_Delete(root);
        return 0;
    }

    if (cJSON_IsObject(root)) {
        item1 = cJSON_GetObjectItemCaseSensitive(root, key1);
        (void)cJSON_IsString(item1);

        item2 = cJSON_GetObjectItemCaseSensitive(root, key2);
        (void)cJSON_IsTrue(item2);

        item3 = cJSON_GetObjectItemCaseSensitive(root, key3);
        item4 = cJSON_GetObjectItemCaseSensitive(root, key4);
    } else {
        item1 = cJSON_GetObjectItemCaseSensitive(root, key1);
        (void)cJSON_IsString(item1);

        item2 = cJSON_GetObjectItemCaseSensitive(root, key2);
        (void)cJSON_IsTrue(item2);

        item3 = cJSON_GetObjectItemCaseSensitive(root, key3);
        item4 = cJSON_GetObjectItemCaseSensitive(root, key4);
    }

    recurse = (Size > 0) ? (Data[0] & 1) : 0;
    dup = cJSON_Duplicate(root, recurse);

    if (dup != NULL) {
        dup_item1 = cJSON_GetObjectItemCaseSensitive(dup, key1);
        dup_item2 = cJSON_GetObjectItemCaseSensitive(dup, key2);
        case_sensitive = (Size > 1) ? (Data[1] & 1) : 0;
        (void)cJSON_Compare(dup_item1, dup_item2, case_sensitive);
    } else {
        (void)cJSON_Compare(item3, item4, 0);
    }

    cJSON_Delete(dup);
    cJSON_Delete(root);

    free(key1);
    free(key2);
    free(key3);
    free(key4);

    return 0;
}