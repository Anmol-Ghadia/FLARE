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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';

    cJSON *item_false = cJSON_CreateFalse();
    cJSON *item_string = cJSON_CreateString(buf);
    cJSON *item_object = cJSON_CreateObject();
    cJSON *item_string_ref = cJSON_CreateStringReference(buf);

    if (item_object != NULL) {
        if ((Size & 1) != 0) {
            item_object->child = item_string;
            if (item_string != NULL) {
                item_string->prev = NULL;
                item_string->next = NULL;
            }
            item_string = NULL;
        } else if ((Size & 2) != 0) {
            item_object->child = item_false;
            if (item_false != NULL) {
                item_false->prev = NULL;
                item_false->next = NULL;
            }
            item_false = NULL;
        } else if ((Size & 4) != 0) {
            item_object->child = item_string_ref;
            if (item_string_ref != NULL) {
                item_string_ref->prev = NULL;
                item_string_ref->next = NULL;
            }
            item_string_ref = NULL;
        }
    }

    cJSON_Delete(item_false);

    cJSON *item_object_ref = cJSON_CreateObjectReference(item_object);

    cJSON_Delete(item_string);
    cJSON_Delete(item_object);
    cJSON_Delete(item_string_ref);
    cJSON_Delete(item_object_ref);

    free(buf);
    return 0;
}