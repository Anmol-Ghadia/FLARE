// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_ParseWithOpts at cJSON.c:1131:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1147:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1232:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1147:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1232:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1131:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
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

static char *make_nul_terminated_copy(const uint8_t *Data, size_t Size) {
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

static void consume_error_ptr(const char *base, size_t len, const char *err) {
    volatile unsigned char sink = 0;
    if (base == NULL || err == NULL) {
        return;
    }

    if (err >= base && (size_t)(err - base) < len) {
        size_t pos = (size_t)(err - base);
        sink ^= (unsigned char)base[pos];
        if (pos > 0) {
            sink ^= (unsigned char)base[pos - 1];
        }
        if (pos + 1 < len) {
            sink ^= (unsigned char)base[pos + 1];
        }
    }

    (void)sink;
}

int LLVMFuzzerTestOneInput_26(const uint8_t *Data, size_t Size) {
    const char *parse_end = NULL;
    const char *err = NULL;
    cJSON *root = NULL;
    char *printed = NULL;

    char *nul_buf = make_nul_terminated_copy(Data, Size);
    if (nul_buf == NULL) {
        return 0;
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0) {
                (void)fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }
    }

    root = cJSON_Parse(nul_buf);
    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            cJSON *roundtrip = cJSON_Parse(printed);
            if (roundtrip != NULL) {
                cJSON_Delete(roundtrip);
            }
            free(printed);
            printed = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        err = cJSON_GetErrorPtr();
        consume_error_ptr(nul_buf, Size, err);
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(nul_buf, &parse_end, 0);
    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        err = cJSON_GetErrorPtr();
        consume_error_ptr(nul_buf, Size, err);
        consume_error_ptr(nul_buf, Size, parse_end);
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(nul_buf, &parse_end, 1);
    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        err = cJSON_GetErrorPtr();
        consume_error_ptr(nul_buf, Size, err);
        consume_error_ptr(nul_buf, Size, parse_end);
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts((const char *)Data, Size, &parse_end, 0);
    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            cJSON *roundtrip_len = cJSON_ParseWithLength(printed, strlen(printed));
            if (roundtrip_len != NULL) {
                cJSON_Delete(roundtrip_len);
            }
            free(printed);
            printed = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        err = cJSON_GetErrorPtr();
        consume_error_ptr((const char *)Data, Size, err);
        consume_error_ptr((const char *)Data, Size, parse_end);
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts((const char *)Data, Size, &parse_end, 1);
    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        err = cJSON_GetErrorPtr();
        consume_error_ptr((const char *)Data, Size, err);
        consume_error_ptr((const char *)Data, Size, parse_end);
    }

    root = cJSON_ParseWithLength((const char *)Data, Size);
    if (root != NULL) {
        printed = cJSON_Print(root);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        err = cJSON_GetErrorPtr();
        consume_error_ptr((const char *)Data, Size, err);
    }

    free(nul_buf);
    return 0;
}