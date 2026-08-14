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

static char *make_nul_terminated_copy(const uint8_t *Data, size_t Size)
{
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

static char *make_writable_copy_with_extra_nul(const uint8_t *Data, size_t Size)
{
    char *buf = (char *)malloc(Size + 2);
    if (buf == NULL) {
        return NULL;
    }
    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';
    buf[Size + 1] = '\0';
    return buf;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const char *parse_end = NULL;
    const char *err = NULL;
    cJSON *root = NULL;
    char *input = NULL;
    char *minify_buf = NULL;
    FILE *fp = NULL;

    if (Data == NULL) {
        return 0;
    }

    input = make_nul_terminated_copy(Data, Size);
    minify_buf = make_writable_copy_with_extra_nul(Data, Size);
    if ((input == NULL) || (minify_buf == NULL)) {
        free(input);
        free(minify_buf);
        return 0;
    }

    /* Exercise Parse */
    root = cJSON_Parse(input);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile unsigned char sink = 0;
            size_t i;
            for (i = 0; i < 4 && err >= input + i; i++) {
                sink ^= (unsigned char)err[-(ptrdiff_t)i];
            }
            (void)sink;
        }
    }

    /* Exercise ParseWithOpts with both require_null_terminated states */
    parse_end = NULL;
    root = cJSON_ParseWithOpts(input, &parse_end, 0);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (parse_end != NULL && err != NULL) {
            volatile ptrdiff_t diff = parse_end - err;
            (void)diff;
        }
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(input, &parse_end, 1);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (parse_end != NULL && err != NULL) {
            volatile ptrdiff_t diff = parse_end - err;
            (void)diff;
        }
    }

    /* Exercise ParseWithLength */
    root = cJSON_ParseWithLength(input, Size);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile unsigned char c = (unsigned char)*err;
            (void)c;
        }
    }

    /* Exercise ParseWithLengthOpts with multiple lengths and flags */
    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(input, Size, &parse_end, 0);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (parse_end != NULL && err != NULL) {
            volatile ptrdiff_t diff = parse_end - err;
            (void)diff;
        }
    }

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(input, Size, &parse_end, 1);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        err = cJSON_GetErrorPtr();
        if (parse_end != NULL && err != NULL) {
            volatile ptrdiff_t diff = parse_end - err;
            (void)diff;
        }
    }

    if (Size > 0) {
        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(input, Size - 1, &parse_end, 0);
        if (root != NULL) {
            cJSON_Delete(root);
        } else {
            err = cJSON_GetErrorPtr();
            if (parse_end != NULL && err != NULL) {
                volatile ptrdiff_t diff = parse_end - err;
                (void)diff;
            }
        }

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(input, Size - 1, &parse_end, 1);
        if (root != NULL) {
            cJSON_Delete(root);
        } else {
            err = cJSON_GetErrorPtr();
            if (parse_end != NULL && err != NULL) {
                volatile ptrdiff_t diff = parse_end - err;
                (void)diff;
            }
        }
    }

    /* Exercise Minify on writable memory */
    cJSON_Minify(minify_buf);

    /* Parse minified content through different entry points */
    root = cJSON_Parse(minify_buf);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        (void)cJSON_GetErrorPtr();
    }

    parse_end = NULL;
    root = cJSON_ParseWithOpts(minify_buf, &parse_end, 0);
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        (void)cJSON_GetErrorPtr();
    }

    root = cJSON_ParseWithLength(minify_buf, strlen(minify_buf));
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        (void)cJSON_GetErrorPtr();
    }

    /* Required file handling path */
    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);

        fp = fopen("./dummy_file", "rb");
        if (fp != NULL) {
            char *file_buf = (char *)malloc(Size + 1);
            if (file_buf != NULL) {
                size_t n = fread(file_buf, 1, Size, fp);
                file_buf[n] = '\0';

                root = cJSON_ParseWithLength(file_buf, n);
                if (root != NULL) {
                    cJSON_Delete(root);
                } else {
                    (void)cJSON_GetErrorPtr();
                }

                free(file_buf);
            }
            fclose(fp);
        }
    }

    /* Explicitly exercise NULL handling documented by APIs */
    (void)cJSON_Parse(NULL);
    (void)cJSON_ParseWithOpts(NULL, &parse_end, 0);
    (void)cJSON_ParseWithLength(NULL, 0);
    (void)cJSON_ParseWithLengthOpts(NULL, 0, &parse_end, 0);
    cJSON_Minify(NULL);
    (void)cJSON_GetErrorPtr();

    free(input);
    free(minify_buf);
    return 0;
}