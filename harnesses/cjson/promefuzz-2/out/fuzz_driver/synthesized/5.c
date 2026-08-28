// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2246:22 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2198:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2198:22 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "cJSON.h"

int LLVMFuzzerTestOneInput_5(const uint8_t *Data, size_t Size) {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    size_t split1 = (Size > 0) ? (Data[0] % (Size + 1)) : 0;
    size_t split2 = (Size > 1) ? (Data[1] % (Size + 1)) : 0;
    if (split1 > split2) {
        size_t tmp = split1;
        split1 = split2;
        split2 = tmp;
    }
    if (split2 > Size) {
        split2 = Size;
    }

    size_t key1_len = split1;
    size_t val1_len = split2 - split1;
    size_t key2_len = Size - split2;

    char *key1 = (char *)malloc(key1_len + 2);
    char *val1 = (char *)malloc(val1_len + 1);
    char *key2 = (char *)malloc(key2_len + 2);

    if (key1 == NULL || val1 == NULL || key2 == NULL) {
        free(key1);
        free(val1);
        free(key2);
        cJSON_Delete(root);
        return 0;
    }

    if (key1_len > 0) {
        memcpy(key1, Data, key1_len);
    }
    key1[key1_len] = '\0';

    if (val1_len > 0) {
        memcpy(val1, Data + split1, val1_len);
    }
    val1[val1_len] = '\0';

    if (key2_len > 0) {
        memcpy(key2, Data + split2, key2_len);
    }
    key2[key2_len] = '\0';

    if (key1_len == 0) {
        key1[0] = 'k';
        key1[1] = '\0';
    }
    if (key2_len == 0) {
        key2[0] = 'a';
        key2[1] = '\0';
    }

    (void)cJSON_AddStringToObject(root, key1, val1);

    cJSON *array = cJSON_AddArrayToObject(root, key2);
    if (array != NULL) {
        cJSON *child = cJSON_CreateObject();
        if (child != NULL) {
            double num1 = 0.0;
            double num2 = 0.0;

            if (Size >= 8) {
                uint64_t u1 = 0;
                memcpy(&u1, Data, 8);
                num1 = (double)(int64_t)u1;
            } else {
                for (size_t i = 0; i < Size; ++i) {
                    num1 = (num1 * 257.0) + Data[i];
                }
            }

            if (Size >= 16) {
                uint64_t u2 = 0;
                memcpy(&u2, Data + 8, 8);
                num2 = (double)(int64_t)u2;
            } else {
                for (size_t i = 0; i < Size; ++i) {
                    num2 = (num2 * 131.0) - Data[i];
                }
            }

            (void)cJSON_AddNumberToObject(child, "n1", num1);
            (void)cJSON_AddNumberToObject(child, "n2", num2);
            if (!cJSON_AddItemToArray(array, child)) {
                cJSON_Delete(child);
            }
        }
    }

    char *printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
    }

    free(key1);
    free(val1);
    free(key2);
    cJSON_Delete(root);
    return 0;
}