// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_ParseWithOpts at cJSON.c:1131:23 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1312:22 in cJSON.h
// cJSON_Minify at cJSON.c:2924:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Minify at cJSON.c:2924:20 in cJSON.h
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
    char *input;
    const char *parse_end = NULL;
    cJSON *root;
    char *printed_buffered = NULL;
    char *printed = NULL;
    char *printed_unformatted = NULL;
    char *minify_buf = NULL;
    int prebuffer;
    cJSON_bool require_null_terminated;
    cJSON_bool fmt;

    input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    require_null_terminated = (Size > 0) ? (cJSON_bool)(Data[0] & 1) : 0;
    fmt = (Size > 1) ? (cJSON_bool)(Data[1] & 1) : 0;

    root = cJSON_ParseWithOpts(input, &parse_end, require_null_terminated);
    if (root != NULL) {
        if (Size > 5) {
            prebuffer = (int)(((unsigned int)Data[2] << 16) |
                              ((unsigned int)Data[3] << 8) |
                              (unsigned int)Data[4]);
            if (Data[5] & 1) {
                prebuffer = -prebuffer;
            }
        } else {
            prebuffer = (int)Size;
        }

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
            minify_buf = (char *)malloc(strlen(printed_unformatted) + 1);
            if (minify_buf != NULL) {
                memcpy(minify_buf, printed_unformatted, strlen(printed_unformatted) + 1);
                cJSON_Minify(minify_buf);
                free(minify_buf);
                minify_buf = NULL;
            }
            free(printed_unformatted);
            printed_unformatted = NULL;
        }

        cJSON_Delete(root);
    } else {
        minify_buf = (char *)malloc(Size + 1);
        if (minify_buf != NULL) {
            if (Size > 0) {
                memcpy(minify_buf, Data, Size);
            }
            minify_buf[Size] = '\0';
            cJSON_Minify(minify_buf);
            free(minify_buf);
        }
    }

    free(input);
    return 0;
}