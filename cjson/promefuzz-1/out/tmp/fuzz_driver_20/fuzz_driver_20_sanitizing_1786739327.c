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
    cJSON *object = cJSON_CreateObject();
    if (object == NULL) {
        return 0;
    }

    char *name = (char *)malloc(Size + 1);
    if (name == NULL) {
        cJSON_Delete(object);
        return 0;
    }

    if (Size > 0) {
        memcpy(name, Data, Size);
    }
    name[Size] = '\0';

    (void)cJSON_AddNullToObject(object, name);

    if (Size > 1) {
        size_t split = Data[0] % (Size + 1);
        char *name2 = (char *)malloc(split + 1);
        if (name2 != NULL) {
            if (split > 0) {
                memcpy(name2, Data, split);
            }
            name2[split] = '\0';
            (void)cJSON_AddNullToObject(object, name2);
            free(name2);
        }
    }

    if (Size > 2) {
        size_t split2 = Data[Size - 1] % (Size + 1);
        char *name3 = (char *)malloc(split2 + 1);
        if (name3 != NULL) {
            if (split2 > 0) {
                memcpy(name3, Data + (Size - split2), split2);
            }
            name3[split2] = '\0';
            (void)cJSON_AddNullToObject(object, name3);
            free(name3);
        }
    }

    free(name);
    cJSON_Delete(object);
    return 0;
}