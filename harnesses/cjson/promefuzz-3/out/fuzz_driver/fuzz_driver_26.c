// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1312:22 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_PrintUnformatted at cJSON.c:1312:22 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1317:22 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_PrintPreallocated at cJSON.c:1348:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_free at cJSON.c:3202:20 in cJSON.h
// cJSON_Parse at cJSON.c:1227:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2198:22 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2186:22 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToObject at cJSON.c:2119:26 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2210:22 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2784:23 in cJSON.h
// cJSON_Print at cJSON.c:1307:22 in cJSON.h
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

static int consume_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(value)) ? *size : sizeof(value);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static cJSON *build_json_from_input(const uint8_t *data, size_t size)
{
    cJSON *parsed = NULL;
    char *input = (char *)malloc(size + 1);
    if (input != NULL) {
        if (size > 0) {
            memcpy(input, data, size);
        }
        input[size] = '\0';
        parsed = cJSON_Parse(input);
        free(input);
    }

    if (parsed != NULL) {
        return parsed;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return NULL;
    }

    cJSON_AddNumberToObject(root, "size", (double)size);
    cJSON_AddBoolToObject(root, "nonempty", size > 0 ? 1 : 0);

    cJSON *arr = cJSON_CreateArray();
    if (arr != NULL) {
        size_t limit = size < 16 ? size : 16;
        for (size_t i = 0; i < limit; ++i) {
            cJSON *num = cJSON_CreateNumber((double)data[i]);
            if (num == NULL) {
                break;
            }
            cJSON_AddItemToArray(arr, num);
        }
        cJSON_AddItemToObject(root, "bytes", arr);
    }

    if (size > 0) {
        size_t slen = size < 32 ? size : 32;
        char *s = (char *)malloc(slen + 1);
        if (s != NULL) {
            memcpy(s, data, slen);
            s[slen] = '\0';
            cJSON_AddStringToObject(root, "snippet", s);
            free(s);
        }
    }

    return root;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *cursor = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *root = build_json_from_input(Data, Size);
    if (root == NULL) {
        return 0;
    }

    cJSON *dup_shallow = cJSON_Duplicate(root, 0);
    cJSON *dup_deep = cJSON_Duplicate(root, 1);

    char *printed_fmt = cJSON_Print(root);
    if (printed_fmt != NULL) {
        cJSON_free(printed_fmt);
    }

    char *printed_unfmt = cJSON_PrintUnformatted(root);
    if (printed_unfmt != NULL) {
        cJSON_free(printed_unfmt);
    }

    int prebuffer1 = consume_int(&cursor, &remaining);
    char *buffered1 = cJSON_PrintBuffered(root, prebuffer1, (remaining > 0) ? (cursor[0] & 1) : 0);
    if (buffered1 != NULL) {
        cJSON_free(buffered1);
    }

    int prebuffer2 = consume_int(&cursor, &remaining);
    char *buffered2 = cJSON_PrintBuffered(dup_deep ? dup_deep : root, prebuffer2, (remaining > 0) ? ((cursor[0] >> 1) & 1) : 1);
    if (buffered2 != NULL) {
        cJSON_free(buffered2);
    }

    {
        int extra = consume_int(&cursor, &remaining);
        int format = (remaining > 0) ? (cursor[0] & 1) : 0;
        size_t bufsize = Size + 8;
        if (extra > 0 && (size_t)extra < 4096) {
            bufsize += (size_t)extra;
        }

        char *buf = (char *)malloc(bufsize);
        if (buf != NULL) {
            (void)cJSON_PrintPreallocated(root, buf, (int)bufsize, format);
            if (bufsize > 0) {
                (void)cJSON_PrintPreallocated(root, buf, (int)(bufsize / 2), !format);
            }
            free(buf);
        }
    }

    {
        char smallbuf[4];
        (void)cJSON_PrintPreallocated(root, smallbuf, (int)sizeof(smallbuf), 0);
        (void)cJSON_PrintPreallocated(root, NULL, 16, 1);
        (void)cJSON_PrintPreallocated(root, smallbuf, -1, 1);
    }

    if (dup_shallow != NULL) {
        char *s = cJSON_PrintUnformatted(dup_shallow);
        if (s != NULL) {
            cJSON_free(s);
        }
    }

    if (dup_deep != NULL) {
        char *s1 = cJSON_Print(dup_deep);
        if (s1 != NULL) {
            cJSON_free(s1);
        }

        char *s2 = cJSON_PrintBuffered(dup_deep, 32, 1);
        if (s2 != NULL) {
            cJSON_free(s2);
        }

        char stackbuf[256];
        (void)cJSON_PrintPreallocated(dup_deep, stackbuf, (int)sizeof(stackbuf), 1);
    }

    cJSON_Delete(dup_shallow);
    cJSON_Delete(dup_deep);
    cJSON_Delete(root);

    cJSON_free(NULL);

    return 0;
}