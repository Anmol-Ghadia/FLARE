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
    char *input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }
    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    size_t quarter = Size / 4;
    size_t half = Size / 2;
    size_t three_quarter = (Size * 3) / 4;

    char *s1 = (char *)malloc(quarter + 1);
    char *s2 = (char *)malloc((half - quarter) + 1);
    char *k1 = (char *)malloc((three_quarter - half) + 1);
    char *k2 = (char *)malloc((Size - three_quarter) + 1);

    if (s1 == NULL || s2 == NULL || k1 == NULL || k2 == NULL) {
        free(input);
        free(s1);
        free(s2);
        free(k1);
        free(k2);
        return 0;
    }

    memcpy(s1, Data, quarter);
    s1[quarter] = '\0';

    memcpy(s2, Data + quarter, half - quarter);
    s2[half - quarter] = '\0';

    memcpy(k1, Data + half, three_quarter - half);
    k1[three_quarter - half] = '\0';

    memcpy(k2, Data + three_quarter, Size - three_quarter);
    k2[Size - three_quarter] = '\0';

    cJSON *root = cJSON_Parse(input);

    if (root == NULL) {
        root = cJSON_Parse("{}");
    }

    if (root != NULL) {
        cJSON *item1 = cJSON_CreateString(s1);
        cJSON *item2 = cJSON_CreateStringReference(s2);

        if (item1 != NULL) {
            if (!cJSON_AddItemToObject(root, k1, item1)) {
                cJSON_Delete(item1);
                item1 = NULL;
            }
        }

        if (item2 != NULL) {
            if (!cJSON_AddItemToObject(root, k2, item2)) {
                cJSON_Delete(item2);
                item2 = NULL;
            }
        }

        if (item1 != NULL) {
            (void)cJSON_SetValuestring(item1, s2);
        }

        cJSON *g1 = cJSON_GetObjectItem(root, k1);
        cJSON *g2 = cJSON_GetObjectItem(root, k2);

        if (g1 != NULL) {
            (void)cJSON_SetValuestring(g1, s1);
        }

        cJSON *g3 = cJSON_GetObjectItem(root, "");
        cJSON *g4 = cJSON_GetObjectItem(root, input);

        if (g2 != NULL) {
            (void)cJSON_SetValuestring(g2, s1);
        }

        cJSON *g5 = cJSON_GetObjectItem(root, "a");
        cJSON *g6 = cJSON_GetObjectItem(root, "A");

        if (g3 != NULL) {
            (void)cJSON_SetValuestring(g3, s2);
        }

        (void)g4;
        (void)g5;
        (void)g6;

        cJSON_Delete(root);
    }

    free(input);
    free(s1);
    free(s2);
    free(k1);
    free(k2);
    return 0;
}