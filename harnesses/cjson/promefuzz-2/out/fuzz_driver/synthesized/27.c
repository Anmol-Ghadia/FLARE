// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2548:23 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_SetValuestring at cJSON.c:435:21 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
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

static char *make_cstring(const uint8_t *data, size_t size) {
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

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size) {
    char *input = NULL;
    char *alt = NULL;
    char *key = NULL;
    cJSON *str_item = NULL;
    cJSON *ref_item = NULL;
    cJSON *obj = NULL;
    cJSON *added = NULL;
    char *ret = NULL;
    FILE *fp = NULL;

    if (Data == NULL) {
        return 0;
    }

    input = make_cstring(Data, Size);
    if (input == NULL) {
        return 0;
    }

    {
        size_t alt_size = (Size > 0) ? Size : 1;
        alt = (char *)malloc(alt_size + 1);
        if (alt != NULL) {
            size_t i;
            for (i = 0; i < alt_size; i++) {
                uint8_t b = (i < Size) ? Data[i] : 0;
                alt[i] = (char)((b % 95) + 32);
            }
            alt[alt_size] = '\0';
        }
    }

    {
        size_t key_len = (Size % 32) + 1;
        key = (char *)malloc(key_len + 1);
        if (key != NULL) {
            size_t i;
            for (i = 0; i < key_len; i++) {
                uint8_t b = (i < Size) ? Data[i] : (uint8_t)i;
                key[i] = (char)('a' + (b % 26));
            }
            key[key_len] = '\0';
        }
    }

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    (void)cJSON_IsString(NULL);
    (void)cJSON_GetStringValue(NULL);
    (void)cJSON_SetValuestring(NULL, input);

    str_item = cJSON_CreateString(input);
    if (str_item != NULL) {
        (void)cJSON_IsString(str_item);
        (void)cJSON_GetStringValue(str_item);

        ret = cJSON_SetValuestring(str_item, input);
        (void)ret;

        if (alt != NULL) {
            ret = cJSON_SetValuestring(str_item, alt);
            (void)ret;
            (void)cJSON_GetStringValue(str_item);
        }

        if (str_item->valuestring != NULL) {
            ret = cJSON_SetValuestring(str_item, str_item->valuestring);
            (void)ret;
        }
    }

    ref_item = cJSON_CreateStringReference(input);
    if (ref_item != NULL) {
        (void)cJSON_IsString(ref_item);
        (void)cJSON_GetStringValue(ref_item);
        if (alt != NULL) {
            ret = cJSON_SetValuestring(ref_item, alt);
            (void)ret;
        }
    }

    obj = cJSON_CreateObject();
    if (obj != NULL) {
        const char *obj_key = (key != NULL) ? key : "k";
        const char *obj_val = (alt != NULL) ? alt : input;

        added = cJSON_AddStringToObject(obj, obj_key, obj_val);
        if (added != NULL) {
            (void)cJSON_IsString(added);
            (void)cJSON_GetStringValue(added);

            ret = cJSON_SetValuestring(added, input);
            (void)ret;

            if (alt != NULL) {
                ret = cJSON_SetValuestring(added, alt);
                (void)ret;
            }
        }

        (void)cJSON_AddStringToObject(obj, "", input);
        (void)cJSON_AddStringToObject(obj, obj_key, "");
        (void)cJSON_AddStringToObject(obj, obj_key, obj_val);
    }

    cJSON_Delete(obj);
    cJSON_Delete(ref_item);
    cJSON_Delete(str_item);

    free(key);
    free(alt);
    free(input);

    return 0;
}