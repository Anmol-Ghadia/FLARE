// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1946:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *input;
    cJSON *root = NULL;
    const char *errptr;
    cJSON *item1;
    cJSON *item2;
    cJSON *item3;
    cJSON *item4;

    input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    root = cJSON_Parse(input);

    errptr = cJSON_GetErrorPtr();
    (void)errptr;

    item1 = cJSON_GetObjectItemCaseSensitive(root, "a");
    (void)cJSON_IsString(item1);

    item2 = cJSON_GetObjectItemCaseSensitive(root, "b");
    item3 = cJSON_GetObjectItemCaseSensitive(root, "");
    item4 = cJSON_GetObjectItemCaseSensitive(root, input);

    (void)item2;
    (void)cJSON_IsNumber(item3);
    (void)cJSON_IsNumber(item4);

    cJSON_Delete(root);
    free(input);
    return 0;
}