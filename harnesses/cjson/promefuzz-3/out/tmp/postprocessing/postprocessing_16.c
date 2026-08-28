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
    char *name;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[Size] = '\0';

    added = cJSON_AddFalseToObject(root, name);
    (void)added;

    cJSON_Delete(root);
    free(name);
    return 0;
}