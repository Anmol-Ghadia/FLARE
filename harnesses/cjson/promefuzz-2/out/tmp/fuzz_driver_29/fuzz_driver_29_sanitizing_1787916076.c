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

static double read_double(const uint8_t **data, size_t *size) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = 0;
    size_t n = (*size < 8) ? *size : 8;
    for (size_t i = 0; i < n; ++i) {
        conv.u = (conv.u << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return conv.d;
}

static char *read_string(const uint8_t **data, size_t *size, size_t max_len) {
    if (*size == 0) {
        char *s = (char *)malloc(1);
        if (s != NULL) {
            s[0] = '\0';
        }
        return s;
    }

    size_t len = (*data)[0] % (max_len + 1);
    *data += 1;
    *size -= 1;

    if (len > *size) {
        len = *size;
    }

    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    memcpy(s, *data, len);
    s[len] = '\0';

    *data += len;
    *size -= len;
    return s;
}

static cJSON *make_item_from_fuzz(const uint8_t **data, size_t *size) {
    int kind = 0;
    if (*size > 0) {
        kind = (*data)[0] % 6;
        *data += 1;
        *size -= 1;
    }

    switch (kind) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((*size > 0) ? ((*data)[0] & 1) : 0);
        case 2:
            return cJSON_CreateNumber(read_double(data, size));
        case 3: {
            char *s = read_string(data, size, 32);
            cJSON *item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        case 4:
            return cJSON_CreateArray();
        case 5:
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    int count = 0;
    if (remaining > 0) {
        count = (int)(ptr[0] % 32);
        ptr++;
        remaining--;
    }

    double *numbers = NULL;
    if (count > 0) {
        numbers = (double *)malloc((size_t)count * sizeof(double));
        if (numbers == NULL) {
            return 0;
        }
        for (int i = 0; i < count; ++i) {
            numbers[i] = read_double(&ptr, &remaining);
        }
    }

    cJSON *double_array = cJSON_CreateDoubleArray(numbers, count);
    free(numbers);

    cJSON *root_object = cJSON_CreateObject();
    cJSON *object_array = NULL;
    if (root_object != NULL) {
        char *name = read_string(&ptr, &remaining, 32);
        if (name != NULL) {
            object_array = cJSON_AddArrayToObject(root_object, name);
            free(name);
        }
    }

    if (object_array != NULL && double_array != NULL) {
        int initial_size = cJSON_GetArraySize(double_array);
        for (int i = 0; i < initial_size; ++i) {
            cJSON *detached = cJSON_DetachItemFromArray(double_array, 0);
            if (detached == NULL) {
                break;
            }
            if (!cJSON_AddItemToArray(object_array, detached)) {
                cJSON_Delete(detached);
            }
        }
    }

    int op_count = 0;
    if (remaining > 0) {
        op_count = ptr[0] % 32;
        ptr++;
        remaining--;
    }

    for (int i = 0; i < op_count; ++i) {
        int op = 0;
        if (remaining > 0) {
            op = ptr[0] % 6;
            ptr++;
            remaining--;
        }

        switch (op) {
            case 0: {
                cJSON *item = make_item_from_fuzz(&ptr, &remaining);
                if (item != NULL) {
                    if (!cJSON_AddItemToArray(object_array, item)) {
                        cJSON_Delete(item);
                    }
                }
                break;
            }
            case 1: {
                cJSON *item = make_item_from_fuzz(&ptr, &remaining);
                if (item != NULL) {
                    if (!cJSON_AddItemToArray(double_array, item)) {
                        cJSON_Delete(item);
                    }
                }
                break;
            }
            case 2: {
                int idx = read_int(&ptr, &remaining);
                cJSON *detached = cJSON_DetachItemFromArray(object_array, idx);
                if (detached != NULL) {
                    if (double_array != NULL) {
                        if (!cJSON_AddItemToArray(double_array, detached)) {
                            cJSON_Delete(detached);
                        }
                    } else {
                        cJSON_Delete(detached);
                    }
                }
                break;
            }
            case 3: {
                int idx = read_int(&ptr, &remaining);
                cJSON *detached = cJSON_DetachItemFromArray(double_array, idx);
                if (detached != NULL) {
                    if (object_array != NULL) {
                        if (!cJSON_AddItemToArray(object_array, detached)) {
                            cJSON_Delete(detached);
                        }
                    } else {
                        cJSON_Delete(detached);
                    }
                }
                break;
            }
            case 4: {
                int idx = read_int(&ptr, &remaining);
                cJSON_DeleteItemFromArray(object_array, idx);
                break;
            }
            case 5: {
                int idx = read_int(&ptr, &remaining);
                cJSON_DeleteItemFromArray(double_array, idx);
                break;
            }
            default:
                break;
        }

        (void)cJSON_GetArraySize(object_array);
        (void)cJSON_GetArraySize(double_array);
    }

    if (remaining > 0 && object_array != NULL) {
        int extra = ptr[0] % 8;
        ptr++;
        remaining--;
        for (int i = 0; i < extra; ++i) {
            cJSON *item = make_item_from_fuzz(&ptr, &remaining);
            if (item != NULL) {
                if (!cJSON_AddItemToArray(object_array, item)) {
                    cJSON_Delete(item);
                }
            }
        }
    }

    if (remaining > 0 && double_array != NULL) {
        int idx = (int)((int8_t)ptr[0]);
        cJSON *detached = cJSON_DetachItemFromArray(double_array, idx);
        if (detached != NULL) {
            cJSON_Delete(detached);
        }
    }

    (void)cJSON_GetArraySize(root_object);
    (void)cJSON_GetArraySize(object_array);
    (void)cJSON_GetArraySize(double_array);

    cJSON_Delete(root_object);
    cJSON_Delete(double_array);

    return 0;
}