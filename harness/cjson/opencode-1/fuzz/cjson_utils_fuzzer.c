#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../cJSON.h"
#include "../cJSON_Utils.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size); /* required by C89 */

static void build_sample_string(char *buffer, size_t buffer_size, const uint8_t *data, size_t size)
{
    size_t i = 0;
    size_t limit = 0;

    if (buffer_size == 0)
    {
        return;
    }

    limit = buffer_size - 1;
    if (limit > size)
    {
        limit = size;
    }

    if (limit == 0)
    {
        buffer[0] = '\0';
        return;
    }

    for (i = 0; i < limit; i++)
    {
        buffer[i] = (char)('a' + (int)(data[i] % 26U));
    }
    buffer[limit] = '\0';
}

static cJSON_bool mutate_json_in_place(cJSON *item, const char *sample)
{
    cJSON *string_item = NULL;
    unsigned char first_byte = 0;

    if ((item == NULL) || (sample == NULL) || (sample[0] == '\0'))
    {
        return 0;
    }

    first_byte = (unsigned char)sample[0];

    switch (item->type & 0xFF)
    {
        case cJSON_Object:
            return cJSON_AddStringToObject(item, "fuzz", sample) != NULL;

        case cJSON_Array:
            string_item = cJSON_CreateString(sample);
            if (string_item == NULL)
            {
                return 0;
            }
            return cJSON_AddItemToArray(item, string_item);

        case cJSON_String:
            return cJSON_SetValuestring(item, sample) != NULL;

        case cJSON_Number:
            cJSON_SetNumberValue(item, (double)first_byte);
            return 1;

        case cJSON_True:
        case cJSON_False:
            return cJSON_SetBoolValue(item, (first_byte & 1U) != 0U) != cJSON_Invalid;

        default:
            return 0;
    }
}

static cJSON *select_target(cJSON *root)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (root->child != NULL)
    {
        if (root->child->next != NULL)
        {
            return root->child->next;
        }

        return root->child;
    }

    return root;
}

static void exercise_pointer_apis(cJSON *root)
{
    cJSON *target = NULL;
    cJSON *resolved = NULL;
    char *pointer = NULL;

    target = select_target(root);
    if (target == NULL)
    {
        return;
    }

    pointer = cJSONUtils_FindPointerFromObjectTo(root, target);
    if (pointer == NULL)
    {
        return;
    }

    resolved = cJSONUtils_GetPointer(root, pointer);
    if (resolved != NULL)
    {
        (void)cJSON_Compare(resolved, target, 1);
    }

    resolved = cJSONUtils_GetPointerCaseSensitive(root, pointer);
    if (resolved != NULL)
    {
        (void)cJSON_Compare(resolved, target, 1);
    }

    cJSON_free(pointer);
}

static void exercise_sorting(cJSON *root)
{
    cJSON *sorted = NULL;

    sorted = cJSON_Duplicate(root, 1);
    if (sorted == NULL)
    {
        return;
    }

    cJSONUtils_SortObject(sorted);
    cJSONUtils_SortObjectCaseSensitive(sorted);

    cJSON_Delete(sorted);
}

static void exercise_patch_apis(cJSON *root, const char *sample)
{
    cJSON *from = NULL;
    cJSON *to = NULL;
    cJSON *patches = NULL;
    cJSON *apply_target = NULL;
    cJSON *merge_from = NULL;
    cJSON *merge_to = NULL;
    cJSON *merge_patch = NULL;
    cJSON *merge_target = NULL;
    cJSON *merged = NULL;

    if ((root == NULL) || (sample == NULL) || (sample[0] == '\0'))
    {
        return;
    }

    from = cJSON_Duplicate(root, 1);
    to = cJSON_Duplicate(root, 1);
    if ((from == NULL) || (to == NULL))
    {
        goto cleanup;
    }

    if (!mutate_json_in_place(to, sample))
    {
        cJSON_Delete(to);
        to = cJSON_CreateString(sample);
        if (to == NULL)
        {
            goto cleanup;
        }
    }

    patches = cJSONUtils_GeneratePatches(from, to);
    apply_target = cJSON_Duplicate(root, 1);
    if ((patches != NULL) && (apply_target != NULL))
    {
        (void)cJSONUtils_ApplyPatches(apply_target, patches);
        (void)cJSON_Compare(apply_target, to, 1);
    }

    cJSON_Delete(patches);
    patches = cJSONUtils_GeneratePatchesCaseSensitive(from, to);
    if ((patches != NULL) && (apply_target != NULL))
    {
        cJSON_Delete(apply_target);
        apply_target = cJSON_Duplicate(root, 1);
        if (apply_target != NULL)
        {
            (void)cJSONUtils_ApplyPatchesCaseSensitive(apply_target, patches);
            (void)cJSON_Compare(apply_target, to, 1);
        }
    }

    merge_from = cJSON_Duplicate(root, 1);
    merge_to = cJSON_Duplicate(to, 1);
    merge_target = cJSON_Duplicate(root, 1);
    if ((merge_from != NULL) && (merge_to != NULL) && (merge_target != NULL))
    {
        merge_patch = cJSONUtils_GenerateMergePatch(merge_from, merge_to);
        if (merge_patch != NULL)
        {
            merged = cJSONUtils_MergePatch(merge_target, merge_patch);
            merge_target = NULL;
            if (merged != NULL)
            {
                (void)cJSON_Compare(merged, merge_to, 1);
                cJSON_Delete(merged);
                merged = NULL;
            }
        }
    }

    cJSON_Delete(merge_patch);
    merge_patch = NULL;
    cJSON_Delete(merge_from);
    merge_from = cJSON_Duplicate(root, 1);
    cJSON_Delete(merge_to);
    merge_to = cJSON_Duplicate(to, 1);
    cJSON_Delete(merge_target);
    merge_target = cJSON_Duplicate(root, 1);
    if ((merge_from != NULL) && (merge_to != NULL) && (merge_target != NULL))
    {
        merge_patch = cJSONUtils_GenerateMergePatchCaseSensitive(merge_from, merge_to);
        if (merge_patch != NULL)
        {
            merged = cJSONUtils_MergePatchCaseSensitive(merge_target, merge_patch);
            merge_target = NULL;
            if (merged != NULL)
            {
                (void)cJSON_Compare(merged, merge_to, 1);
            }
        }
    }

cleanup:
    cJSON_Delete(from);
    cJSON_Delete(to);
    cJSON_Delete(patches);
    cJSON_Delete(apply_target);
    cJSON_Delete(merge_from);
    cJSON_Delete(merge_to);
    cJSON_Delete(merge_patch);
    cJSON_Delete(merge_target);
    cJSON_Delete(merged);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char *input = NULL;
    cJSON *root = NULL;
    char sample[32];

    if ((data == NULL) || (size == 0))
    {
        return 0;
    }

    input = (char*)malloc(size + 1);
    if (input == NULL)
    {
        return 0;
    }

    memcpy(input, data, size);
    input[size] = '\0';

    build_sample_string(sample, sizeof(sample), data, size);

    root = cJSON_ParseWithLengthOpts(input, size, NULL, 0);
    if (root != NULL)
    {
        exercise_pointer_apis(root);
        exercise_sorting(root);
        exercise_patch_apis(root, sample);
        cJSON_Delete(root);
    }

    free(input);

    return 0;
}

#ifdef __cplusplus
}
#endif
