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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    size_t split = 0;
    if (Size > 0) {
        split = Data[0] % (Size + 1);
    }

    const uint8_t *name_data = Data;
    size_t name_size = split;
    const uint8_t *raw_data = Data + split;
    size_t raw_size = Size - split;

    char *name = make_cstring(name_data, name_size);
    char *raw = make_cstring(raw_data, raw_size);

    if (name != NULL && raw != NULL) {
        (void)cJSON_AddRawToObject(root, name, raw);
    }

    if (Size > 1) {
        size_t split2 = Data[Size - 1] % (Size + 1);
        const uint8_t *name_data2 = Data;
        size_t name_size2 = split2;
        const uint8_t *raw_data2 = Data + split2;
        size_t raw_size2 = Size - split2;

        char *name2 = make_cstring(name_data2, name_size2);
        char *raw2 = make_cstring(raw_data2, raw_size2);

        if (name2 != NULL && raw2 != NULL) {
            (void)cJSON_AddRawToObject(root, name2, raw2);
        }

        free(name2);
        free(raw2);
    }

    free(name);
    free(raw);
    cJSON_Delete(root);
    return 0;
}