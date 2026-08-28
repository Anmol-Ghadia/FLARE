// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2132:22 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2132:22 in cJSON.h
// cJSON_AddFalseToObject at cJSON.c:2132:22 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2168:22 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2168:22 in cJSON.h
// cJSON_AddStringToObject at cJSON.c:2168:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_AddNumberToObject at cJSON.c:2156:22 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2539:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateRaw at cJSON.c:2539:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
// cJSON_PrintBuffered at cJSON.c:1285:22 in cJSON.h
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

static char *make_cstring(const uint8_t *data, size_t size)
{
    char *s = (char *)malloc(size + 1);
    if (s == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(s, data, size);
    }
    s[size] = '\0';
    return s;
}

static double bytes_to_double(const uint8_t *data, size_t size)
{
    double result = 0.0;
    size_t i;
    size_t limit = size < sizeof(uint64_t) ? size : sizeof(uint64_t);
    uint64_t v = 0;

    for (i = 0; i < limit; i++) {
        v |= ((uint64_t)data[i]) << (8 * i);
    }

    result = (double)(int64_t)v;
    if ((size > 0) && (data[0] & 1)) {
        result = -result;
    }
    if ((size > 1) && (data[1] & 1)) {
        result /= 3.14159265358979323846;
    }

    return result;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *raw = NULL;
    char *key1 = NULL;
    char *key2 = NULL;
    char *key3 = NULL;
    char *strval = NULL;
    char *rawstr = NULL;
    char *printed = NULL;
    FILE *fp = NULL;

    size_t p0, p1, p2, p3;
    size_t len1, len2, len3, len4;
    int prebuffer;
    cJSON_bool fmt;
    double number_value;

    if (Data == NULL) {
        return 0;
    }

    p0 = (Size > 0) ? (Data[0] % (Size + 1)) : 0;
    p1 = (Size > 1) ? (Data[1] % (Size + 1)) : p0;
    p2 = (Size > 2) ? (Data[2] % (Size + 1)) : p1;
    p3 = (Size > 3) ? (Data[3] % (Size + 1)) : p2;

    if (p1 < p0) { size_t t = p0; p0 = p1; p1 = t; }
    if (p2 < p1) { size_t t = p1; p1 = p2; p2 = t; }
    if (p3 < p2) { size_t t = p2; p2 = p3; p3 = t; }
    if (p1 < p0) { size_t t = p0; p0 = p1; p1 = t; }
    if (p2 < p1) { size_t t = p1; p1 = p2; p2 = t; }
    if (p1 < p0) { size_t t = p0; p0 = p1; p1 = t; }

    len1 = p0;
    len2 = (p1 >= p0) ? (p1 - p0) : 0;
    len3 = (p2 >= p1) ? (p2 - p1) : 0;
    len4 = (p3 >= p2) ? (p3 - p2) : 0;

    key1 = make_cstring(Data, len1);
    key2 = make_cstring(Data + p0, len2);
    key3 = make_cstring(Data + p1, len3);
    strval = make_cstring(Data + p2, len4);
    rawstr = make_cstring(Data + p3, Size - p3);

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        free(key1);
        free(key2);
        free(key3);
        free(strval);
        free(rawstr);
        return 0;
    }

    number_value = bytes_to_double(Data, Size);
    fmt = (Size > 4) ? (Data[4] & 1) : 0;
    prebuffer = (Size > 5) ? (int)((int8_t)Data[5]) : 0;

    (void)cJSON_AddFalseToObject(root, key1);
    (void)cJSON_AddFalseToObject(root, "");
    (void)cJSON_AddFalseToObject(root, key2);

    (void)cJSON_AddStringToObject(root, key2, strval);
    (void)cJSON_AddStringToObject(root, key3, "");
    (void)cJSON_AddStringToObject(root, "", rawstr);

    (void)cJSON_AddNumberToObject(root, key3, number_value);
    (void)cJSON_AddNumberToObject(root, key1, 0.0);
    (void)cJSON_AddNumberToObject(root, "", -number_value);

    raw = cJSON_CreateRaw(rawstr);
    if (raw != NULL) {
        cJSON_Delete(raw);
        raw = NULL;
    }

    raw = cJSON_CreateRaw("");
    if (raw != NULL) {
        cJSON_Delete(raw);
        raw = NULL;
    }

    printed = cJSON_PrintBuffered(root, prebuffer, fmt);
    if (printed != NULL) {
        free(printed);
        printed = NULL;
    }

    printed = cJSON_PrintBuffered(root, -1, fmt);
    if (printed != NULL) {
        free(printed);
        printed = NULL;
    }

    printed = cJSON_PrintBuffered(root, (int)Size, (cJSON_bool)!fmt);
    if (printed != NULL) {
        free(printed);
        printed = NULL;
    }

    cJSON_Delete(root);
    free(key1);
    free(key2);
    free(key3);
    free(strval);
    free(rawstr);

    return 0;
}