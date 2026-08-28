// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:384:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "cJSON.h"

static double read_double_from_data(const uint8_t *data, size_t size, size_t offset) {
    double value = 0.0;
    if (data == NULL || offset >= size) {
        return value;
    }

    size_t remaining = size - offset;
    size_t copy_size = remaining < sizeof(double) ? remaining : sizeof(double);
    memcpy(&value, data + offset, copy_size);
    return value;
}

static char *make_key_from_data(const uint8_t *data, size_t size, size_t start, size_t len) {
    char *key = (char *)malloc(len + 1);
    if (key == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (start + i < size) ? data[start + i] : 0;
        if (c == '\0') {
            c = 'A';
        }
        key[i] = (char)c;
    }
    key[len] = '\0';
    return key;
}

int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Data != NULL && Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    double num1 = read_double_from_data(Data, Size, 0);
    double num2 = read_double_from_data(Data, Size, sizeof(double));
    double num3 = read_double_from_data(Data, Size, sizeof(double) * 2);

    cJSON *standalone = cJSON_CreateNumber(num1);
    if (standalone != NULL) {
        (void)cJSON_GetNumberValue(standalone);
        (void)cJSON_SetNumberHelper(standalone, num2);
        (void)cJSON_GetNumberValue(standalone);

        char *printed_num = cJSON_Print(standalone);
        if (printed_num != NULL) {
            free(printed_num);
        }
        cJSON_Delete(standalone);
    }

    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) {
        return 0;
    }

    size_t key_len1 = (Size > 0) ? (Data[0] % 32) : 0;
    size_t key_len2 = (Size > 1) ? (Data[1] % 32) : 0;
    size_t key_len3 = (Size > 2) ? (Data[2] % 32) : 0;

    char *key1 = make_key_from_data(Data, Size, 3, key_len1);
    char *key2 = make_key_from_data(Data, Size, 3 + key_len1, key_len2);
    char *key3 = make_key_from_data(Data, Size, 3 + key_len1 + key_len2, key_len3);

    if (key1 != NULL) {
        cJSON *item1 = cJSON_AddNumberToObject(obj, key1, num1);
        if (item1 != NULL) {
            (void)cJSON_GetNumberValue(item1);
            (void)cJSON_SetNumberHelper(item1, num2);
            (void)cJSON_GetNumberValue(item1);
        }
    }

    if (key2 != NULL) {
        cJSON *item2 = cJSON_AddNumberToObject(obj, key2, num2);
        if (item2 != NULL) {
            (void)cJSON_GetNumberValue(item2);
            (void)cJSON_SetNumberHelper(item2, num3);
            (void)cJSON_GetNumberValue(item2);
        }
    }

    if (key3 != NULL) {
        cJSON *item3 = cJSON_AddNumberToObject(obj, key3, num3);
        if (item3 != NULL) {
            (void)cJSON_GetNumberValue(item3);
            (void)cJSON_SetNumberHelper(item3, num1);
            (void)cJSON_GetNumberValue(item3);
        }
    }

    char *printed_obj = cJSON_Print(obj);
    if (printed_obj != NULL) {
        free(printed_obj);
    }

    free(key1);
    free(key2);
    free(key3);
    cJSON_Delete(obj);

    return 0;
}