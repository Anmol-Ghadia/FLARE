// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1312:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1312:22 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
// cJSON_GetObjectItemCaseSensitive at cJSON.c:1988:23 in cJSON.h
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

static int consume_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(int)) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

int LLVMFuzzerTestOneInput_36(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    int prebuffer = consume_int(&ptr, &remaining);
    int extra = consume_int(&ptr, &remaining);
    cJSON_bool fmt1 = (cJSON_bool)(consume_int(&ptr, &remaining) & 1);
    cJSON_bool fmt2 = (cJSON_bool)(consume_int(&ptr, &remaining) & 1);

    char *json_input = (char *)malloc(remaining + 1);
    if (json_input == NULL) {
        return 0;
    }
    if (remaining > 0) {
        memcpy(json_input, ptr, remaining);
    }
    json_input[remaining] = '\0';

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (remaining > 0) {
            fwrite(ptr, 1, remaining, fp);
        }
        fclose(fp);
    }

    cJSON *root = cJSON_Parse(json_input);

    if (root != NULL) {
        char *printed = cJSON_Print(root);
        if (printed != NULL) {
            cJSON *reparsed = cJSON_Parse(printed);
            if (reparsed != NULL) {
                cJSON_Delete(reparsed);
            }
            free(printed);
        }

        char *unformatted = cJSON_PrintUnformatted(root);
        if (unformatted != NULL) {
            cJSON *reparsed2 = cJSON_Parse(unformatted);
            if (reparsed2 != NULL) {
                cJSON_Delete(reparsed2);
            }
            free(unformatted);
        }

        char *buffered1 = cJSON_PrintBuffered(root, prebuffer, fmt1);
        if (buffered1 != NULL) {
            free(buffered1);
        }

        char *buffered2 = cJSON_PrintBuffered(root, extra, fmt2);
        if (buffered2 != NULL) {
            free(buffered2);
        }

        if ((root->type & 0xFF) == cJSON_Object) {
            cJSON_GetObjectItemCaseSensitive(root, "");
            cJSON_GetObjectItemCaseSensitive(root, "a");
            cJSON_GetObjectItemCaseSensitive(root, "A");
            cJSON_GetObjectItemCaseSensitive(root, json_input);
            if (remaining > 0) {
                size_t key_len = remaining > 32 ? 32 : remaining;
                char keybuf[33];
                memcpy(keybuf, ptr, key_len);
                keybuf[key_len] = '\0';
                cJSON_GetObjectItemCaseSensitive(root, keybuf);
            }
        } else {
            cJSON_GetObjectItemCaseSensitive(root, "nonobject");
        }

        {
            int lengths[6];
            lengths[0] = 0;
            lengths[1] = 1;
            lengths[2] = 5;
            lengths[3] = 16;
            lengths[4] = prebuffer;
            lengths[5] = extra;

            for (size_t i = 0; i < sizeof(lengths) / sizeof(lengths[0]); ++i) {
                int len = lengths[i];
                if (len > 0 && len < (1 << 20)) {
                    char *buf = (char *)malloc((size_t)len);
                    if (buf != NULL) {
                        memset(buf, 0, (size_t)len);
                        (void)cJSON_PrintPreallocated(root, buf, len, fmt1);
                        (void)cJSON_PrintPreallocated(root, buf, len, fmt2);
                        free(buf);
                    }
                }
            }
        }

        cJSON_Delete(root);
    } else {
        cJSON_GetObjectItemCaseSensitive(NULL, json_input);
        (void)cJSON_Print(root);
        (void)cJSON_PrintUnformatted(root);
        (void)cJSON_PrintBuffered(root, prebuffer, fmt1);

        if (prebuffer > 0 && prebuffer < (1 << 20)) {
            char *buf = (char *)malloc((size_t)prebuffer);
            if (buf != NULL) {
                memset(buf, 0, (size_t)prebuffer);
                (void)cJSON_PrintPreallocated(NULL, buf, prebuffer, fmt1);
                free(buf);
            }
        }
    }

    free(json_input);
    return 0;
}