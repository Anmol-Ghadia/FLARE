#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*Offset < Size) {
            v |= Data[*Offset];
            (*Offset)++;
        }
    }
    return v;
}

static int read_int(const uint8_t *Data, size_t Size, size_t *Offset) {
    return (int)read_u32(Data, Size, Offset);
}

static char *make_string(const uint8_t *Data, size_t Size, size_t *Offset) {
    size_t remaining = (*Offset < Size) ? (Size - *Offset) : 0;
    size_t len = remaining > 32 ? 32 : remaining;
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(s, Data + *Offset, len);
        *Offset += len;
    }
    s[len] = '\0';
    return s;
}

static cJSON *make_item_from_byte(uint8_t tag, const uint8_t *Data, size_t Size, size_t *Offset) {
    switch (tag % 6) {
        case 0:
            return cJSON_CreateNull();
        case 1:
            return cJSON_CreateBool((int)(tag & 1));
        case 2:
            return cJSON_CreateNumber((double)read_int(Data, Size, Offset));
        case 3: {
            char *s = make_string(Data, Size, Offset);
            cJSON *item = cJSON_CreateString(s ? s : "");
            free(s);
            return item;
        }
        case 4:
            return cJSON_CreateArray();
        case 5:
        default:
            return cJSON_CreateObject();
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *root = NULL;
    cJSON *parsed = NULL;
    cJSON *array = NULL;
    cJSON *detached = NULL;
    cJSON *replacement = NULL;
    cJSON *inserted = NULL;
    int idx1, idx2, idx3, idx4;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    parsed = cJSON_ParseWithLength((const char *)Data, Size);
    if (parsed != NULL && cJSON_IsArray(parsed)) {
        root = parsed;
        parsed = NULL;
    } else {
        cJSON_Delete(parsed);
        parsed = NULL;
        root = cJSON_CreateArray();
        if (root == NULL) {
            return 0;
        }

        {
            size_t count = (Size > 64) ? 64 : Size;
            size_t i;
            for (i = 0; i < count; ++i) {
                cJSON *item = make_item_from_byte(Data[i], Data, Size, &offset);
                if (item != NULL) {
                    cJSON_AddItemToArray(root, item);
                }
            }
        }
    }

    array = root;

    (void)cJSON_GetArraySize(NULL);
    (void)cJSON_GetArraySize(array);

    idx1 = read_int(Data, Size, &offset);
    idx2 = read_int(Data, Size, &offset);
    idx3 = read_int(Data, Size, &offset);
    idx4 = read_int(Data, Size, &offset);

    inserted = make_item_from_byte((offset < Size) ? Data[offset++] : 0, Data, Size, &offset);
    if (inserted != NULL) {
        if (!cJSON_InsertItemInArray(array, idx1, inserted)) {
            cJSON_Delete(inserted);
            inserted = NULL;
        }
    }

    replacement = make_item_from_byte((offset < Size) ? Data[offset++] : 1, Data, Size, &offset);
    if (replacement != NULL) {
        if (!cJSON_ReplaceItemInArray(array, idx2, replacement)) {
            cJSON_Delete(replacement);
            replacement = NULL;
        }
    }

    detached = cJSON_DetachItemFromArray(array, idx3);
    if (detached != NULL) {
        cJSON *extra = make_item_from_byte((offset < Size) ? Data[offset++] : 2, Data, Size, &offset);
        if (extra != NULL) {
            if (!cJSON_ReplaceItemViaPointer(array, detached, extra)) {
                cJSON_Delete(extra);
            }
        }
        cJSON_Delete(detached);
        detached = NULL;
    }

    {
        cJSON *first = cJSON_GetArrayItem(array, 0);
        cJSON *rep2 = make_item_from_byte((offset < Size) ? Data[offset++] : 3, Data, Size, &offset);
        if (rep2 != NULL) {
            if (!cJSON_ReplaceItemViaPointer(array, first, rep2)) {
                cJSON_Delete(rep2);
            }
        }
    }

    cJSON_DeleteItemFromArray(array, idx4);
    (void)cJSON_GetArraySize(array);

    {
        cJSON *tail_insert = make_item_from_byte((offset < Size) ? Data[offset++] : 4, Data, Size, &offset);
        if (tail_insert != NULL) {
            if (!cJSON_InsertItemInArray(array, cJSON_GetArraySize(array) + 10, tail_insert)) {
                cJSON_Delete(tail_insert);
            }
        }
    }

    {
        int sz = cJSON_GetArraySize(array);
        if (sz > 0) {
            cJSON *mid = cJSON_GetArrayItem(array, sz / 2);
            cJSON *rep3 = make_item_from_byte((offset < Size) ? Data[offset++] : 5, Data, Size, &offset);
            if (rep3 != NULL) {
                if (!cJSON_ReplaceItemViaPointer(array, mid, rep3)) {
                    cJSON_Delete(rep3);
                }
            }
        }
    }

    cJSON_Delete(root);
    return 0;
}