// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1232:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateFalse at cJSON.c:2483:23 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2972:26 in cJSON.h
// cJSON_IsNull at cJSON.c:3012:26 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2982:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2972:26 in cJSON.h
// cJSON_IsNull at cJSON.c:3012:26 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2982:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2972:26 in cJSON.h
// cJSON_IsNull at cJSON.c:3012:26 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2982:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
// cJSON_Compare at cJSON.c:3072:26 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size) {
    return (int)read_u32(data, size);
}

static char *dup_from_input(const uint8_t **data, size_t *size) {
    if (*size == 0) {
        return NULL;
    }

    size_t max_len = *size - 1;
    size_t len = (*data)[0] % (max_len + 1);
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(s, *data + 1, len);
    }
    s[len] = '\0';

    *data += 1 + len;
    *size -= 1 + len;
    return s;
}

static cJSON *make_manual_item(const uint8_t **data, size_t *size) {
    cJSON *item = cJSON_CreateObject();
    char *valuestring = NULL;
    char *string = NULL;

    if (item == NULL) {
        return NULL;
    }

    item->type = read_int(data, size);

    valuestring = dup_from_input(data, size);
    string = dup_from_input(data, size);

    item->valueint = read_int(data, size);
    item->valuedouble = (double)read_int(data, size);

    item->valuestring = valuestring;
    item->string = string;

    return item;
}

static void free_manual_item(cJSON *item) {
    if (item == NULL) {
        return;
    }

    free(item->valuestring);
    item->valuestring = NULL;
    free(item->string);
    item->string = NULL;
    cJSON_Delete(item);
}

int LLVMFuzzerTestOneInput_23(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON_IsInvalid(NULL);
    cJSON_IsNull(NULL);
    cJSON_IsString(NULL);
    cJSON_IsFalse(NULL);
    cJSON_IsNumber(NULL);
    cJSON_Compare(NULL, NULL, 0);
    cJSON_Compare(NULL, NULL, 1);

    char *json_buf = (char *)malloc(Size + 1);
    if (json_buf == NULL) {
        return 0;
    }
    memcpy(json_buf, Data, Size);
    json_buf[Size] = '\0';

    cJSON *parsed1 = cJSON_Parse(json_buf);
    cJSON *parsed2 = cJSON_ParseWithLength((const char *)Data, Size);

    cJSON *manual1 = make_manual_item(&ptr, &remaining);
    cJSON *manual2 = make_manual_item(&ptr, &remaining);

    cJSON *extra_string = NULL;
    cJSON *extra_number = NULL;

    if (remaining > 0) {
        extra_string = cJSON_CreateString((const char *)ptr);
    } else {
        extra_string = cJSON_CreateNull();
    }

    if (remaining > 1) {
        extra_number = cJSON_CreateNumber((double)ptr[0]);
    } else {
        extra_number = cJSON_CreateFalse();
    }

    cJSON *items[6];
    items[0] = parsed1;
    items[1] = parsed2;
    items[2] = manual1;
    items[3] = manual2;
    items[4] = extra_string;
    items[5] = extra_number;

    for (size_t i = 0; i < 6; ++i) {
        cJSON_IsInvalid(items[i]);
        cJSON_IsNull(items[i]);
        cJSON_IsString(items[i]);
        cJSON_IsFalse(items[i]);
        cJSON_IsNumber(items[i]);
    }

    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            cJSON_Compare(items[i], items[j], 0);
            cJSON_Compare(items[i], items[j], 1);
        }
    }

    if (parsed1 != NULL) {
        cJSON *dup = cJSON_Duplicate(parsed1, 1);
        if (dup != NULL) {
            cJSON_Compare(parsed1, dup, 0);
            cJSON_Compare(parsed1, dup, 1);
            cJSON_IsInvalid(dup);
            cJSON_IsNull(dup);
            cJSON_IsString(dup);
            cJSON_IsFalse(dup);
            cJSON_IsNumber(dup);
            cJSON_Delete(dup);
        }
    }

    if (parsed2 != NULL) {
        cJSON *dup = cJSON_Duplicate(parsed2, 1);
        if (dup != NULL) {
            cJSON_Compare(parsed2, dup, 0);
            cJSON_Compare(parsed2, dup, 1);
            cJSON_Delete(dup);
        }
    }

    cJSON_Delete(extra_string);
    cJSON_Delete(extra_number);
    cJSON_Delete(parsed1);
    cJSON_Delete(parsed2);
    free_manual_item(manual1);
    free_manual_item(manual2);
    free(json_buf);

    return 0;
}