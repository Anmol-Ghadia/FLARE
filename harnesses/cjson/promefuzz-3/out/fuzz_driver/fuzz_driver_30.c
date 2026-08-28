// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_ParseWithLengthOpts at cJSON.c:1115:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_GetObjectItem at cJSON.c:1941:23 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
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

static void exercise_tree(cJSON *root, const uint8_t *Data, size_t Size) {
    char *printed;
    char *keybuf;
    cJSON *item;

    if (root == NULL) {
        return;
    }

    printed = cJSON_Print(root);
    if (printed != NULL) {
        free(printed);
    }

    item = cJSON_GetObjectItem(root, "a");
    if (item != NULL) {
        printed = cJSON_Print(item);
        if (printed != NULL) {
            free(printed);
        }
    }

    item = cJSON_GetObjectItem(root, "");
    if (item != NULL) {
        printed = cJSON_Print(item);
        if (printed != NULL) {
            free(printed);
        }
    }

    keybuf = (char *)malloc(Size + 1);
    if (keybuf != NULL) {
        if (Size > 0) {
            memcpy(keybuf, Data, Size);
        }
        keybuf[Size] = '\0';

        item = cJSON_GetObjectItem(root, keybuf);
        if (item != NULL) {
            printed = cJSON_Print(item);
            if (printed != NULL) {
                free(printed);
            }
        }
        free(keybuf);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *buf;
    cJSON *root;
    const char *parse_end = NULL;
    FILE *fp;

    if (Data == NULL) {
        return 0;
    }

    buf = make_nul_terminated_copy(Data, Size);
    if (buf == NULL) {
        return 0;
    }

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    root = cJSON_Parse(buf);
    exercise_tree(root, Data, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(buf, &parse_end, 0);
    exercise_tree(root, Data, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(buf, &parse_end, 1);
    exercise_tree(root, Data, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    }

    root = cJSON_ParseWithLength(buf, Size);
    exercise_tree(root, Data, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(buf, Size, &parse_end, 0);
    exercise_tree(root, Data, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(buf, Size, &parse_end, 1);
    exercise_tree(root, Data, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    }

    if (Size > 0) {
        size_t half = Size / 2;

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(buf, half, &parse_end, 0);
        exercise_tree(root, Data, Size);
        if (root != NULL) {
            cJSON_Delete(root);
        }

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(buf, half, &parse_end, 1);
        exercise_tree(root, Data, Size);
        if (root != NULL) {
            cJSON_Delete(root);
        }
    }

    free(buf);
    return 0;
}