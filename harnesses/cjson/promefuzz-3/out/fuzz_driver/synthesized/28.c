// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2019:26 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2286:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2267:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddObjectToObject at cJSON.c:2192:22 in cJSON.h
// cJSON_AddArrayToObject at cJSON.c:2204:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2281:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2286:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
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
#include "cJSON.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*offset < size) {
            v |= data[*offset];
            (*offset)++;
        }
    }
    return v;
}

static double read_double_like(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t bits = 0;
    size_t i;
    union {
        uint64_t u;
        double d;
    } conv;

    for (i = 0; i < 8; ++i) {
        bits <<= 8;
        if (*offset < size) {
            bits |= data[*offset];
            (*offset)++;
        }
    }

    conv.u = bits;
    if (isnan(conv.d) || isinf(conv.d)) {
        return (double)(int32_t)bits;
    }
    return conv.d;
}

static char *make_string(const uint8_t *data, size_t size, size_t *offset) {
    size_t remaining, len, i;
    char *out;

    if (*offset >= size) {
        out = (char *)malloc(1);
        if (out != NULL) {
            out[0] = '\0';
        }
        return out;
    }

    remaining = size - *offset;
    len = (size_t)(data[*offset] % (remaining + 1));
    (*offset)++;

    if (len > size - *offset) {
        len = size - *offset;
    }

    out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }

    for (i = 0; i < len; ++i) {
        unsigned char c = data[*offset + i];
        if (c == '\0') {
            c = 'A';
        }
        out[i] = (char)c;
    }
    out[len] = '\0';
    *offset += len;
    return out;
}

int LLVMFuzzerTestOneInput_28(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *root = NULL;
    cJSON *detached = NULL;
    int iterations, i;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    iterations = (Size > 0) ? (int)(Data[offset++] % 32) : 0;

    for (i = 0; i < iterations; ++i) {
        uint8_t op = 0;
        char *name1 = NULL;
        char *name2 = NULL;
        cJSON *tmp = NULL;
        double num;

        if (offset < Size) {
            op = Data[offset++];
        }

        name1 = make_string(Data, Size, &offset);
        if (name1 == NULL) {
            break;
        }

        switch (op % 6) {
            case 0:
                tmp = cJSON_AddObjectToObject(root, name1);
                if (tmp != NULL) {
                    name2 = make_string(Data, Size, &offset);
                    if (name2 != NULL) {
                        (void)cJSON_AddNumberToObject(tmp, name2, read_double_like(Data, Size, &offset));
                        free(name2);
                        name2 = NULL;
                    }
                }
                break;

            case 1:
                tmp = cJSON_AddArrayToObject(root, name1);
                if (tmp != NULL) {
                    name2 = make_string(Data, Size, &offset);
                    if (name2 != NULL) {
                        cJSON *obj_in_array = cJSON_CreateObject();
                        if (obj_in_array != NULL) {
                            cJSON_AddItemToArray(tmp, obj_in_array);
                            (void)cJSON_AddNumberToObject(obj_in_array, name2, read_double_like(Data, Size, &offset));
                        }
                        free(name2);
                        name2 = NULL;
                    }
                }
                break;

            case 2:
                num = read_double_like(Data, Size, &offset);
                (void)cJSON_AddNumberToObject(root, name1, num);
                break;

            case 3:
                cJSON_DeleteItemFromObjectCaseSensitive(root, name1);
                break;

            case 4:
                cJSON_DeleteItemFromObject(root, name1);
                break;

            case 5:
                detached = cJSON_DetachItemFromObject(root, name1);
                if (detached != NULL) {
                    if (offset < Size && (Data[offset++] & 1)) {
                        name2 = make_string(Data, Size, &offset);
                        if (name2 != NULL) {
                            cJSON_AddItemToObject(root, name2, detached);
                            detached = NULL;
                            free(name2);
                            name2 = NULL;
                        }
                    }
                    if (detached != NULL) {
                        cJSON_Delete(detached);
                        detached = NULL;
                    }
                }
                break;
        }

        free(name1);
    }

    while (offset < Size) {
        char *name = make_string(Data, Size, &offset);
        if (name == NULL) {
            break;
        }

        switch (read_u32(Data, Size, &offset) % 5) {
            case 0:
                (void)cJSON_AddObjectToObject(root, name);
                break;
            case 1:
                (void)cJSON_AddArrayToObject(root, name);
                break;
            case 2:
                (void)cJSON_AddNumberToObject(root, name, read_double_like(Data, Size, &offset));
                break;
            case 3:
                cJSON_DeleteItemFromObject(root, name);
                break;
            case 4:
                cJSON_DeleteItemFromObjectCaseSensitive(root, name);
                break;
        }

        free(name);
    }

    cJSON_Delete(root);
    return 0;
}