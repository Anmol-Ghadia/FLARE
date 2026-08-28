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

static int write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const char *parse_end = NULL;
    cJSON *root = NULL;
    char *printed = NULL;
    char *printed_unformatted = NULL;
    char *printed_buffered = NULL;
    char *mutable_buf = NULL;
    char *nul_term = NULL;
    size_t i;

    (void)write_dummy_file(Data, Size);

    nul_term = (char *)malloc(Size + 1);
    if (nul_term == NULL) {
        return 0;
    }
    if (Size > 0) {
        memcpy(nul_term, Data, Size);
    }
    nul_term[Size] = '\0';

    root = cJSON_ParseWithOpts(nul_term, &parse_end, 0);
    if (root == NULL) {
        root = cJSON_ParseWithOpts(nul_term, &parse_end, 1);
    }

    if (root != NULL) {
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

        for (i = 0; i < 4; i++) {
            int prebuffer = 0;
            cJSON_bool fmt = (cJSON_bool)(i & 1);

            switch (i) {
                case 0:
                    prebuffer = 0;
                    break;
                case 1:
                    prebuffer = (int)Size;
                    break;
                case 2:
                    prebuffer = (int)(Size / 2);
                    break;
                default:
                    prebuffer = 256;
                    break;
            }

            printed_buffered = cJSON_PrintBuffered(root, prebuffer, fmt);
            if (printed_buffered != NULL) {
                free(printed_buffered);
                printed_buffered = NULL;
            }
        }

        {
            int lengths[5];
            char *buffers[5] = { NULL, NULL, NULL, NULL, NULL };
            size_t base_len = Size + 8;

            lengths[0] = 0;
            lengths[1] = 1;
            lengths[2] = (int)(base_len > 32 ? 32 : base_len);
            lengths[3] = (int)base_len;
            lengths[4] = (int)(base_len + 64);

            for (i = 0; i < 5; i++) {
                if (lengths[i] > 0) {
                    buffers[i] = (char *)malloc((size_t)lengths[i]);
                    if (buffers[i] != NULL) {
                        memset(buffers[i], 0, (size_t)lengths[i]);
                        (void)cJSON_PrintPreallocated(root, buffers[i], lengths[i], (cJSON_bool)(i & 1));
                    }
                }
            }

            for (i = 0; i < 5; i++) {
                free(buffers[i]);
            }
        }

        if (parse_end != NULL && parse_end >= nul_term && parse_end <= nul_term + Size) {
            cJSON *tail = cJSON_ParseWithOpts(parse_end, &parse_end, 0);
            if (tail != NULL) {
                char stackbuf[64];
                (void)cJSON_PrintPreallocated(tail, stackbuf, (int)sizeof(stackbuf), 0);
                cJSON_Delete(tail);
            }
        }

        cJSON_Delete(root);
        root = NULL;
    }

    mutable_buf = (char *)malloc(Size + 1);
    if (mutable_buf != NULL) {
        if (Size > 0) {
            memcpy(mutable_buf, Data, Size);
        }
        mutable_buf[Size] = '\0';
        cJSON_Minify(mutable_buf);

        root = cJSON_ParseWithOpts(mutable_buf, &parse_end, 0);
        if (root != NULL) {
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

            printed_buffered = cJSON_PrintBuffered(root, (int)(Size + 16), 1);
            if (printed_buffered != NULL) {
                free(printed_buffered);
                printed_buffered = NULL;
            }

            {
                int outlen = (int)(Size + 32);
                char *outbuf = NULL;
                if (outlen > 0) {
                    outbuf = (char *)malloc((size_t)outlen);
                    if (outbuf != NULL) {
                        memset(outbuf, 0, (size_t)outlen);
                        (void)cJSON_PrintPreallocated(root, outbuf, outlen, 0);
                        free(outbuf);
                    }
                }
            }

            cJSON_Delete(root);
            root = NULL;
        }

        free(mutable_buf);
        mutable_buf = NULL;
    }

    cJSON_Minify(NULL);

    free(nul_term);
    return 0;
}