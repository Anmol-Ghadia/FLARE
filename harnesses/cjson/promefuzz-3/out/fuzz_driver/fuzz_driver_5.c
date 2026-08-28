// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2168:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
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

static char *dup_bytes_as_cstring(const uint8_t *data, size_t len) {
    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(out, data, len);
    }
    out[len] = '\0';
    return out;
}

static double read_double_from_data(const uint8_t *data, size_t size, size_t offset) {
    double result = 0.0;
    size_t copy_len = 0;
    if (offset < size) {
        copy_len = size - offset;
        if (copy_len > sizeof(double)) {
            copy_len = sizeof(double);
        }
        memcpy(&result, data + offset, copy_len);
    }
    return result;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t split1 = 0;
    size_t split2 = 0;
    char *str_value = NULL;
    char *print1 = NULL;
    char *print2 = NULL;
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *child = NULL;

    if (Data == NULL) {
        return 0;
    }

    split1 = Size / 3;
    split2 = (Size * 2) / 3;

    str_value = dup_bytes_as_cstring(Data, split1);
    if (str_value == NULL) {
        return 0;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        free(str_value);
        return 0;
    }

    (void)cJSON_AddStringToObject(root, "key", str_value);

    array = cJSON_AddArrayToObject(root, "array");
    if (array != NULL) {
        child = cJSON_CreateObject();
        if (child != NULL) {
            double num1 = read_double_from_data(Data, Size, split1);
            double num2 = read_double_from_data(Data, Size, split2);

            (void)cJSON_AddNumberToObject(child, "num1", num1);
            (void)cJSON_AddNumberToObject(child, "num2", num2);

            if (!cJSON_AddItemToArray(array, child)) {
                cJSON_Delete(child);
                child = NULL;
            }
        }
    }

    print1 = cJSON_Print(root);
    if (print1 != NULL) {
        free(print1);
        print1 = NULL;
    }

    if (array != NULL) {
        cJSON *child2 = cJSON_CreateObject();
        if (child2 != NULL) {
            double alt1 = (double)Size;
            double alt2 = (Size > 0) ? (double)Data[Size - 1] : 0.0;

            (void)cJSON_AddNumberToObject(child2, "num1", alt1);
            (void)cJSON_AddNumberToObject(child2, "num2", alt2);

            if (!cJSON_AddItemToArray(array, child2)) {
                cJSON_Delete(child2);
                child2 = NULL;
            }
        }
    }

    print2 = cJSON_Print(root);
    if (print2 != NULL) {
        free(print2);
        print2 = NULL;
    }

    cJSON_Delete(root);
    free(str_value);
    return 0;
}