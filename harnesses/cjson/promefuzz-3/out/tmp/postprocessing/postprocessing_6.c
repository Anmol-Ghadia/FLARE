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
    char *mutable_buf = NULL;
    const char *parse_end = NULL;
    cJSON *root = NULL;
    char *printed_buffered = NULL;
    char *printed = NULL;
    char *printed_unformatted = NULL;
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

    if (Size > 2) {
        prebuffer = (int)Data[2];
        if (Size > 3) {
            prebuffer |= ((int)Data[3] << 8);
        }
        if (Data[0] & 2) {
            prebuffer = -prebuffer;
        }
    } else {
        prebuffer = 0;
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
    }

    mutable_buf = (char *)malloc(Size + 1);
    if (mutable_buf != NULL) {
        if (Size > 0) {
            memcpy(mutable_buf, Data, Size);
        }
        mutable_buf[Size] = '\0';
        cJSON_Minify(mutable_buf);
        free(mutable_buf);
    }

    cJSON_Delete(root);
    free(input);
    return 0;
}