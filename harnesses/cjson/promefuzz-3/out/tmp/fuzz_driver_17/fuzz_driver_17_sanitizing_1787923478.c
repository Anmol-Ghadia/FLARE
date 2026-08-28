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

static char *make_name(const uint8_t *data, size_t size, size_t *consumed)
{
    size_t len;
    char *name;
    size_t i;

    if (size == 0) {
        *consumed = 0;
        name = (char *)malloc(1);
        if (name != NULL) {
            name[0] = '\0';
        }
        return name;
    }

    len = data[0];
    *consumed = 1;

    if (len > size - 1) {
        len = size - 1;
    }

    name = (char *)malloc(len + 1);
    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        unsigned char c = data[1 + i];
        if (c == '\0') {
            c = 'A';
        }
        name[i] = (char)c;
    }
    name[len] = '\0';
    *consumed += len;
    return name;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root;
    cJSON_bool b1 = 0;
    cJSON_bool b2 = 0;
    char *name1 = NULL;
    char *name2 = NULL;
    size_t used1 = 0;
    size_t used2 = 0;
    const uint8_t *p = Data;
    size_t remaining = Size;

    if (remaining > 0) {
        b1 = (cJSON_bool)(p[0] & 1);
        p++;
        remaining--;
    }

    if (remaining > 0) {
        b2 = (cJSON_bool)(p[0] & 1);
        p++;
        remaining--;
    }

    name1 = make_name(p, remaining, &used1);
    if (name1 == NULL) {
        return 0;
    }
    if (used1 <= remaining) {
        p += used1;
        remaining -= used1;
    } else {
        remaining = 0;
    }

    name2 = make_name(p, remaining, &used2);
    if (name2 == NULL) {
        free(name1);
        return 0;
    }

    root = cJSON_CreateObject();
    if (root != NULL) {
        (void)cJSON_AddBoolToObject(root, name1, b1);
        (void)cJSON_AddBoolToObject(root, name2, b2);
        cJSON_Delete(root);
    }

    free(name1);
    free(name2);
    return 0;
}