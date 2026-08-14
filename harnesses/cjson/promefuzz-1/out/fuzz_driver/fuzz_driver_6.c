// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *input = NULL;
    char *minify_buf = NULL;
    const char *parse_end = NULL;
    cJSON *root = NULL;
    char *printed_buffered = NULL;
    char *printed = NULL;
    char *printed_unformatted = NULL;

    if (Data == NULL) {
        return 0;
    }

    input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }
    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    root = cJSON_ParseWithOpts(input, &parse_end, (Size > 0) ? (Data[0] & 1) : 0);

    if (root != NULL) {
        int prebuffer = 0;
        cJSON_bool fmt = 0;

        if (Size >= 4) {
            prebuffer = (int)((unsigned int)Data[1] |
                              ((unsigned int)Data[2] << 8) |
                              ((unsigned int)Data[3] << 16));
        } else {
            prebuffer = (int)Size;
        }
        if (prebuffer < 0) {
            prebuffer = 0;
        }
        fmt = (Size > 4) ? (Data[4] & 1) : 0;

        printed_buffered = cJSON_PrintBuffered(root, prebuffer, fmt);
        if (printed_buffered != NULL) {
            free(printed_buffered);
            printed_buffered = NULL;
        }

        printed = cJSON_Print(root);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }

        printed_unformatted = cJSON_PrintUnformatted(root);
        if (printed_unformatted != NULL) {
            free(printed_unformatted);
            printed_unformatted = NULL;
        }
    }

    minify_buf = (char *)malloc(Size + 1);
    if (minify_buf != NULL) {
        if (Size > 0) {
            memcpy(minify_buf, Data, Size);
        }
        minify_buf[Size] = '\0';
        cJSON_Minify(minify_buf);
        free(minify_buf);
        minify_buf = NULL;
    } else {
        cJSON_Minify(NULL);
    }

    if (root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }

    free(input);
    return 0;
}