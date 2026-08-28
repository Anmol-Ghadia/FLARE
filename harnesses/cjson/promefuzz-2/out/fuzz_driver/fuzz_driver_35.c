// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_Version at cJSON.c:124:27 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_InitHooks at cJSON.c:209:20 in cJSON.h
// cJSON_ParseWithOpts at cJSON.c:1099:23 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1280:22 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_GetErrorPtr at cJSON.c:94:28 in cJSON.h
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

static void *fuzz_malloc(size_t sz)
{
    return malloc(sz);
}

static void fuzz_free(void *ptr)
{
    free(ptr);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON_Hooks hooks;
    const char *version;
    const char *parse_end = NULL;
    const char *error_ptr = NULL;
    cJSON *root1 = NULL;
    cJSON *root2 = NULL;
    char *printed1 = NULL;
    char *printed2 = NULL;
    char *input = NULL;
    size_t i;
    int require_null_terminated;

    version = cJSON_Version();
    (void)version;

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            (void)fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    hooks.malloc_fn = fuzz_malloc;
    hooks.free_fn = fuzz_free;
    cJSON_InitHooks(&hooks);

    input = (char *)malloc(Size + 1);
    if (input == NULL) {
        cJSON_InitHooks(NULL);
        return 0;
    }

    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    require_null_terminated = (Size > 0) ? (Data[0] & 1) : 0;

    root1 = cJSON_ParseWithOpts(input, &parse_end, require_null_terminated);
    if (root1 != NULL) {
        printed1 = cJSON_PrintUnformatted(root1);
        if (printed1 != NULL) {
            root2 = cJSON_Parse(printed1);
            if (root2 != NULL) {
                printed2 = cJSON_PrintUnformatted(root2);
                if (printed2 != NULL) {
                    hooks.free_fn(printed2);
                    printed2 = NULL;
                }
                cJSON_Delete(root2);
                root2 = NULL;
            } else {
                error_ptr = cJSON_GetErrorPtr();
                (void)error_ptr;
            }
            hooks.free_fn(printed1);
            printed1 = NULL;
        }
        cJSON_Delete(root1);
        root1 = NULL;
    } else {
        error_ptr = cJSON_GetErrorPtr();
        (void)error_ptr;
    }

    for (i = 0; i < Size; i++) {
        if (input[i] == '\0') {
            input[i] = ' ';
        }
    }

    root1 = cJSON_Parse(input);
    if (root1 != NULL) {
        printed1 = cJSON_PrintUnformatted(root1);
        if (printed1 != NULL) {
            hooks.free_fn(printed1);
            printed1 = NULL;
        }
        cJSON_Delete(root1);
        root1 = NULL;
    } else {
        error_ptr = cJSON_GetErrorPtr();
        (void)error_ptr;
    }

    free(input);
    cJSON_InitHooks(NULL);
    return 0;
}