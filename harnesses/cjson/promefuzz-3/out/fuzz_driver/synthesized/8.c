// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2579:23 in cJSON.h
// cJSON_GetArrayItem at cJSON.c:1899:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2077:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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

static int read_int32(const uint8_t *data, size_t size, size_t *offset) {
    int value = 0;
    if (*offset + 4 <= size) {
        value = (int)(
            ((unsigned int)data[*offset]) |
            ((unsigned int)data[*offset + 1] << 8) |
            ((unsigned int)data[*offset + 2] << 16) |
            ((unsigned int)data[*offset + 3] << 24));
        *offset += 4;
    } else if (*offset < size) {
        while (*offset < size) {
            value = (value << 8) | data[*offset];
            (*offset)++;
        }
    }
    return value;
}

int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *root1 = NULL;
    cJSON *array = NULL;
    cJSON *root2 = NULL;
    cJSON *root3 = NULL;
    cJSON *str1 = NULL;
    cJSON *str2 = NULL;
    cJSON *borrowed_item = NULL;

    root1 = cJSON_CreateObject();

    int count = 0;
    if (Size > 0) {
        count = (int)(Data[offset] % 16);
        offset++;
    }

    int *numbers = NULL;
    if (count > 0) {
        numbers = (int *)malloc((size_t)count * sizeof(int));
        if (numbers == NULL) {
            cJSON_Delete(root1);
            return 0;
        }
        for (int i = 0; i < count; i++) {
            numbers[i] = read_int32(Data, Size, &offset);
        }
    }

    array = cJSON_CreateIntArray(numbers, count);
    free(numbers);
    numbers = NULL;

    int index = 0;
    if (Size > offset) {
        index = (int)((int8_t)Data[offset]);
        offset++;
    }
    borrowed_item = cJSON_GetArrayItem(array, index);
    (void)borrowed_item;

    if (root1 != NULL && array != NULL) {
        size_t key1_len = (Size > offset) ? ((size_t)Data[offset] % (Size - offset + 1)) : 0;
        if (Size > offset) {
            offset++;
        }
        if (key1_len > Size - offset) {
            key1_len = Size - offset;
        }
        char *key1 = (char *)malloc(key1_len + 1);
        if (key1 != NULL) {
            if (key1_len > 0) {
                memcpy(key1, Data + offset, key1_len);
            }
            key1[key1_len] = '\0';
            offset += key1_len;
            if (!cJSON_AddItemToObject(root1, key1, array)) {
                cJSON_Delete(array);
                array = NULL;
            } else {
                array = NULL;
            }
            free(key1);
        } else {
            cJSON_Delete(array);
            array = NULL;
        }
    } else if (array != NULL) {
        cJSON_Delete(array);
        array = NULL;
    }

    root2 = cJSON_CreateObject();

    size_t str1_len = (Size > offset) ? ((size_t)Data[offset] % (Size - offset + 1)) : 0;
    if (Size > offset) {
        offset++;
    }
    if (str1_len > Size - offset) {
        str1_len = Size - offset;
    }
    char *buf1 = (char *)malloc(str1_len + 1);
    if (buf1 != NULL) {
        if (str1_len > 0) {
            memcpy(buf1, Data + offset, str1_len);
        }
        buf1[str1_len] = '\0';
        offset += str1_len;
        str1 = cJSON_CreateString(buf1);
        free(buf1);
    }

    if (root2 != NULL && str1 != NULL) {
        size_t key2_len = (Size > offset) ? ((size_t)Data[offset] % (Size - offset + 1)) : 0;
        if (Size > offset) {
            offset++;
        }
        if (key2_len > Size - offset) {
            key2_len = Size - offset;
        }
        char *key2 = (char *)malloc(key2_len + 1);
        if (key2 != NULL) {
            if (key2_len > 0) {
                memcpy(key2, Data + offset, key2_len);
            }
            key2[key2_len] = '\0';
            offset += key2_len;
            if (!cJSON_AddItemToObject(root2, key2, str1)) {
                cJSON_Delete(str1);
                str1 = NULL;
            } else {
                str1 = NULL;
            }
            free(key2);
        } else {
            cJSON_Delete(str1);
            str1 = NULL;
        }
    } else if (str1 != NULL) {
        cJSON_Delete(str1);
        str1 = NULL;
    }

    root3 = cJSON_CreateObject();

    size_t str2_len = (Size > offset) ? ((size_t)Data[offset] % (Size - offset + 1)) : 0;
    if (Size > offset) {
        offset++;
    }
    if (str2_len > Size - offset) {
        str2_len = Size - offset;
    }
    char *buf2 = (char *)malloc(str2_len + 1);
    if (buf2 != NULL) {
        if (str2_len > 0) {
            memcpy(buf2, Data + offset, str2_len);
        }
        buf2[str2_len] = '\0';
        offset += str2_len;
        str2 = cJSON_CreateString(buf2);
        free(buf2);
    }

    if (root3 != NULL && str2 != NULL) {
        size_t key3_len = (Size > offset) ? ((size_t)Data[offset] % (Size - offset + 1)) : 0;
        if (Size > offset) {
            offset++;
        }
        if (key3_len > Size - offset) {
            key3_len = Size - offset;
        }
        char *key3 = (char *)malloc(key3_len + 1);
        if (key3 != NULL) {
            if (key3_len > 0) {
                memcpy(key3, Data + offset, key3_len);
            }
            key3[key3_len] = '\0';
            if (!cJSON_AddItemToObject(root3, key3, str2)) {
                cJSON_Delete(str2);
                str2 = NULL;
            } else {
                str2 = NULL;
            }
            free(key3);
        } else {
            cJSON_Delete(str2);
            str2 = NULL;
        }
    } else if (str2 != NULL) {
        cJSON_Delete(str2);
        str2 = NULL;
    }

    cJSON_Delete(root3);
    cJSON_Delete(root2);
    cJSON_Delete(root1);

    return 0;
}