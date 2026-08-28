// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_IsString at cJSON.c:3032:26 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:3022:26 in cJSON.h
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

static char *make_nul_terminated(const uint8_t *Data, size_t Size) {
    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return NULL;
    }
    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';
    return buf;
}

int LLVMFuzzerTestOneInput_10(const uint8_t *Data, size_t Size) {
    char *input = make_nul_terminated(Data, Size);
    if (input == NULL) {
        return 0;
    }

    cJSON *root = cJSON_Parse(input);

    const char *err = cJSON_GetErrorPtr();
    (void)err;

    cJSON *item1 = cJSON_GetObjectItemCaseSensitive(root, "name");
    cJSON_bool is_str = cJSON_IsString(item1);
    (void)is_str;

    cJSON *item2 = cJSON_GetObjectItemCaseSensitive(root, "id");
    cJSON *item3 = cJSON_GetObjectItemCaseSensitive(root, "value");
    cJSON *item4 = cJSON_GetObjectItemCaseSensitive(root, "count");

    cJSON_bool is_num1 = cJSON_IsNumber(item3);
    cJSON_bool is_num2 = cJSON_IsNumber(item4);
    (void)item2;
    (void)is_num1;
    (void)is_num2;

    cJSON_Delete(root);
    free(input);
    return 0;
}