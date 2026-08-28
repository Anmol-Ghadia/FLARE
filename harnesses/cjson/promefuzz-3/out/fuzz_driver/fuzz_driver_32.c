// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_malloc at cJSON.c:3155:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_free at cJSON.c:3160:20 in cJSON.h
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

static int consume_u8(const uint8_t **data, size_t *size)
{
    if (*size == 0) {
        return 0;
    }
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static size_t consume_size(const uint8_t **data, size_t *size)
{
    size_t v = 0;
    size_t n = (*size < sizeof(size_t)) ? *size : sizeof(size_t);
    for (size_t i = 0; i < n; i++) {
        v = (v << 8) | **data;
        (*data)++;
        (*size)--;
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    int fmt = consume_u8(&ptr, &remaining) & 1;
    size_t prebuffer_raw = consume_size(&ptr, &remaining);
    int prebuffer = (int)(prebuffer_raw % 4096);

    void *tmp = cJSON_malloc(remaining + 1);
    if (tmp != NULL) {
        memset(tmp, 0, remaining + 1);
        cJSON_free(tmp);
    }

    cJSON *root = cJSON_ParseWithLength((const char *)ptr, remaining);
    if (root != NULL) {
        char *printed1 = cJSON_Print(root);
        if (printed1 != NULL) {
            cJSON *reparsed1 = cJSON_ParseWithLength(printed1, strlen(printed1));
            if (reparsed1 != NULL) {
                char *bufprint = cJSON_PrintBuffered(reparsed1, prebuffer, fmt);
                if (bufprint != NULL) {
                    cJSON_Minify(bufprint);
                    cJSON *reparsed2 = cJSON_ParseWithLength(bufprint, strlen(bufprint));
                    if (reparsed2 != NULL) {
                        char *printed2 = cJSON_PrintUnformatted(reparsed2);
                        if (printed2 != NULL) {
                            cJSON_free(printed2);
                        }
                        cJSON_Delete(reparsed2);
                    }
                    cJSON_free(bufprint);
                }
                cJSON_Delete(reparsed1);
            }
            cJSON_free(printed1);
        }

        char *printed3 = cJSON_PrintUnformatted(root);
        if (printed3 != NULL) {
            cJSON_Minify(printed3);
            cJSON *reparsed3 = cJSON_ParseWithLength(printed3, strlen(printed3));
            if (reparsed3 != NULL) {
                char *printed4 = cJSON_PrintBuffered(reparsed3, prebuffer, fmt ^ 1);
                if (printed4 != NULL) {
                    cJSON_free(printed4);
                }
                cJSON_Delete(reparsed3);
            }
            cJSON_free(printed3);
        }

        char *printed5 = cJSON_PrintBuffered(root, prebuffer, fmt);
        if (printed5 != NULL) {
            cJSON_Minify(printed5);
            cJSON_free(printed5);
        }

        cJSON_Delete(root);
    } else {
        char *mutable_buf = (char *)malloc(remaining + 1);
        if (mutable_buf != NULL) {
            if (remaining > 0) {
                memcpy(mutable_buf, ptr, remaining);
            }
            mutable_buf[remaining] = '\0';
            cJSON_Minify(mutable_buf);

            cJSON *minified_root = cJSON_ParseWithLength(mutable_buf, strlen(mutable_buf));
            if (minified_root != NULL) {
                char *printed = cJSON_Print(minified_root);
                if (printed != NULL) {
                    cJSON_free(printed);
                }
                char *printed_buf = cJSON_PrintBuffered(minified_root, prebuffer, fmt);
                if (printed_buf != NULL) {
                    cJSON_free(printed_buf);
                }
                char *printed_unfmt = cJSON_PrintUnformatted(minified_root);
                if (printed_unfmt != NULL) {
                    cJSON_free(printed_unfmt);
                }
                cJSON_Delete(minified_root);
            }

            free(mutable_buf);
        }
    }

    cJSON_Minify(NULL);
    (void)cJSON_PrintBuffered(NULL, prebuffer, fmt);
    (void)cJSON_Print(NULL);
    (void)cJSON_PrintUnformatted(NULL);

    return 0;
}