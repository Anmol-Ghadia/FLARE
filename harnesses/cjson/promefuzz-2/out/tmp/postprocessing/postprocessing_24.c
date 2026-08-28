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

static char *make_fuzz_string(const uint8_t *data, size_t size)
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    cJSON *root = NULL;
    cJSON *tmp = NULL;
    char *name1 = NULL;
    char *name2 = NULL;
    char *name3 = NULL;
    size_t part1, part2, part3;
    cJSON_bool b1, b2;

    tmp = cJSON_CreateTrue();
    if (tmp != NULL) {
        cJSON_Delete(tmp);
        tmp = NULL;
    }

    tmp = cJSON_CreateFalse();
    if (tmp != NULL) {
        cJSON_Delete(tmp);
        tmp = NULL;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return 0;
    }

    part1 = Size / 3;
    part2 = (Size - part1) / 2;
    part3 = Size - part1 - part2;

    name1 = make_fuzz_string(Data, part1);
    name2 = make_fuzz_string(Data + part1, part2);
    name3 = make_fuzz_string(Data + part1 + part2, part3);

    b1 = (Size > 0) ? (cJSON_bool)(Data[0] & 1) : 0;
    b2 = (Size > 1) ? (cJSON_bool)(Data[1] & 1) : 1;

    (void)cJSON_AddTrueToObject(root, name1);
    (void)cJSON_AddFalseToObject(root, name2);
    (void)cJSON_AddBoolToObject(root, name3, b1);

    if (name1 != NULL) {
        (void)cJSON_AddBoolToObject(root, name1, b2);
        (void)cJSON_AddTrueToObject(root, name1);
        (void)cJSON_AddFalseToObject(root, name1);
    }

    (void)cJSON_AddTrueToObject(root, "");
    (void)cJSON_AddFalseToObject(root, "");
    (void)cJSON_AddBoolToObject(root, "", b1);

    (void)cJSON_AddTrueToObject(NULL, name1);
    (void)cJSON_AddFalseToObject(NULL, name2);
    (void)cJSON_AddBoolToObject(NULL, name3, b2);

    (void)cJSON_AddTrueToObject(root, NULL);
    (void)cJSON_AddFalseToObject(root, NULL);
    (void)cJSON_AddBoolToObject(root, NULL, b1);

    free(name1);
    free(name2);
    free(name3);
    cJSON_Delete(root);

    return 0;
}