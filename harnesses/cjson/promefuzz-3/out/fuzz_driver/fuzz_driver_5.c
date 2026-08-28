// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2246:22 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2198:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2198:22 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
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

static double read_double_from_data(const uint8_t *Data, size_t Size, size_t offset) {
    double value = 0.0;
    size_t copy_size = 0;

    if (offset < Size) {
        copy_size = Size - offset;
        if (copy_size > sizeof(value)) {
            copy_size = sizeof(value);
        }
        memcpy(&value, Data + offset, copy_size);
    }

    return value;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *child = NULL;
    char *printed = NULL;

    const char *default_key1 = "k1";
    const char *default_key2 = "arr";
    const char *default_key3 = "n1";
    const char *default_key4 = "n2";
    const char *default_str = "";

    char *buf = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *key4 = NULL;
    char *strval = NULL;

    if (Size > 0) {
        buf = (char *)malloc(Size + 5);
        if (buf != NULL) {
            memcpy(buf, Data, Size);
            buf[Size] = '\0';
            buf[Size + 1] = '\0';
            buf[Size + 2] = '\0';
            buf[Size + 3] = '\0';
            buf[Size + 4] = '\0';

            key1 = buf;
            key2 = buf + (Size / 4);
            key3 = buf + (Size / 2);
            key4 = buf + ((Size * 3) / 4);
            strval = buf + (Size / 8);

            default_key1 = key1;
            default_key2 = key2;
            default_key3 = key3;
            default_key4 = key4;
            default_str = strval;
        }
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        free(buf);
        return 0;
    }

    (void)cJSON_AddStringToObject(root, default_key1, default_str);

    array = cJSON_AddArrayToObject(root, default_key2);

    child = cJSON_CreateObject();
    if (child != NULL) {
        (void)cJSON_AddNumberToObject(child, default_key3, read_double_from_data(Data, Size, 0));
        (void)cJSON_AddNumberToObject(child, default_key4, read_double_from_data(Data, Size, sizeof(double)));

        if (array != NULL) {
            if (!cJSON_AddItemToArray(array, child)) {
                cJSON_Delete(child);
            }
        } else {
            cJSON_Delete(child);
        }
    }

    printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
    }

    cJSON_Delete(root);
    free(buf);
    return 0;
}