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

static cJSON *make_manual_node_from_byte(uint8_t tag, const uint8_t *data, size_t size) {
    cJSON *item = (cJSON *)calloc(1, sizeof(cJSON));
    if (item == NULL) {
        return NULL;
    }

    switch (tag % 8) {
        case 0:
            item->type = cJSON_NULL;
            break;
        case 1:
            item->type = cJSON_String;
            item->valuestring = (char *)malloc(size + 1);
            if (item->valuestring != NULL) {
                if (size > 0) {
                    memcpy(item->valuestring, data, size);
                }
                item->valuestring[size] = '\0';
            }
            break;
        case 2:
            item->type = cJSON_Number;
            if (size >= sizeof(double)) {
                memcpy(&item->valuedouble, data, sizeof(double));
            } else {
                item->valuedouble = (double)tag;
            }
            item->valueint = (int)item->valuedouble;
            break;
        case 3:
            item->type = cJSON_True;
            break;
        case 4:
            item->type = cJSON_False;
            break;
        case 5:
            item->type = cJSON_Raw;
            item->valuestring = (char *)malloc(size + 1);
            if (item->valuestring != NULL) {
                if (size > 0) {
                    memcpy(item->valuestring, data, size);
                }
                item->valuestring[size] = '\0';
            }
            break;
        case 6:
            item->type = cJSON_Array;
            break;
        default:
            item->type = cJSON_Object;
            break;
    }

    return item;
}

static void free_manual_node(cJSON *item) {
    if (item == NULL) {
        return;
    }
    free(item->valuestring);
    free(item->string);
    free(item);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *parsed1 = NULL;
    cJSON *parsed2 = NULL;
    cJSON *dup1 = NULL;
    cJSON *manual1 = NULL;
    cJSON *manual2 = NULL;
    FILE *fp;
    char *buf1;
    char *buf2;
    size_t split = 0;
    cJSON_bool cs = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    buf1 = (char *)malloc(Size + 1);
    if (buf1 == NULL) {
        return 0;
    }
    if (Size > 0) {
        memcpy(buf1, Data, Size);
    }
    buf1[Size] = '\0';

    split = (Size > 0) ? (Data[0] % (Size + 1)) : 0;
    buf2 = (char *)malloc((Size - split) + 1);
    if (buf2 == NULL) {
        free(buf1);
        return 0;
    }
    if (Size > split) {
        memcpy(buf2, Data + split, Size - split);
    }
    buf2[Size - split] = '\0';

    parsed1 = cJSON_Parse(buf1);
    parsed2 = cJSON_Parse(buf2);

    cJSON_IsRaw(NULL);
    cJSON_IsNull(NULL);
    cJSON_IsString(NULL);
    cJSON_IsBool(NULL);
    cJSON_IsNumber(NULL);
    cJSON_Compare(NULL, NULL, 0);
    cJSON_Compare(parsed1, NULL, 1);
    cJSON_Compare(NULL, parsed2, 0);

    if (parsed1 != NULL) {
        cJSON_IsRaw(parsed1);
        cJSON_IsNull(parsed1);
        cJSON_IsString(parsed1);
        cJSON_IsBool(parsed1);
        cJSON_IsNumber(parsed1);

        dup1 = cJSON_Duplicate(parsed1, 1);
        if (dup1 != NULL) {
            cJSON_Compare(parsed1, dup1, 0);
            cJSON_Compare(parsed1, dup1, 1);

            cJSON_IsRaw(dup1);
            cJSON_IsNull(dup1);
            cJSON_IsString(dup1);
            cJSON_IsBool(dup1);
            cJSON_IsNumber(dup1);
        }

        cJSON_Compare(parsed1, parsed1, 0);
        cJSON_Compare(parsed1, parsed1, 1);

        if (parsed1->child != NULL) {
            cJSON_IsRaw(parsed1->child);
            cJSON_IsNull(parsed1->child);
            cJSON_IsString(parsed1->child);
            cJSON_IsBool(parsed1->child);
            cJSON_IsNumber(parsed1->child);
            cJSON_Compare(parsed1, parsed1->child, 0);
            cJSON_Compare(parsed1->child, parsed1->child, 1);
        }
    }

    if (parsed2 != NULL) {
        cJSON_IsRaw(parsed2);
        cJSON_IsNull(parsed2);
        cJSON_IsString(parsed2);
        cJSON_IsBool(parsed2);
        cJSON_IsNumber(parsed2);

        cJSON_Compare(parsed2, parsed2, 0);
        cJSON_Compare(parsed2, parsed2, 1);

        if (parsed2->child != NULL) {
            cJSON_IsRaw(parsed2->child);
            cJSON_IsNull(parsed2->child);
            cJSON_IsString(parsed2->child);
            cJSON_IsBool(parsed2->child);
            cJSON_IsNumber(parsed2->child);
            cJSON_Compare(parsed2, parsed2->child, 1);
        }
    }

    if (parsed1 != NULL && parsed2 != NULL) {
        cs = (Size > 1) ? (Data[1] & 1) : 0;
        cJSON_Compare(parsed1, parsed2, cs);
        cJSON_Compare(parsed2, parsed1, !cs);
    }

    manual1 = make_manual_node_from_byte(Size > 0 ? Data[0] : 0, Data, Size);
    manual2 = make_manual_node_from_byte(Size > 1 ? Data[1] : 1, Data, Size);

    if (manual1 != NULL) {
        cJSON_IsRaw(manual1);
        cJSON_IsNull(manual1);
        cJSON_IsString(manual1);
        cJSON_IsBool(manual1);
        cJSON_IsNumber(manual1);
        cJSON_Compare(manual1, manual1, 0);
        cJSON_Compare(manual1, parsed1, 1);
    }

    if (manual2 != NULL) {
        cJSON_IsRaw(manual2);
        cJSON_IsNull(manual2);
        cJSON_IsString(manual2);
        cJSON_IsBool(manual2);
        cJSON_IsNumber(manual2);
        cJSON_Compare(manual2, manual2, 1);
        cJSON_Compare(manual2, parsed2, 0);
    }

    if (manual1 != NULL && manual2 != NULL) {
        cJSON_Compare(manual1, manual2, 0);
        cJSON_Compare(manual1, manual2, 1);
    }

    free_manual_node(manual1);
    free_manual_node(manual2);
    cJSON_Delete(dup1);
    cJSON_Delete(parsed1);
    cJSON_Delete(parsed2);
    free(buf2);
    free(buf1);

    return 0;
}