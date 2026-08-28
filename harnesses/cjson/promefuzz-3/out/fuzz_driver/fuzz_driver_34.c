// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:411:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1983:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:411:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:411:22 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:411:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:411:22 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_GetNumberValue at cJSON.c:109:22 in cJSON.h
// cJSON_SetNumberHelper at cJSON.c:411:22 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "cJSON.h"

static double read_double(const uint8_t *data, size_t size, size_t offset) {
    double d = 0.0;
    if (offset + sizeof(double) <= size) {
        memcpy(&d, data + offset, sizeof(double));
    } else if (size > offset) {
        memcpy(&d, data + offset, size - offset);
    }
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    double num1 = read_double(Data, Size, 0);
    double num2 = read_double(Data, Size, sizeof(double));
    double num3 = read_double(Data, Size, sizeof(double) * 2);

    cJSON *n1 = cJSON_CreateNumber(num1);
    cJSON *n2 = cJSON_CreateNumber(num2);
    cJSON *n3 = cJSON_CreateNumber(num3);

    if (n1 != NULL) {
        (void)cJSON_IsNumber(n1);
        (void)cJSON_GetNumberValue(n1);
        (void)cJSON_SetNumberHelper(n1, num2);
        (void)cJSON_GetNumberValue(n1);

        cJSON *dup_shallow = cJSON_Duplicate(n1, 0);
        cJSON *dup_deep = cJSON_Duplicate(n1, 1);

        if (dup_shallow != NULL) {
            (void)cJSON_IsNumber(dup_shallow);
            (void)cJSON_GetNumberValue(dup_shallow);
            (void)cJSON_SetNumberHelper(dup_shallow, num3);
            cJSON_Delete(dup_shallow);
        }

        if (dup_deep != NULL) {
            (void)cJSON_IsNumber(dup_deep);
            (void)cJSON_GetNumberValue(dup_deep);
            cJSON_Delete(dup_deep);
        }
    }

    if (n2 != NULL) {
        (void)cJSON_IsNumber(n2);
        (void)cJSON_GetNumberValue(n2);
        (void)cJSON_SetNumberHelper(n2, num1);
    }

    if (n3 != NULL) {
        (void)cJSON_IsNumber(n3);
        (void)cJSON_GetNumberValue(n3);
        (void)cJSON_SetNumberHelper(n3, num1 + num2);
    }

    cJSON *obj = cJSON_CreateObject();
    if (obj != NULL) {
        size_t remaining = (Size > (sizeof(double) * 3)) ? (Size - (sizeof(double) * 3)) : 0;
        const uint8_t *key_data = Data + ((Size > (sizeof(double) * 3)) ? (sizeof(double) * 3) : Size);
        size_t key_len = remaining > 64 ? 64 : remaining;

        char key1[65];
        char key2[65];
        size_t i;

        for (i = 0; i < key_len; ++i) {
            char c = (char)key_data[i];
            if (c == '\0') {
                c = 'A';
            }
            key1[i] = c;
            if (c >= 'a' && c <= 'z') {
                key2[i] = (char)(c - 'a' + 'A');
            } else if (c >= 'A' && c <= 'Z') {
                key2[i] = (char)(c - 'A' + 'a');
            } else {
                key2[i] = c;
            }
        }
        key1[key_len] = '\0';
        key2[key_len] = '\0';

        if (n1 != NULL) {
            cJSON_AddItemToObject(obj, key1, n1);
            n1 = NULL;
        }
        if (n2 != NULL) {
            cJSON_AddItemToObject(obj, "fixed", n2);
            n2 = NULL;
        }
        if (n3 != NULL) {
            cJSON_AddItemToObject(obj, "another", n3);
            n3 = NULL;
        }

        cJSON *found1 = cJSON_GetObjectItem(obj, key1);
        cJSON *found2 = cJSON_GetObjectItem(obj, key2);
        cJSON *found3 = cJSON_GetObjectItem(obj, "fixed");
        cJSON *found4 = cJSON_GetObjectItem(obj, "another");

        if (found1 != NULL) {
            (void)cJSON_IsNumber(found1);
            (void)cJSON_GetNumberValue(found1);
            if (cJSON_IsNumber(found1)) {
                (void)cJSON_SetNumberHelper(found1, num3);
            }
        }

        if (found2 != NULL) {
            (void)cJSON_IsNumber(found2);
            (void)cJSON_GetNumberValue(found2);
        }

        if (found3 != NULL) {
            (void)cJSON_IsNumber(found3);
            (void)cJSON_GetNumberValue(found3);
            if (cJSON_IsNumber(found3)) {
                cJSON *dup = cJSON_Duplicate(found3, (Size & 1) ? 1 : 0);
                if (dup != NULL) {
                    (void)cJSON_IsNumber(dup);
                    (void)cJSON_GetNumberValue(dup);
                    cJSON_Delete(dup);
                }
            }
        }

        if (found4 != NULL) {
            (void)cJSON_IsNumber(found4);
            (void)cJSON_GetNumberValue(found4);
        }

        cJSON *obj_dup_shallow = cJSON_Duplicate(obj, 0);
        cJSON *obj_dup_deep = cJSON_Duplicate(obj, 1);

        if (obj_dup_shallow != NULL) {
            cJSON *tmp = cJSON_GetObjectItem(obj_dup_shallow, key1);
            if (tmp != NULL) {
                (void)cJSON_IsNumber(tmp);
                (void)cJSON_GetNumberValue(tmp);
            }
            cJSON_Delete(obj_dup_shallow);
        }

        if (obj_dup_deep != NULL) {
            cJSON *tmp1 = cJSON_GetObjectItem(obj_dup_deep, key2);
            cJSON *tmp2 = cJSON_GetObjectItem(obj_dup_deep, "fixed");
            if (tmp1 != NULL) {
                (void)cJSON_IsNumber(tmp1);
                (void)cJSON_GetNumberValue(tmp1);
            }
            if (tmp2 != NULL) {
                (void)cJSON_IsNumber(tmp2);
                (void)cJSON_GetNumberValue(tmp2);
                if (cJSON_IsNumber(tmp2)) {
                    (void)cJSON_SetNumberHelper(tmp2, num1 - num2);
                }
            }
            cJSON_Delete(obj_dup_deep);
        }

        cJSON_Delete(obj);
    }

    if (n1 != NULL) {
        cJSON_Delete(n1);
    }
    if (n2 != NULL) {
        cJSON_Delete(n2);
    }
    if (n3 != NULL) {
        cJSON_Delete(n3);
    }

    return 0;
}