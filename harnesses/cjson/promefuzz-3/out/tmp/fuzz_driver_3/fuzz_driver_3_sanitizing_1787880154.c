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

static char *make_cstring_from_bytes(const uint8_t *data, size_t size) {
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
    char *json_buf = make_cstring_from_bytes(Data, Size);
    if (json_buf == NULL) {
        return 0;
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *root = cJSON_ParseWithLength(json_buf, Size);
    if (root == NULL) {
        root = cJSON_CreateObject();
        if (root != NULL) {
            cJSON_AddStringToObject(root, "a", json_buf);
            cJSON_AddBoolToObject(root, "b", (Size > 0) ? ((Data[0] & 1) != 0) : 0);
            cJSON *nested = cJSON_CreateObject();
            if (nested != NULL) {
                cJSON_AddStringToObject(nested, "x", (Size > 1) ? (const char *)(json_buf + 1) : "");
                cJSON_AddItemToObject(root, "c", nested);
            }
        }
    }

    if (root != NULL) {
        size_t q1_len = (Size > 0) ? ((size_t)Data[0] % (Size + 1)) : 0;
        size_t q2_len = (Size > 1) ? ((size_t)Data[1] % (Size + 1)) : 0;
        size_t q3_len = (Size > 2) ? ((size_t)Data[2] % (Size + 1)) : 0;
        size_t q4_len = (Size > 3) ? ((size_t)Data[3] % (Size + 1)) : 0;
        size_t q5_len = (Size > 4) ? ((size_t)Data[4] % (Size + 1)) : 0;

        char *key1 = make_cstring_from_bytes(Data + ((Size > 5) ? 5 : 0), (q1_len <= Size) ? q1_len : 0);
        char *key2 = make_cstring_from_bytes(Data + ((Size > 6) ? 6 : 0), (q2_len <= Size) ? q2_len : 0);
        char *key3 = make_cstring_from_bytes(Data + ((Size > 7) ? 7 : 0), (q3_len <= Size) ? q3_len : 0);
        char *key4 = make_cstring_from_bytes(Data + ((Size > 8) ? 8 : 0), (q4_len <= Size) ? q4_len : 0);
        char *key5 = make_cstring_from_bytes(Data + ((Size > 9) ? 9 : 0), (q5_len <= Size) ? q5_len : 0);

        if (key1 == NULL) key1 = make_cstring_from_bytes((const uint8_t *)"a", 1);
        if (key2 == NULL) key2 = make_cstring_from_bytes((const uint8_t *)"b", 1);
        if (key3 == NULL) key3 = make_cstring_from_bytes((const uint8_t *)"c", 1);
        if (key4 == NULL) key4 = make_cstring_from_bytes((const uint8_t *)"x", 1);
        if (key5 == NULL) key5 = make_cstring_from_bytes((const uint8_t *)"a", 1);

        cJSON *item1 = cJSON_GetObjectItemCaseSensitive(root, key1);
        (void)cJSON_IsString(item1);

        cJSON *item2 = cJSON_GetObjectItemCaseSensitive(root, key2);
        (void)cJSON_IsTrue(item2);

        cJSON *item3 = cJSON_GetObjectItemCaseSensitive(root, key3);
        cJSON *item4 = cJSON_GetObjectItemCaseSensitive(root, key4);

        cJSON *dup = cJSON_Duplicate((item3 != NULL) ? item3 : root, 1);

        cJSON *item5 = cJSON_GetObjectItemCaseSensitive(dup, key4);
        cJSON *item6 = cJSON_GetObjectItemCaseSensitive(root, key5);

        (void)cJSON_Compare((item5 != NULL) ? item5 : dup, (item6 != NULL) ? item6 : root, (Size > 0) ? (Data[0] & 1) : 0);

        cJSON_Delete(dup);

        free(key1);
        free(key2);
        free(key3);
        free(key4);
        free(key5);
    }

    cJSON_Delete(root);
    free(json_buf);
    return 0;
}