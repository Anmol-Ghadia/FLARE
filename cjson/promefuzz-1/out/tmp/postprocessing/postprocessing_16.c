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

    (void)cJSON_AddFalseToObject(object, name);

    if (Size > 0) {
        size_t mid = Size / 2;
        name[mid] = '\0';
        (void)cJSON_AddFalseToObject(object, name);
    }

    if (Size > 1) {
        char *name2 = (char *)malloc(Size + 1);
        if (name2 != NULL) {
            for (size_t i = 0; i < Size; ++i) {
                uint8_t b = Data[Size - 1 - i];
                name2[i] = (char)((b >= 32 && b <= 126) ? b : (b % 26) + 'a');
            }
            name2[Size] = '\0';
            (void)cJSON_AddFalseToObject(object, name2);
            free(name2);
        }
    }

    free(name);
    cJSON_Delete(object);
    return 0;
}