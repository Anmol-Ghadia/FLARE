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

static void reset_hooks_to_default(void) {
    cJSON_InitHooks(NULL);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON_Hooks hooks;

    /* Exercise hook initialization with defaults via explicit stdlib hooks. */
    hooks.malloc_fn = malloc;
    hooks.free_fn = free;
    cJSON_InitHooks(&hooks);

    /* Exercise cJSON_malloc/cJSON_free with fuzz-controlled size. */
    {
        size_t alloc_size = 0;
        if (Size >= sizeof(size_t)) {
            memcpy(&alloc_size, Data, sizeof(size_t));
        } else if (Size > 0) {
            for (size_t i = 0; i < Size; ++i) {
                alloc_size = (alloc_size << 8) ^ Data[i];
            }
        }
        alloc_size %= 4096;

        void *p = cJSON_malloc(alloc_size);
        if (p != NULL && alloc_size > 0) {
            memset(p, 0xA5, alloc_size);
        }
        cJSON_free(p);
        cJSON_free(NULL);
    }

    /* Prepare NUL-terminated input for parsing. */
    char *input = (char *)malloc(Size + 1);
    if (input == NULL) {
        reset_hooks_to_default();
        return 0;
    }
    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    /* Parse fuzz input as JSON. */
    cJSON *root = cJSON_Parse(input);
    if (root != NULL) {
        char *printed = cJSON_Print(root);
        if (printed != NULL) {
            cJSON *reparsed_pretty = cJSON_Parse(printed);
            if (reparsed_pretty != NULL) {
                char *printed2 = cJSON_PrintUnformatted(reparsed_pretty);
                if (printed2 != NULL) {
                    cJSON_free(printed2);
                }
                cJSON_Delete(reparsed_pretty);
            }
            cJSON_free(printed);
        }

        char *compact = cJSON_PrintUnformatted(root);
        if (compact != NULL) {
            cJSON *reparsed_compact = cJSON_Parse(compact);
            if (reparsed_compact != NULL) {
                char *printed3 = cJSON_Print(reparsed_compact);
                if (printed3 != NULL) {
                    cJSON_free(printed3);
                }
                cJSON_Delete(reparsed_compact);
            }
            cJSON_free(compact);
        }

        cJSON_Delete(root);
    }

    /* Reinitialize hooks to defaults and repeat some operations to explore states. */
    reset_hooks_to_default();

    {
        void *p2 = cJSON_malloc(Size % 1024);
        cJSON_free(p2);
    }

    {
        cJSON *root2 = cJSON_Parse(input);
        if (root2 != NULL) {
            char *s1 = cJSON_Print(root2);
            char *s2 = cJSON_PrintUnformatted(root2);
            if (s1 != NULL) {
                cJSON_free(s1);
            }
            if (s2 != NULL) {
                cJSON_free(s2);
            }
            cJSON_Delete(root2);
        }
    }

    free(input);
    return 0;
}