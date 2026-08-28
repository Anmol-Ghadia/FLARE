// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2222:22 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2222:22 in cJSON.h
// cJSON_AddRawToObject at cJSON.c:2222:22 in cJSON.h
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

static char *make_nul_terminated_copy(const uint8_t *data, size_t size) {
    char *out = (char *)malloc(size + 1);
    if (out == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(out, data, size);
    }
    out[size] = '\0';
    return out;
}

int LLVMFuzzerTestOneInput_18(const uint8_t *Data, size_t Size) {
    cJSON *root = NULL;
    cJSON *added = NULL;
    char *name = NULL;
    char *raw = NULL;
    size_t split;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    split = (Size > 0) ? (size_t)(Data[0] % (Size + 1)) : 0;

    name = make_nul_terminated_copy(Data, split);
    raw = make_nul_terminated_copy(Data + split, Size - split);

    if ((name != NULL) && (raw != NULL)) {
        added = cJSON_AddRawToObject(root, name, raw);
        (void)added;
    }

    if (Size > 1) {
        char small_name[2];
        char small_raw[2];

        small_name[0] = (char)Data[0];
        small_name[1] = '\0';
        small_raw[0] = (char)Data[1];
        small_raw[1] = '\0';

        added = cJSON_AddRawToObject(root, small_name, small_raw);
        (void)added;
    }

    added = cJSON_AddRawToObject(root, "", "");
    (void)added;

    cJSON_Delete(root);
    free(name);
    free(raw);

    return 0;
}