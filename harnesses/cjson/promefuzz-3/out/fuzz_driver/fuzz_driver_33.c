// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2309:23 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2316:23 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObject at cJSON.c:2309:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemFromObjectCaseSensitive at cJSON.c:2316:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2258:23 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_DeleteItemFromObject at cJSON.c:2323:20 in cJSON.h
// cJSON_DeleteItemFromObjectCaseSensitive at cJSON.c:2328:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2494:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_IsObject at cJSON.c:3052:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
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

static size_t fuzz_strnlen(const char *s, size_t maxlen) {
    size_t i;
    for (i = 0; i < maxlen; ++i) {
        if (s[i] == '\0') {
            break;
        }
    }
    return i;
}

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

static cJSON *make_item_from_byte(uint8_t tag, const char *text, size_t text_len) {
    switch (tag % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((tag & 1) ? 1 : 0);
        case 2:
            return cJSON_CreateNumber((double)tag);
        case 3:
            return cJSON_CreateString(text != NULL ? text : "");
        case 4:
            return cJSON_CreateArray();
        case 5:
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char *input = NULL;
    cJSON *root = NULL;
    cJSON *parsed = NULL;
    cJSON *detached = NULL;
    cJSON *detached_cs = NULL;
    cJSON *detached_ptr = NULL;
    cJSON *item1 = NULL;
    cJSON *item2 = NULL;
    cJSON *item3 = NULL;
    cJSON *item4 = NULL;
    cJSON *item5 = NULL;
    cJSON *child = NULL;
    const char *const_key1 = "const_key";
    const char *const_key2 = "CONST_KEY";
    const char *const_key3 = "";
    char *dynkey1 = NULL;
    char *dynkey2 = NULL;
    char *dynkey3 = NULL;
    size_t len1, len2, len3;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    input = make_nul_terminated_copy(Data, Size);
    if (input == NULL) {
        return 0;
    }

    parsed = cJSON_Parse(input);
    if (parsed != NULL && cJSON_IsObject(parsed)) {
        root = parsed;
        parsed = NULL;
    } else {
        if (parsed != NULL) {
            cJSON_Delete(parsed);
            parsed = NULL;
        }
        root = cJSON_CreateObject();
    }

    if (root == NULL) {
        free(input);
        return 0;
    }

    len1 = Size > 0 ? (Size / 3) : 0;
    len2 = Size > len1 ? ((Size - len1) / 2) : 0;
    len3 = Size > (len1 + len2) ? (Size - len1 - len2) : 0;

    dynkey1 = (char *)malloc(len1 + 1);
    dynkey2 = (char *)malloc(len2 + 1);
    dynkey3 = (char *)malloc(len3 + 1);

    if (dynkey1 != NULL) {
        if (len1 > 0) {
            memcpy(dynkey1, Data, len1);
        }
        dynkey1[len1] = '\0';
    }
    if (dynkey2 != NULL) {
        if (len2 > 0) {
            memcpy(dynkey2, Data + len1, len2);
        }
        dynkey2[len2] = '\0';
    }
    if (dynkey3 != NULL) {
        if (len3 > 0) {
            memcpy(dynkey3, Data + len1 + len2, len3);
        }
        dynkey3[len3] = '\0';
    }

    item1 = make_item_from_byte(Size > 0 ? Data[0] : 0, input, fuzz_strnlen(input, Size));
    item2 = make_item_from_byte(Size > 1 ? Data[1] : 1, input, fuzz_strnlen(input, Size));
    item3 = make_item_from_byte(Size > 2 ? Data[2] : 2, input, fuzz_strnlen(input, Size));
    item4 = make_item_from_byte(Size > 3 ? Data[3] : 3, input, fuzz_strnlen(input, Size));
    item5 = make_item_from_byte(Size > 4 ? Data[4] : 4, input, fuzz_strnlen(input, Size));

    if (item1 != NULL) {
        if (!cJSON_AddItemToObjectCS(root, const_key1, item1)) {
            cJSON_Delete(item1);
            item1 = NULL;
        }
    }
    if (item2 != NULL) {
        if (!cJSON_AddItemToObjectCS(root, const_key2, item2)) {
            cJSON_Delete(item2);
            item2 = NULL;
        }
    }
    if (item3 != NULL) {
        if (!cJSON_AddItemToObjectCS(root, const_key3, item3)) {
            cJSON_Delete(item3);
            item3 = NULL;
        }
    }
    if (item4 != NULL && dynkey1 != NULL) {
        if (!cJSON_AddItemToObject(root, dynkey1, item4)) {
            cJSON_Delete(item4);
            item4 = NULL;
        }
    } else if (item4 != NULL) {
        cJSON_Delete(item4);
        item4 = NULL;
    }
    if (item5 != NULL && dynkey2 != NULL) {
        if (!cJSON_AddItemToObject(root, dynkey2, item5)) {
            cJSON_Delete(item5);
            item5 = NULL;
        }
    } else if (item5 != NULL) {
        cJSON_Delete(item5);
        item5 = NULL;
    }

    detached = cJSON_DetachItemFromObject(root, const_key1);
    if (detached != NULL) {
        if (!cJSON_AddItemToObjectCS(root, const_key1, detached)) {
            cJSON_Delete(detached);
            detached = NULL;
        }
    }

    detached_cs = cJSON_DetachItemFromObjectCaseSensitive(root, const_key2);
    if (detached_cs != NULL) {
        if (!cJSON_AddItemToObjectCS(root, const_key2, detached_cs)) {
            cJSON_Delete(detached_cs);
            detached_cs = NULL;
        }
    }

    if (dynkey1 != NULL) {
        detached = cJSON_DetachItemFromObject(root, dynkey1);
        if (detached != NULL) {
            if (!cJSON_AddItemToObject(root, dynkey1, detached)) {
                cJSON_Delete(detached);
                detached = NULL;
            }
        }
    }

    if (dynkey2 != NULL) {
        detached_cs = cJSON_DetachItemFromObjectCaseSensitive(root, dynkey2);
        if (detached_cs != NULL) {
            if (!cJSON_AddItemToObject(root, dynkey2, detached_cs)) {
                cJSON_Delete(detached_cs);
                detached_cs = NULL;
            }
        }
    }

    child = root->child;
    if (child != NULL) {
        detached_ptr = cJSON_DetachItemViaPointer(root, child);
        if (detached_ptr != NULL) {
            const char *rekey = detached_ptr->string != NULL ? detached_ptr->string : "reattach";
            if (!cJSON_AddItemToObject(root, rekey, detached_ptr)) {
                cJSON_Delete(detached_ptr);
                detached_ptr = NULL;
            }
        }
    }

    cJSON_DeleteItemFromObject(root, const_key1);
    cJSON_DeleteItemFromObjectCaseSensitive(root, const_key2);

    if (dynkey1 != NULL) {
        cJSON_DeleteItemFromObject(root, dynkey1);
    }
    if (dynkey2 != NULL) {
        cJSON_DeleteItemFromObjectCaseSensitive(root, dynkey2);
    }
    if (dynkey3 != NULL) {
        cJSON_DeleteItemFromObject(root, dynkey3);
        cJSON_DeleteItemFromObjectCaseSensitive(root, dynkey3);
    }

    cJSON_Delete(root);
    free(dynkey1);
    free(dynkey2);
    free(dynkey3);
    free(input);
    return 0;
}