#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *input;
    cJSON *root;
    const char *err;
    cJSON *item1, *item2, *item3, *item4;

    input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    root = cJSON_Parse(input);

    err = cJSON_GetErrorPtr();
    if (err != NULL) {
        volatile char a = err[0];
        volatile char b = (err > input) ? err[-1] : 0;
        (void)a;
        (void)b;
    }

    item1 = cJSON_GetObjectItemCaseSensitive(root, "a");
    (void)cJSON_IsString(item1);

    item2 = cJSON_GetObjectItemCaseSensitive(root, "b");
    item3 = cJSON_GetObjectItemCaseSensitive(root, "c");
    item4 = cJSON_GetObjectItemCaseSensitive(root, "d");

    (void)cJSON_IsNumber(item3);
    (void)cJSON_IsNumber(item4);

    cJSON_Delete(root);
    free(input);
    return 0;
}