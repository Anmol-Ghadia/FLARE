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

static cJSON *make_manual_item(const uint8_t **data, size_t *size) {
    cJSON *item = cJSON_CreateObject();
    if (item == NULL) {
        return NULL;
    }

    item->type = read_int(data, size);

    if (*size > 0) {
        size_t len = (*data)[0] % ((*size > 1) ? *size : 1);
        if (len > 0 && len < *size) {
            char *s = (char *)malloc(len + 1);
            if (s != NULL) {
                memcpy(s, *data + 1, len);
                s[len] = '\0';
                item->valuestring = s;
            }
        }
    }

    item->valueint = read_int(data, size);
    item->valuedouble = (double)read_int(data, size);

    if (*size > 0) {
        size_t len = (*data)[0] % ((*size > 1) ? *size : 1);
        if (len > 0 && len < *size) {
            char *s = (char *)malloc(len + 1);
            if (s != NULL) {
                memcpy(s, *data + 1, len);
                s[len] = '\0';
                item->string = s;
            }
        }
    }

    return item;
}

static void free_manual_item(cJSON *item) {
    if (item == NULL) {
        return;
    }
    free(item->valuestring);
    free(item->string);
    cJSON_Delete(item);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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

    cJSON *items[6];
    items[0] = parsed1;
    items[1] = parsed2;
    items[2] = manual1;
    items[3] = manual2;
    items[4] = cJSON_CreateNull();
    items[5] = cJSON_CreateFalse();

    if (remaining > 0) {
        cJSON *tmp = cJSON_CreateString((const char *)ptr);
        if (tmp != NULL) {
            items[4] = tmp;
        }
    }
    if (remaining > 1) {
        cJSON *tmp = cJSON_CreateNumber((double)(ptr[0]));
        if (tmp != NULL) {
            items[5] = tmp;
        }
    }

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

    if (items[4] != NULL && items[4] != parsed1 && items[4] != parsed2 && items[4] != manual1 && items[4] != manual2) {
        cJSON_Delete(items[4]);
    }
    if (items[5] != NULL && items[5] != parsed1 && items[5] != parsed2 && items[5] != manual1 && items[5] != manual2) {
        cJSON_Delete(items[5]);
    }

    if (parsed1 != NULL) {
        cJSON_Delete(parsed1);
    }
    if (parsed2 != NULL) {
        cJSON_Delete(parsed2);
    }
    free_manual_item(manual1);
    free_manual_item(manual2);
    free(json_buf);

    return 0;
}