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

static double read_double_from_data(const uint8_t *data, size_t size, size_t offset) {
    double out = 0.0;
    if (data == NULL || size <= offset) {
        return 0.0;
    }

    uint64_t bits = 0;
    size_t remaining = size - offset;
    size_t copy_size = remaining < sizeof(bits) ? remaining : sizeof(bits);
    memcpy(&bits, data + offset, copy_size);

    memcpy(&out, &bits, sizeof(out));
    return out;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *child_object = NULL;
    cJSON *printed_owner = NULL;
    char *printed = NULL;

    const char *key1 = "s";
    const char *key2 = "a";
    const char *key3 = "n1";
    const char *key4 = "n2";

    char *string_value = NULL;
    size_t string_len = 0;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    string_value = (char *)malloc(Size + 1);
    if (string_value == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    if (Size > 0) {
        memcpy(string_value, Data, Size);
    }
    string_value[Size] = '\0';
    string_len = Size;

    if (string_len > 0) {
        key1 = (Data[0] & 1) ? "string" : "s";
        key2 = (Data[0] & 2) ? "array" : "a";
        key3 = (Data[0] & 4) ? "first" : "n1";
        key4 = (Data[0] & 8) ? "second" : "n2";
    }

    (void)cJSON_AddStringToObject(root, key1, string_value);

    array = cJSON_AddArrayToObject(root, key2);

    child_object = cJSON_CreateObject();
    if (child_object != NULL) {
        double num1 = read_double_from_data(Data, Size, 0);
        double num2 = read_double_from_data(Data, Size, sizeof(double));

        if (Size > 0) {
            switch (Data[0] % 6) {
                case 0:
                    num1 = 0.0;
                    num2 = -0.0;
                    break;
                case 1:
                    num1 = 1.0;
                    num2 = -1.0;
                    break;
                case 2:
                    num1 = 1e308;
                    num2 = -1e308;
                    break;
                case 3:
                    num1 = 1e-308;
                    num2 = -1e-308;
                    break;
                case 4:
                    num1 = (double)(int8_t)Data[0];
                    num2 = (double)(uint8_t)(Size > 1 ? Data[1] : Data[0]);
                    break;
                default:
                    break;
            }
        }

        (void)cJSON_AddNumberToObject(child_object, key3, num1);
        (void)cJSON_AddNumberToObject(child_object, key4, num2);

        if (array != NULL) {
            if (!cJSON_AddItemToArray(array, child_object)) {
                cJSON_Delete(child_object);
                child_object = NULL;
            }
        } else {
            cJSON_Delete(child_object);
            child_object = NULL;
        }
    }

    printed_owner = (Size > 0 && (Data[0] & 0x10) && array != NULL) ? array : root;
    printed = cJSON_Print(printed_owner);
    if (printed != NULL) {
        free(printed);
    }

    free(string_value);
    cJSON_Delete(root);
    return 0;
}