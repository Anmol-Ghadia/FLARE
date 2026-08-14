// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_Version at cJSON.c:124:27 in cJSON.h
// cJSON_Version at cJSON.c:124:27 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_ParseWithLength at cJSON.c:1200:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_Minify at cJSON.c:2882:20 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
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
#include <stdio.h>

#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const char *version1 = cJSON_Version();
    const char *version2 = cJSON_Version();
    (void)version1;
    (void)version2;

    cJSON_Minify(NULL);

    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';

    cJSON *root1 = cJSON_Parse(buf);
    if (root1 != NULL) {
        const char *err = cJSON_GetErrorPtr();
        (void)err;
        cJSON_Delete(root1);
    } else {
        const char *err = cJSON_GetErrorPtr();
        if (err != NULL) {
            volatile char a = *err;
            (void)a;
            if (err > buf) {
                volatile char b = *(err - 1);
                (void)b;
            }
            if (err > buf + 1) {
                volatile char c = *(err - 2);
                (void)c;
            }
        }
    }

    const char *parse_end1 = NULL;
    cJSON *root2 = cJSON_ParseWithOpts(buf, &parse_end1, 0);
    if (root2 != NULL) {
        cJSON_Delete(root2);
    } else {
        const char *err = cJSON_GetErrorPtr();
        (void)err;
        if (parse_end1 != NULL) {
            volatile char d = *parse_end1;
            (void)d;
        }
    }

    const char *parse_end2 = NULL;
    cJSON *root3 = cJSON_ParseWithOpts(buf, &parse_end2, 1);
    if (root3 != NULL) {
        cJSON_Delete(root3);
    } else {
        const char *err = cJSON_GetErrorPtr();
        (void)err;
        if (parse_end2 != NULL) {
            volatile char e = *parse_end2;
            (void)e;
        }
    }

    cJSON *root4 = cJSON_ParseWithLength(buf, Size);
    if (root4 != NULL) {
        cJSON_Delete(root4);
    } else {
        const char *err = cJSON_GetErrorPtr();
        (void)err;
    }

    char *minify_buf = (char *)malloc(Size + 1);
    if (minify_buf != NULL) {
        if (Size > 0) {
            memcpy(minify_buf, Data, Size);
        }
        minify_buf[Size] = '\0';

        cJSON_Minify(minify_buf);

        cJSON *root5 = cJSON_Parse(minify_buf);
        if (root5 != NULL) {
            cJSON_Delete(root5);
        } else {
            const char *err = cJSON_GetErrorPtr();
            (void)err;
        }

        const char *parse_end3 = NULL;
        cJSON *root6 = cJSON_ParseWithOpts(minify_buf, &parse_end3, 0);
        if (root6 != NULL) {
            cJSON_Delete(root6);
        } else {
            const char *err = cJSON_GetErrorPtr();
            (void)err;
            if (parse_end3 != NULL) {
                volatile char f = *parse_end3;
                (void)f;
            }
        }

        free(minify_buf);
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);

        fp = fopen("./dummy_file", "rb");
        if (fp != NULL) {
            if (fseek(fp, 0, SEEK_END) == 0) {
                long fsize = ftell(fp);
                if (fsize >= 0 && fseek(fp, 0, SEEK_SET) == 0) {
                    char *file_buf = (char *)malloc((size_t)fsize + 1);
                    if (file_buf != NULL) {
                        size_t nread = fread(file_buf, 1, (size_t)fsize, fp);
                        file_buf[nread] = '\0';

                        cJSON *root7 = cJSON_ParseWithLength(file_buf, nread);
                        if (root7 != NULL) {
                            cJSON_Delete(root7);
                        } else {
                            const char *err = cJSON_GetErrorPtr();
                            (void)err;
                        }

                        free(file_buf);
                    }
                }
            }
            fclose(fp);
        }
    }

    free(buf);
    return 0;
}