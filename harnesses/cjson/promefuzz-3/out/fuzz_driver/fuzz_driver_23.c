// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
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

static uint32_t read_u32(const uint8_t **data, size_t *size)
{
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_flag(const uint8_t **data, size_t *size)
{
    if (*size == 0) {
        return 0;
    }
    int v = (**data) & 1;
    (*data)++;
    (*size)--;
    return v;
}

static char *dup_bytes_as_cstring(const uint8_t *data, size_t len)
{
    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(out, data, len);
    }
    out[len] = '\0';
    return out;
}

static cJSON *make_manual_item(const uint8_t **data, size_t *size)
{
    cJSON *item = cJSON_CreateNull();
    if (item == NULL) {
        return NULL;
    }

    int selector = 0;
    if (*size > 0) {
        selector = **data % 8;
        (*data)++;
        (*size)--;
    }

    switch (selector) {
        case 0:
            item->type = cJSON_Raw;
            if (*size > 0) {
                size_t len = (**data) % (*size + 1);
                (*data)++;
                (*size)--;
                if (len > *size) {
                    len = *size;
                }
                item->valuestring = dup_bytes_as_cstring(*data, len);
                *data += len;
                *size -= len;
            } else {
                item->valuestring = dup_bytes_as_cstring((const uint8_t *)"", 0);
            }
            break;
        case 1:
            item->type = cJSON_False;
            break;
        case 2:
            item->type = cJSON_True;
            break;
        case 3:
            item->type = cJSON_NULL;
            break;
        case 4:
            item->type = cJSON_Number;
            item->valueint = (int)read_u32(data, size);
            item->valuedouble = (double)item->valueint;
            break;
        case 5:
            item->type = cJSON_String;
            if (*size > 0) {
                size_t len = (**data) % (*size + 1);
                (*data)++;
                (*size)--;
                if (len > *size) {
                    len = *size;
                }
                item->valuestring = dup_bytes_as_cstring(*data, len);
                *data += len;
                *size -= len;
            } else {
                item->valuestring = dup_bytes_as_cstring((const uint8_t *)"", 0);
            }
            break;
        case 6:
            item->type = cJSON_Array;
            break;
        default:
            item->type = cJSON_Object;
            break;
    }

    if (*size > 0 && ((**data) & 1)) {
        size_t len = (**data) % (*size + 1);
        (*data)++;
        (*size)--;
        if (len > *size) {
            len = *size;
        }
        item->string = dup_bytes_as_cstring(*data, len);
        *data += len;
        *size -= len;
    }

    return item;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *parsed1 = NULL;
    cJSON *parsed2 = NULL;
    cJSON *dup1 = NULL;
    cJSON *dup2 = NULL;
    cJSON *manual1 = NULL;
    cJSON *manual2 = NULL;

    size_t split = 0;
    if (remaining > 0) {
        split = read_u32(&ptr, &remaining);
        if (Size > 0) {
            split %= (Size + 1);
        } else {
            split = 0;
        }
    }

    if (split > Size) {
        split = Size;
    }

    char *json1 = dup_bytes_as_cstring(Data, split);
    char *json2 = dup_bytes_as_cstring(Data + split, Size - split);

    if (json1 != NULL) {
        parsed1 = cJSON_Parse(json1);
    }
    if (json2 != NULL) {
        parsed2 = cJSON_Parse(json2);
    }

    manual1 = make_manual_item(&ptr, &remaining);
    manual2 = make_manual_item(&ptr, &remaining);

    cJSON_IsRaw(NULL);
    cJSON_IsFalse(NULL);
    cJSON_IsTrue(NULL);
    cJSON_IsNull(NULL);
    cJSON_Compare(NULL, NULL, 0);
    cJSON_Compare(parsed1, NULL, 1);
    cJSON_Compare(NULL, parsed2, 0);
    cJSON_Duplicate(NULL, 0);
    cJSON_Duplicate(NULL, 1);

    if (parsed1 != NULL) {
        cJSON_IsRaw(parsed1);
        cJSON_IsFalse(parsed1);
        cJSON_IsTrue(parsed1);
        cJSON_IsNull(parsed1);

        dup1 = cJSON_Duplicate(parsed1, read_flag(&ptr, &remaining));
        cJSON_Compare(parsed1, parsed1, 0);
        cJSON_Compare(parsed1, parsed1, 1);

        if (dup1 != NULL) {
            cJSON_IsRaw(dup1);
            cJSON_IsFalse(dup1);
            cJSON_IsTrue(dup1);
            cJSON_IsNull(dup1);
            cJSON_Compare(parsed1, dup1, 0);
            cJSON_Compare(parsed1, dup1, 1);
            cJSON_Compare(dup1, parsed1, 0);
            cJSON_Compare(dup1, dup1, 1);
        }
    }

    if (parsed2 != NULL) {
        cJSON_IsRaw(parsed2);
        cJSON_IsFalse(parsed2);
        cJSON_IsTrue(parsed2);
        cJSON_IsNull(parsed2);

        dup2 = cJSON_Duplicate(parsed2, read_flag(&ptr, &remaining));
        cJSON_Compare(parsed2, parsed2, 0);
        cJSON_Compare(parsed2, parsed2, 1);

        if (dup2 != NULL) {
            cJSON_IsRaw(dup2);
            cJSON_IsFalse(dup2);
            cJSON_IsTrue(dup2);
            cJSON_IsNull(dup2);
            cJSON_Compare(parsed2, dup2, 0);
            cJSON_Compare(parsed2, dup2, 1);
            cJSON_Compare(dup2, parsed2, 1);
            cJSON_Compare(dup2, dup2, 0);
        }
    }

    if (parsed1 != NULL && parsed2 != NULL) {
        cJSON_Compare(parsed1, parsed2, 0);
        cJSON_Compare(parsed1, parsed2, 1);
        cJSON_Compare(parsed2, parsed1, 0);
        cJSON_Compare(parsed2, parsed1, 1);
    }

    if (manual1 != NULL) {
        cJSON_IsRaw(manual1);
        cJSON_IsFalse(manual1);
        cJSON_IsTrue(manual1);
        cJSON_IsNull(manual1);

        cJSON *mdup1 = cJSON_Duplicate(manual1, 0);
        cJSON *mdup1r = cJSON_Duplicate(manual1, 1);
        cJSON_Compare(manual1, manual1, 0);
        cJSON_Compare(manual1, manual1, 1);

        if (mdup1 != NULL) {
            cJSON_Compare(manual1, mdup1, 0);
            cJSON_Compare(manual1, mdup1, 1);
            cJSON_Delete(mdup1);
        }
        if (mdup1r != NULL) {
            cJSON_Compare(manual1, mdup1r, 0);
            cJSON_Compare(manual1, mdup1r, 1);
            cJSON_Delete(mdup1r);
        }
    }

    if (manual2 != NULL) {
        cJSON_IsRaw(manual2);
        cJSON_IsFalse(manual2);
        cJSON_IsTrue(manual2);
        cJSON_IsNull(manual2);

        cJSON *mdup2 = cJSON_Duplicate(manual2, 0);
        cJSON *mdup2r = cJSON_Duplicate(manual2, 1);
        cJSON_Compare(manual2, manual2, 0);
        cJSON_Compare(manual2, manual2, 1);

        if (mdup2 != NULL) {
            cJSON_Compare(manual2, mdup2, 0);
            cJSON_Compare(manual2, mdup2, 1);
            cJSON_Delete(mdup2);
        }
        if (mdup2r != NULL) {
            cJSON_Compare(manual2, mdup2r, 0);
            cJSON_Compare(manual2, mdup2r, 1);
            cJSON_Delete(mdup2r);
        }
    }

    if (manual1 != NULL && manual2 != NULL) {
        cJSON_Compare(manual1, manual2, 0);
        cJSON_Compare(manual1, manual2, 1);
        cJSON_Compare(manual2, manual1, 0);
        cJSON_Compare(manual2, manual1, 1);
    }

    if (parsed1 != NULL && manual1 != NULL) {
        cJSON_Compare(parsed1, manual1, 0);
        cJSON_Compare(parsed1, manual1, 1);
    }
    if (parsed2 != NULL && manual2 != NULL) {
        cJSON_Compare(parsed2, manual2, 0);
        cJSON_Compare(parsed2, manual2, 1);
    }

    cJSON_Delete(dup1);
    cJSON_Delete(dup2);
    cJSON_Delete(parsed1);
    cJSON_Delete(parsed2);
    cJSON_Delete(manual1);
    cJSON_Delete(manual2);
    free(json1);
    free(json2);

    return 0;
}