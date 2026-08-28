// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_Version at cJSON.c:124:27 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1312:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
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
    static int hooks_initialized = 0;

    if (!hooks_initialized) {
        cJSON_InitHooks(NULL);
        hooks_initialized = 1;
    }

    write_dummy_file(Data, Size);

    (void)cJSON_Version();

    char *input = (char *)malloc(Size + 1);
    if (input == NULL) {
        return 0;
    }

    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    cJSON *root = cJSON_Parse(input);

    if (root != NULL) {
        char *printed = cJSON_Print(root);
        if (printed != NULL) {
            free(printed);
        }

        char *unformatted = cJSON_PrintUnformatted(root);
        if (unformatted != NULL) {
            free(unformatted);
        }

        int prebuffer_values[] = {
            -1,
            0,
            1,
            2,
            8,
            64,
            256,
            (Size > (size_t)INT32_MAX) ? INT32_MAX : (int)Size
        };

        size_t i;
        for (i = 0; i < sizeof(prebuffer_values) / sizeof(prebuffer_values[0]); i++) {
            char *buf0 = cJSON_PrintBuffered(root, prebuffer_values[i], 0);
            if (buf0 != NULL) {
                free(buf0);
            }

            char *buf1 = cJSON_PrintBuffered(root, prebuffer_values[i], 1);
            if (buf1 != NULL) {
                free(buf1);
            }
        }

        cJSON_Delete(root);
    }

    free(input);
    return 0;
}