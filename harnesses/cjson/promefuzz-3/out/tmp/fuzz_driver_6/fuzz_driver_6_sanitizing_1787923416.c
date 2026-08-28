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
    cJSON *root = NULL;
    char *printed_buffered = NULL;
    char *printed = NULL;
    char *printed_unformatted = NULL;
    const char *parse_end = NULL;
    int prebuffer = 0;
    cJSON_bool require_null_terminated = 0;
    cJSON_bool fmt = 0;

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

    if (Size > 2) {
        prebuffer = (int)Data[2];
        if (Size > 3) {
            prebuffer |= ((int)Data[3] << 8);
        }
        prebuffer &= 0x7fff;
    }

    root = cJSON_ParseWithOpts(input, &parse_end, require_null_terminated);
    if (root != NULL) {
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

        minify_buf = (char *)malloc(Size + 1);
        if (minify_buf != NULL) {
            if (Size > 0) {
                memcpy(minify_buf, Data, Size);
            }
            minify_buf[Size] = '\0';
            cJSON_Minify(minify_buf);
            free(minify_buf);
            minify_buf = NULL;
        }

        cJSON_Delete(root);
        root = NULL;
    }

    free(input);
    return 0;
}