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
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    char *name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[Size] = '\0';

    (void)cJSON_AddObjectToObject(root, name);

    if (Size > 1) {
        size_t mid = Size / 2;
        name[mid] = '\0';
        (void)cJSON_AddObjectToObject(root, name);
    }

    if (Size > 0) {
        name[0] = '\0';
        (void)cJSON_AddObjectToObject(root, name);
    }

    free(name);
    cJSON_Delete(root);
    return 0;
}