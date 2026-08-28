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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const char *version;
    char *input;
    const char *parse_end = NULL;
    const char *err_ptr;
    cJSON *root1 = NULL;
    cJSON *root2 = NULL;
    cJSON *root3 = NULL;
    char *printed = NULL;

    write_dummy_file(Data, Size);

    version = cJSON_Version();
    (void)version;

    input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    root1 = cJSON_Parse(input);
    if (root1 == NULL) {
        err_ptr = cJSON_GetErrorPtr();
        if (err_ptr != NULL) {
            volatile char c = *err_ptr;
            (void)c;
        }
    } else {
        printed = cJSON_PrintUnformatted(root1);
        if (printed != NULL) {
            cJSON *reparsed = cJSON_Parse(printed);
            if (reparsed != NULL) {
                cJSON_Delete(reparsed);
            } else {
                (void)cJSON_GetErrorPtr();
            }
            cJSON_free(printed);
            printed = NULL;
        }
        cJSON_Delete(root1);
        root1 = NULL;
    }

    parse_end = NULL;
    root2 = cJSON_ParseWithOpts(input, &parse_end, 0);
    if (root2 != NULL) {
        printed = cJSON_PrintUnformatted(root2);
        if (printed != NULL) {
            cJSON_free(printed);
            printed = NULL;
        }
        cJSON_Delete(root2);
        root2 = NULL;
    } else {
        (void)parse_end;
        (void)cJSON_GetErrorPtr();
    }

    parse_end = NULL;
    root3 = cJSON_ParseWithOpts(input, &parse_end, 1);
    if (root3 != NULL) {
        printed = cJSON_PrintUnformatted(root3);
        if (printed != NULL) {
            cJSON_free(printed);
            printed = NULL;
        }
        cJSON_Delete(root3);
        root3 = NULL;
    } else {
        (void)parse_end;
        (void)cJSON_GetErrorPtr();
    }

    {
        cJSON *root4 = cJSON_ParseWithLength(input, Size);
        if (root4 != NULL) {
            printed = cJSON_PrintUnformatted(root4);
            if (printed != NULL) {
                cJSON *reparsed_len = cJSON_ParseWithLength(printed, strlen(printed));
                if (reparsed_len != NULL) {
                    cJSON_Delete(reparsed_len);
                }
                cJSON_free(printed);
                printed = NULL;
            }
            cJSON_Delete(root4);
        } else {
            (void)cJSON_GetErrorPtr();
        }
    }

    free(input);
    return 0;
}