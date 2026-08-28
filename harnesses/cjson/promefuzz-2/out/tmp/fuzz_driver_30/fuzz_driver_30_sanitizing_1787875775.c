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

static cJSON *parse_input(const uint8_t *Data, size_t Size) {
    cJSON *root = cJSON_ParseWithLength((const char *)Data, Size);
    if (root != NULL) {
        return root;
    }

    cJSON *array = cJSON_CreateArray();
    if (array == NULL) {
        return NULL;
    }

    cJSON *s = cJSON_CreateString((const char *)Data);
    if (s != NULL) {
        cJSON_AddItemToArray(array, s);
    } else {
        cJSON_Delete(array);
        return NULL;
    }

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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *root;
    cJSON *child;
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

    exercise_item(root);

    for (child = root->child; child != NULL; child = child->next) {
        exercise_item(child);
        if (child->child != NULL) {
            cJSON *grandchild;
            for (grandchild = child->child; grandchild != NULL; grandchild = grandchild->next) {
                exercise_item(grandchild);
            }
        }
    }

    if (root->child != NULL) {
        (void)cJSON_Compare(root, root->child, 0);
        (void)cJSON_Compare(root, root->child, 1);
    }

    cJSON_Delete(root);
    return 0;
}