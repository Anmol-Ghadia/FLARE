// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1316:26 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
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

static int clamp_int(size_t v) {
    if (v > 0x7fffffffU) {
        return 0x7fffffff;
    }
    return (int)v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const char *parse_end = NULL;
    cJSON *root = NULL;
    char *printed = NULL;
    char *printed_unformatted = NULL;
    char *printed_buffered = NULL;
    char *input = NULL;
    char *minified = NULL;
    char *prebuf = NULL;
    FILE *fp = NULL;
    size_t input_len;
    cJSON_bool require_null_terminated;
    cJSON_bool format_flag;
    int prebuffer;

    if (Data == NULL) {
        return 0;
    }

    require_null_terminated = (Size > 0) ? (Data[0] & 1) : 0;
    format_flag = (Size > 1) ? (Data[1] & 1) : 0;

    input_len = Size;
    input = (char *)malloc(input_len + 1);
    if (input == NULL) {
        return 0;
    }
    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[input_len] = '\0';

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    minified = (char *)malloc(input_len + 1);
    if (minified != NULL) {
        memcpy(minified, input, input_len + 1);
        cJSON_Minify(minified);
    }

    root = cJSON_ParseWithOpts(input, &parse_end, require_null_terminated);

    if (root == NULL && minified != NULL) {
        const char *parse_end2 = NULL;
        root = cJSON_ParseWithOpts(minified, &parse_end2, 0);
        (void)parse_end2;
    }

    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            cJSON *roundtrip = cJSON_ParseWithOpts(printed, &parse_end, 0);
            if (roundtrip != NULL) {
                cJSON_Delete(roundtrip);
            }
        }

        printed_unformatted = cJSON_PrintUnformatted(root);
        if (printed_unformatted != NULL) {
            cJSON *roundtrip2 = cJSON_ParseWithOpts(printed_unformatted, &parse_end, 1);
            if (roundtrip2 != NULL) {
                cJSON_Delete(roundtrip2);
            }
        }

        prebuffer = (Size > 2) ? (int)Data[2] : 0;
        if (Size > 3) {
            prebuffer |= ((int)Data[3] << 8);
        }
        prebuffer &= 0x7fff;

        printed_buffered = cJSON_PrintBuffered(root, prebuffer, format_flag);
        if (printed_buffered != NULL) {
            cJSON *roundtrip3 = cJSON_ParseWithOpts(printed_buffered, &parse_end, 0);
            if (roundtrip3 != NULL) {
                cJSON_Delete(roundtrip3);
            }
        }

        {
            size_t bufsize = 5;
            if (printed_unformatted != NULL) {
                bufsize = strlen(printed_unformatted) + 5;
            } else if (printed != NULL) {
                bufsize = strlen(printed) + 5;
            } else if (Size < 4096) {
                bufsize = Size + 5;
            } else {
                bufsize = 4096 + 5;
            }

            prebuf = (char *)malloc(bufsize);
            if (prebuf != NULL) {
                memset(prebuf, 0, bufsize);
                (void)cJSON_PrintPreallocated(root, prebuf, clamp_int(bufsize), 0);
                (void)cJSON_PrintPreallocated(root, prebuf, clamp_int(bufsize), 1);

                if (bufsize > 0) {
                    (void)cJSON_PrintPreallocated(root, prebuf, 0, 0);
                    (void)cJSON_PrintPreallocated(root, prebuf, 1, 1);
                    if (bufsize > 1) {
                        (void)cJSON_PrintPreallocated(root, prebuf, clamp_int(bufsize / 2), format_flag);
                    }
                }
            }
        }
    }

    free(prebuf);
    if (printed_buffered != NULL) {
        cJSON_free(printed_buffered);
    }
    if (printed_unformatted != NULL) {
        cJSON_free(printed_unformatted);
    }
    if (printed != NULL) {
        cJSON_free(printed);
    }
    if (root != NULL) {
        cJSON_Delete(root);
    }
    free(minified);
    free(input);

    return 0;
}