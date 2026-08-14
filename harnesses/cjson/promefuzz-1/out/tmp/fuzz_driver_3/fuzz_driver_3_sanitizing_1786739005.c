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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *json_buffer;
    cJSON *root = NULL;
    cJSON *dup = NULL;
    cJSON *item1, *item2, *item3, *item4, *item5;
    char key1[32], key2[32], key3[32], key4[32], key5[32];
    cJSON_bool recurse;
    cJSON_bool case_sensitive;

    json_buffer = (char *)malloc(Size + 1);
    if (json_buffer == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(json_buffer, Data, Size);
    }
    json_buffer[Size] = '\0';

    root = cJSON_Parse(json_buffer);
    if (root == NULL) {
        free(json_buffer);
        return 0;
    }

    if (Size == 0) {
        key1[0] = key2[0] = key3[0] = key4[0] = key5[0] = '\0';
        recurse = 0;
        case_sensitive = 0;
    } else {
        size_t i;
        for (i = 0; i < sizeof(key1) - 1; i++) {
            key1[i] = (char)Data[i % Size];
            if (key1[i] == '\0') key1[i] = 'A';
        }
        key1[sizeof(key1) - 1] = '\0';

        for (i = 0; i < sizeof(key2) - 1; i++) {
            key2[i] = (char)Data[(i + 1) % Size];
            if (key2[i] == '\0') key2[i] = 'B';
        }
        key2[sizeof(key2) - 1] = '\0';

        for (i = 0; i < sizeof(key3) - 1; i++) {
            key3[i] = (char)Data[(i + 2) % Size];
            if (key3[i] == '\0') key3[i] = 'C';
        }
        key3[sizeof(key3) - 1] = '\0';

        for (i = 0; i < sizeof(key4) - 1; i++) {
            key4[i] = (char)Data[(i + 3) % Size];
            if (key4[i] == '\0') key4[i] = 'D';
        }
        key4[sizeof(key4) - 1] = '\0';

        for (i = 0; i < sizeof(key5) - 1; i++) {
            key5[i] = (char)Data[(i + 4) % Size];
            if (key5[i] == '\0') key5[i] = 'E';
        }
        key5[sizeof(key5) - 1] = '\0';

        recurse = (Data[0] & 1) ? 1 : 0;
        case_sensitive = (Data[0] & 2) ? 1 : 0;
    }

    item1 = cJSON_GetObjectItemCaseSensitive(root, key1);
    (void)cJSON_IsString(item1);
    item2 = cJSON_GetObjectItemCaseSensitive(root, key2);
    (void)cJSON_IsTrue(item2);
    item3 = cJSON_GetObjectItemCaseSensitive(root, key3);
    item4 = cJSON_GetObjectItemCaseSensitive(root, key4);
    dup = cJSON_Duplicate(item4 != NULL ? item4 : item3, recurse);
    item5 = cJSON_GetObjectItemCaseSensitive(root, key5);
    item4 = cJSON_GetObjectItemCaseSensitive(dup, key1);
    (void)cJSON_Compare(item5, item4, case_sensitive);
    cJSON_Delete(dup);
    cJSON_Delete(root);
    free(json_buffer);
    return 0;
}