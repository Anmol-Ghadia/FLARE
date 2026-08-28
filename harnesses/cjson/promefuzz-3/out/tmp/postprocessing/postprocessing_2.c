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

static double read_double_from_bytes(const uint8_t *data, size_t size, size_t offset) {
    union {
        uint64_t u64;
        double d;
    } conv;
    conv.u64 = 0;

    for (size_t i = 0; i < 8 && (offset + i) < size; ++i) {
        conv.u64 |= ((uint64_t)data[offset + i]) << (8 * i);
    }

    return conv.d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t split1 = 0;
    size_t split2 = 0;
    const char *key1 = "str";
    const char *key2 = "arr";
    const char *key3 = "num1";
    const char *key4 = "num2";
    char *dyn_key1 = NULL;
    char *dyn_key2 = NULL;
    char *dyn_key3 = NULL;
    char *dyn_key4 = NULL;
    char *str_value = NULL;
    cJSON *root = NULL;
    cJSON *str_item = NULL;
    cJSON *array_item = NULL;
    cJSON *nested_obj = NULL;
    cJSON *num_item1 = NULL;
    cJSON *num_item2 = NULL;
    char *printed = NULL;

    if (Size > 0) {
        split1 = Data[0] % (Size + 1);
    }
    if (Size > 1) {
        split2 = Data[1] % (Size + 1);
    }
    if (split2 < split1) {
        size_t tmp = split1;
        split1 = split2;
        split2 = tmp;
    }

    dyn_key1 = (char *)malloc(split1 + 1);
    if (dyn_key1 != NULL) {
        if (split1 > 0) {
            memcpy(dyn_key1, Data, split1);
        }
        dyn_key1[split1] = '\0';
        key1 = dyn_key1;
    }

    dyn_key2 = (char *)malloc((split2 - split1) + 1);
    if (dyn_key2 != NULL) {
        if (split2 > split1) {
            memcpy(dyn_key2, Data + split1, split2 - split1);
        }
        dyn_key2[split2 - split1] = '\0';
        key2 = dyn_key2;
    }

    dyn_key3 = (char *)malloc((Size - split2) + 1);
    if (dyn_key3 != NULL) {
        if (Size > split2) {
            memcpy(dyn_key3, Data + split2, Size - split2);
        }
        dyn_key3[Size - split2] = '\0';
        key3 = dyn_key3;
    }

    dyn_key4 = (char *)malloc(Size + 1);
    if (dyn_key4 != NULL) {
        if (Size > 0) {
            memcpy(dyn_key4, Data, Size);
        }
        dyn_key4[Size] = '\0';
        key4 = dyn_key4;
    }

    str_value = (char *)malloc(Size + 1);
    if (str_value == NULL) {
        free(dyn_key1);
        free(dyn_key2);
        free(dyn_key3);
        free(dyn_key4);
        return 0;
    }
    if (Size > 0) {
        memcpy(str_value, Data, Size);
    }
    str_value[Size] = '\0';

    root = cJSON_CreateObject();
    if (root == NULL) {
        free(str_value);
        free(dyn_key1);
        free(dyn_key2);
        free(dyn_key3);
        free(dyn_key4);
        return 0;
    }

    str_item = cJSON_CreateString(str_value);
    if (str_item != NULL) {
        if (!cJSON_AddItemToObject(root, key1, str_item)) {
            cJSON_Delete(str_item);
            str_item = NULL;
        }
    }

    array_item = cJSON_CreateArray();
    if (array_item != NULL) {
        if (!cJSON_AddItemToObject(root, key2, array_item)) {
            cJSON_Delete(array_item);
            array_item = NULL;
        }
    }

    nested_obj = cJSON_CreateObject();
    if (nested_obj != NULL && array_item != NULL) {
        if (!cJSON_AddItemToArray(array_item, nested_obj)) {
            cJSON_Delete(nested_obj);
            nested_obj = NULL;
        }
    } else if (nested_obj != NULL) {
        cJSON_Delete(nested_obj);
        nested_obj = NULL;
    }

    num_item1 = cJSON_CreateNumber(read_double_from_bytes(Data, Size, 0));
    if (num_item1 != NULL && nested_obj != NULL) {
        if (!cJSON_AddItemToObject(nested_obj, key3, num_item1)) {
            cJSON_Delete(num_item1);
            num_item1 = NULL;
        }
    } else if (num_item1 != NULL) {
        cJSON_Delete(num_item1);
        num_item1 = NULL;
    }

    num_item2 = cJSON_CreateNumber(read_double_from_bytes(Data, Size, 8));
    if (num_item2 != NULL && nested_obj != NULL) {
        if (!cJSON_AddItemToObject(nested_obj, key4, num_item2)) {
            cJSON_Delete(num_item2);
            num_item2 = NULL;
        }
    } else if (num_item2 != NULL) {
        cJSON_Delete(num_item2);
        num_item2 = NULL;
    }

    printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
    }

    cJSON_Delete(root);

    free(str_value);
    free(dyn_key1);
    free(dyn_key2);
    free(dyn_key3);
    free(dyn_key4);

    return 0;
}