// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Compare at cJSON.c:3030:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
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

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *offset < size; i++, (*offset)++) {
        v = (v << 8) | data[*offset];
    }
    return v;
}

static double read_doubleish(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *offset < size; i++, (*offset)++) {
        v = (v << 8) | data[*offset];
    }
    return (double)(v % 1000000) / 13.0;
}

static char *make_string(const uint8_t *data, size_t size, size_t *offset, size_t max_len) {
    size_t remaining = (*offset < size) ? (size - *offset) : 0;
    size_t len = remaining;
    char *out;
    size_t i;

    if (len > max_len) {
        len = max_len;
    }

    out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        unsigned char c = data[*offset + i];
        if (c == '\0') {
            c = 'A';
        }
        out[i] = (char)c;
    }
    out[len] = '\0';
    *offset += len;
    return out;
}

static cJSON *make_manual_item(const uint8_t *data, size_t size, size_t *offset) {
    cJSON *item = cJSON_CreateNull();
    uint32_t selector;
    if (item == NULL) {
        return NULL;
    }

    selector = read_u32(data, size, offset) % 8U;
    switch (selector) {
        case 0:
            item->type = cJSON_Number;
            item->valuedouble = read_doubleish(data, size, offset);
            item->valueint = (int)item->valuedouble;
            break;
        case 1:
            item->type = cJSON_NULL;
            break;
        case 2:
            item->type = cJSON_Raw;
            free(item->valuestring);
            item->valuestring = make_string(data, size, offset, 64);
            break;
        case 3:
            item->type = cJSON_String;
            free(item->valuestring);
            item->valuestring = make_string(data, size, offset, 64);
            break;
        case 4:
            item->type = cJSON_True;
            break;
        case 5:
            item->type = cJSON_False;
            break;
        case 6:
            item->type = cJSON_Array;
            break;
        default:
            item->type = cJSON_Object;
            break;
    }

    if ((read_u32(data, size, offset) & 1U) != 0) {
        free(item->string);
        item->string = make_string(data, size, offset, 32);
    }

    return item;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    cJSON *parsed = NULL;
    cJSON *manual1 = NULL;
    cJSON *manual2 = NULL;
    cJSON *dup0 = NULL;
    cJSON *dup1 = NULL;
    cJSON *parsed_dup0 = NULL;
    cJSON *parsed_dup1 = NULL;
    cJSON *file_parsed = NULL;
    cJSON_bool recurse_flag;
    cJSON_bool case_sensitive;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    {
        char *input = (char *)malloc(Size + 1);
        if (input != NULL) {
            if (Size > 0) {
                memcpy(input, Data, Size);
            }
            input[Size] = '\0';
            parsed = cJSON_Parse(input);
            free(input);
        }
    }

    {
        FILE *rfp = fopen("./dummy_file", "rb");
        if (rfp != NULL) {
            long fsize;
            if (fseek(rfp, 0, SEEK_END) == 0) {
                fsize = ftell(rfp);
                if (fsize >= 0 && fseek(rfp, 0, SEEK_SET) == 0) {
                    char *buf = (char *)malloc((size_t)fsize + 1);
                    if (buf != NULL) {
                        size_t nread = fread(buf, 1, (size_t)fsize, rfp);
                        buf[nread] = '\0';
                        file_parsed = cJSON_Parse(buf);
                        free(buf);
                    }
                }
            }
            fclose(rfp);
        }
    }

    manual1 = make_manual_item(Data, Size, &offset);
    manual2 = make_manual_item(Data, Size, &offset);

    (void)cJSON_IsNumber(NULL);
    (void)cJSON_IsNull(NULL);
    (void)cJSON_IsRaw(NULL);
    (void)cJSON_IsString(NULL);

    if (manual1 != NULL) {
        (void)cJSON_IsNumber(manual1);
        (void)cJSON_IsNull(manual1);
        (void)cJSON_IsRaw(manual1);
        (void)cJSON_IsString(manual1);

        recurse_flag = (cJSON_bool)(read_u32(Data, Size, &offset) & 1U);
        dup0 = cJSON_Duplicate(manual1, recurse_flag);
        dup1 = cJSON_Duplicate(manual1, (cJSON_bool)!recurse_flag);

        case_sensitive = (cJSON_bool)(read_u32(Data, Size, &offset) & 1U);
        (void)cJSON_Compare(manual1, dup0, case_sensitive);
        (void)cJSON_Compare(manual1, dup1, (cJSON_bool)!case_sensitive);
        (void)cJSON_Compare(manual1, manual2, case_sensitive);
        (void)cJSON_Compare(manual1, manual1, case_sensitive);
        (void)cJSON_Compare(manual1, NULL, case_sensitive);
    }

    if (manual2 != NULL) {
        (void)cJSON_IsNumber(manual2);
        (void)cJSON_IsNull(manual2);
        (void)cJSON_IsRaw(manual2);
        (void)cJSON_IsString(manual2);
        (void)cJSON_Compare(manual2, manual1, 0);
        (void)cJSON_Compare(manual2, manual2, 1);
    }

    if (parsed != NULL) {
        (void)cJSON_IsNumber(parsed);
        (void)cJSON_IsNull(parsed);
        (void)cJSON_IsRaw(parsed);
        (void)cJSON_IsString(parsed);

        parsed_dup0 = cJSON_Duplicate(parsed, 0);
        parsed_dup1 = cJSON_Duplicate(parsed, 1);

        (void)cJSON_Compare(parsed, parsed_dup0, 0);
        (void)cJSON_Compare(parsed, parsed_dup1, 1);
        (void)cJSON_Compare(parsed, manual1, 0);
        (void)cJSON_Compare(parsed, parsed, 1);

        if (parsed->child != NULL) {
            (void)cJSON_IsNumber(parsed->child);
            (void)cJSON_IsNull(parsed->child);
            (void)cJSON_IsRaw(parsed->child);
            (void)cJSON_IsString(parsed->child);
            (void)cJSON_Compare(parsed->child, parsed->child, 0);
        }
    }

    if (file_parsed != NULL) {
        (void)cJSON_IsNumber(file_parsed);
        (void)cJSON_IsNull(file_parsed);
        (void)cJSON_IsRaw(file_parsed);
        (void)cJSON_IsString(file_parsed);
        (void)cJSON_Compare(file_parsed, parsed, 0);
        (void)cJSON_Compare(file_parsed, file_parsed, 1);
    }

    cJSON_Delete(parsed_dup1);
    cJSON_Delete(parsed_dup0);
    cJSON_Delete(dup1);
    cJSON_Delete(dup0);
    cJSON_Delete(file_parsed);
    cJSON_Delete(parsed);
    cJSON_Delete(manual2);
    cJSON_Delete(manual1);

    return 0;
}