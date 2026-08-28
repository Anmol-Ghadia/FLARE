// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
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
#include "cJSON.h"

static void consume_tree(cJSON *root)
{
    if (root == NULL) {
        return;
    }

    (void)cJSON_IsString(root);

    if (root->child != NULL) {
        cJSON *child = root->child;
        size_t limit = 0;
        while ((child != NULL) && (limit++ < 1024)) {
            (void)cJSON_IsString(child);
            if (child->child != NULL) {
                cJSON *grand = child->child;
                size_t glimit = 0;
                while ((grand != NULL) && (glimit++ < 1024)) {
                    (void)cJSON_IsString(grand);
                    grand = grand->next;
                }
            }
            child = child->next;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const char *parse_end = NULL;
    const char *err = NULL;
    cJSON *root = NULL;
    char *buf = NULL;
    char *buf2 = NULL;

    (void)cJSON_IsString(NULL);

    if (Data == NULL) {
        return 0;
    }

    buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return 0;
    }
    memcpy(buf, Data, Size);
    buf[Size] = '\0';

    root = cJSON_Parse(buf);
    if (root != NULL) {
        consume_tree(root);
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char sink = *err;
            (void)sink;
        }
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(buf, &parse_end, 0);
    if (root != NULL) {
        consume_tree(root);
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char sink = *err;
            (void)sink;
        }
        if (parse_end != NULL) {
            volatile char sink2 = *parse_end;
            (void)sink2;
        }
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(buf, &parse_end, 1);
    if (root != NULL) {
        consume_tree(root);
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char sink = *err;
            (void)sink;
        }
        if (parse_end != NULL) {
            volatile char sink2 = *parse_end;
            (void)sink2;
        }
    }

    root = cJSON_ParseWithLength(buf, Size);
    if (root != NULL) {
        consume_tree(root);
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char sink = *err;
            (void)sink;
        }
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(buf, Size, &parse_end, 0);
    if (root != NULL) {
        consume_tree(root);
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char sink = *err;
            (void)sink;
        }
        if (parse_end != NULL) {
            volatile char sink2 = *parse_end;
            (void)sink2;
        }
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(buf, Size, &parse_end, 1);
    if (root != NULL) {
        consume_tree(root);
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char sink = *err;
            (void)sink;
        }
        if (parse_end != NULL) {
            volatile char sink2 = *parse_end;
            (void)sink2;
        }
    }

    if (Size > 0) {
        size_t half = Size / 2;

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(buf, half, &parse_end, 0);
        if (root != NULL) {
            consume_tree(root);
            cJSON_Delete(root);
        } else {
            err = cJSON_GetErrorPtr();
            if (err != NULL) {
                volatile char sink = *err;
                (void)sink;
            }
        }

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(buf, half, &parse_end, 1);
        if (root != NULL) {
            consume_tree(root);
            cJSON_Delete(root);
        } else {
            err = cJSON_GetErrorPtr();
            if (err != NULL) {
                volatile char sink = *err;
                (void)sink;
            }
        }
    }

    buf2 = (char *)malloc(Size + 2);
    if (buf2 != NULL) {
        if (Size > 0) {
            memcpy(buf2, Data, Size);
        }
        buf2[0] = ' ';
        if (Size > 0) {
            memmove(buf2 + 1, buf2, Size);
        }
        buf2[Size + 1] = '\0';

        parse_end = NULL;
        root = cJSON_ParseWithOpts(buf2, &parse_end, 0);
        if (root != NULL) {
            consume_tree(root);
            cJSON_Delete(root);
        } else {
            err = cJSON_GetErrorPtr();
            if (err != NULL) {
                volatile char sink = *err;
                (void)sink;
            }
        }

        free(buf2);
    }

    free(buf);
    return 0;
}