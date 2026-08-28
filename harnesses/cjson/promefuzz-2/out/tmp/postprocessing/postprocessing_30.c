#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"

static cJSON *parse_input(const uint8_t *Data, size_t Size) {
    cJSON *root = cJSON_ParseWithLength((const char *)Data, Size);
    if (root != NULL) {
        return root;
    }

    cJSON *array = cJSON_CreateArray();
    char *buf;
    cJSON *s;

    if (array == NULL) {
        return NULL;
    }

    buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        cJSON_Delete(array);
        return NULL;
    }

    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';

    s = cJSON_CreateString(buf);
    free(buf);

    if (s == NULL) {
        cJSON_Delete(array);
        return NULL;
    }

    cJSON_AddItemToArray(array, s);
    return array;
}

static void exercise_item(cJSON *item) {
    cJSON *dup_shallow;
    cJSON *dup_deep;

    (void)cJSON_IsNull(item);
    (void)cJSON_IsInvalid(item);
    (void)cJSON_IsTrue(item);
    (void)cJSON_IsString(item);

    dup_shallow = cJSON_Duplicate(item, 0);
    dup_deep = cJSON_Duplicate(item, 1);

    if (dup_shallow != NULL) {
        (void)cJSON_IsNull(dup_shallow);
        (void)cJSON_IsInvalid(dup_shallow);
        (void)cJSON_IsTrue(dup_shallow);
        (void)cJSON_IsString(dup_shallow);

        (void)cJSON_Compare(item, dup_shallow, 0);
        (void)cJSON_Compare(item, dup_shallow, 1);
        (void)cJSON_Compare(dup_shallow, dup_shallow, 0);
    }

    if (dup_deep != NULL) {
        (void)cJSON_IsNull(dup_deep);
        (void)cJSON_IsInvalid(dup_deep);
        (void)cJSON_IsTrue(dup_deep);
        (void)cJSON_IsString(dup_deep);

        (void)cJSON_Compare(item, dup_deep, 0);
        (void)cJSON_Compare(item, dup_deep, 1);
        (void)cJSON_Compare(dup_deep, dup_deep, 1);
    }

    if ((dup_shallow != NULL) && (dup_deep != NULL)) {
        (void)cJSON_Compare(dup_shallow, dup_deep, 0);
        (void)cJSON_Compare(dup_shallow, dup_deep, 1);
    }

    cJSON_Delete(dup_shallow);
    cJSON_Delete(dup_deep);
}

static void exercise_tree(cJSON *item, int depth) {
    cJSON *child;

    if ((item == NULL) || (depth <= 0)) {
        return;
    }

    exercise_item(item);

    for (child = item->child; child != NULL; child = child->next) {
        exercise_tree(child, depth - 1);
        (void)cJSON_Compare(item, child, 0);
        (void)cJSON_Compare(item, child, 1);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    (void)cJSON_IsNull(NULL);
    (void)cJSON_IsInvalid(NULL);
    (void)cJSON_IsTrue(NULL);
    (void)cJSON_IsString(NULL);
    (void)cJSON_Compare(NULL, NULL, 0);
    (void)cJSON_Compare(NULL, NULL, 1);

    root = parse_input(Data, Size);
    if (root == NULL) {
        return 0;
    }

    exercise_tree(root, 4);

    if (root->child != NULL) {
        (void)cJSON_Compare(root, root->child, 0);
        (void)cJSON_Compare(root, root->child, 1);
    }

    cJSON_Delete(root);
    return 0;
}