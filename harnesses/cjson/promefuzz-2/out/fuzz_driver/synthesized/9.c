// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateFalse at cJSON.c:2441:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2506:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObjectReference at cJSON.c:2518:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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
#include <stdio.h>

#include "cJSON.h"

int LLVMFuzzerTestOneInput_9(const uint8_t *Data, size_t Size)
{
    char *buf = NULL;
    const char *input_str = NULL;
    const char *ref_str = NULL;

    if (Size > 0) {
        buf = (char *)malloc(Size + 1);
        if (buf == NULL) {
            return 0;
        }
        memcpy(buf, Data, Size);
        buf[Size] = '\0';
        input_str = buf;

        if (Size > 1) {
            size_t split = Data[0] % Size;
            buf[split] = '\0';
            ref_str = buf + split;
        } else {
            ref_str = buf;
        }
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *item_false = cJSON_CreateFalse();
    cJSON *item_string = cJSON_CreateString(input_str);
    cJSON *item_object = cJSON_CreateObject();
    cJSON *item_string_ref = cJSON_CreateStringReference((Size & 1) ? ref_str : NULL);

    cJSON_Delete(item_string_ref);

    cJSON *item_object_ref = cJSON_CreateObjectReference(item_object);

    cJSON_Delete(item_object_ref);
    cJSON_Delete(item_object);
    cJSON_Delete(item_string);
    cJSON_Delete(item_false);

    free(buf);
    return 0;
}