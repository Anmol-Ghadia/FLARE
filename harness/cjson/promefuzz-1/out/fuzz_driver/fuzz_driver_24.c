// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2506:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2489:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2539:23 in cJSON.h
// cJSON_GetStringValue at cJSON.c:99:22 in cJSON.h
// cJSON_Print at cJSON.c:1275:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *input = NULL;
    char *printed = NULL;
    char *printed_buffered = NULL;
    cJSON *item_string = NULL;
    cJSON *item_raw = NULL;
    cJSON *item_ref = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    input = make_nul_terminated_copy(Data, Size);
    if (input == NULL) {
        return 0;
    }

    item_string = cJSON_CreateString(input);
    if (item_string != NULL) {
        (void)cJSON_GetStringValue(item_string);

        printed = cJSON_Print(item_string);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }

        {
            int prebuffer_values[] = {
                0,
                1,
                2,
                8,
                16,
                64,
                (int)Size,
                (int)(Size + 1)
            };
            size_t i;
            for (i = 0; i < sizeof(prebuffer_values) / sizeof(prebuffer_values[0]); ++i) {
                printed_buffered = cJSON_PrintBuffered(item_string, prebuffer_values[i], 0);
                if (printed_buffered != NULL) {
                    free(printed_buffered);
                    printed_buffered = NULL;
                }

                printed_buffered = cJSON_PrintBuffered(item_string, prebuffer_values[i], 1);
                if (printed_buffered != NULL) {
                    free(printed_buffered);
                    printed_buffered = NULL;
                }
            }
        }
    }

    item_raw = cJSON_CreateRaw(input);
    if (item_raw != NULL) {
        (void)cJSON_GetStringValue(item_raw);

        printed = cJSON_Print(item_raw);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }

        printed_buffered = cJSON_PrintBuffered(item_raw, (int)Size, 0);
        if (printed_buffered != NULL) {
            free(printed_buffered);
            printed_buffered = NULL;
        }

        printed_buffered = cJSON_PrintBuffered(item_raw, (int)Size, 1);
        if (printed_buffered != NULL) {
            free(printed_buffered);
            printed_buffered = NULL;
        }
    }

    item_ref = cJSON_CreateStringReference(input);
    if (item_ref != NULL) {
        (void)cJSON_GetStringValue(item_ref);

        printed = cJSON_Print(item_ref);
        if (printed != NULL) {
            free(printed);
            printed = NULL;
        }

        printed_buffered = cJSON_PrintBuffered(item_ref, 0, 0);
        if (printed_buffered != NULL) {
            free(printed_buffered);
            printed_buffered = NULL;
        }

        printed_buffered = cJSON_PrintBuffered(item_ref, (int)(Size + 8), 1);
        if (printed_buffered != NULL) {
            free(printed_buffered);
            printed_buffered = NULL;
        }
    }

    cJSON_Delete(item_string);
    cJSON_Delete(item_raw);
    cJSON_Delete(item_ref);
    free(input);

    return 0;
}