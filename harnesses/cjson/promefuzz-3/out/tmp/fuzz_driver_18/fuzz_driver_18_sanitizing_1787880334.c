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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root;
    cJSON *added;
    char *name = NULL;
    char *raw = NULL;
    size_t name_len, raw_len;
    size_t split;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    if (Size == 0) {
        added = cJSON_AddRawToObject(root, "", "null");
        (void)added;
        cJSON_Delete(root);
        return 0;
    }

    split = Size / 2;
    name_len = split;
    raw_len = Size - split;

    name = (char *)malloc(name_len + 1);
    raw = (char *)malloc(raw_len + 1);
    if ((name == NULL) || (raw == NULL)) {
        free(name);
        free(raw);
        cJSON_Delete(root);
        return 0;
    }

    if (name_len > 0) {
        memcpy(name, Data, name_len);
    }
    name[name_len] = '\0';

    if (raw_len > 0) {
        memcpy(raw, Data + split, raw_len);
    }
    raw[raw_len] = '\0';

    added = cJSON_AddRawToObject(root, name, raw);
    (void)added;

    if (raw_len > 0) {
        raw[0] = '{';
        added = cJSON_AddRawToObject(root, name, raw);
        (void)added;
    }

    if (raw_len > 1) {
        raw[0] = '[';
        raw[raw_len - 1] = ']';
        added = cJSON_AddRawToObject(root, name, raw);
        (void)added;
    }

    added = cJSON_AddRawToObject(root, "", "null");
    (void)added;

    added = cJSON_AddRawToObject(root, "x", "true");
    (void)added;

    free(name);
    free(raw);
    cJSON_Delete(root);
    return 0;
}