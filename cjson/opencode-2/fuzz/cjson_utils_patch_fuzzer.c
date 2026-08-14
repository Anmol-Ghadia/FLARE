#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../cJSON.h"
#include "../cJSON_Utils.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size); /* required by C89 */

static unsigned char *copy_input(const uint8_t *data, size_t size)
{
    unsigned char *copy = NULL;

    copy = (unsigned char*)malloc(size + 1);
    if (copy == NULL)
    {
        return NULL;
    }

    if (size > 0)
    {
        memcpy(copy, data, size);
    }
    copy[size] = '\0';

    return copy;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const uint8_t *payload = NULL;
    size_t payload_size = 0;
    size_t split = 0;
    size_t from_size = 0;
    size_t to_size = 0;
    unsigned char *from_bytes = NULL;
    unsigned char *to_bytes = NULL;
    cJSON *from = NULL;
    cJSON *to = NULL;
    cJSON *patch_from = NULL;
    cJSON *patch_to = NULL;
    cJSON *patch_from_case_sensitive = NULL;
    cJSON *patch_to_case_sensitive = NULL;
    cJSON *patches = NULL;
    cJSON *patches_case_sensitive = NULL;
    cJSON *apply_target = NULL;
    cJSON *apply_target_case_sensitive = NULL;
    cJSON *manual_patches = NULL;
    cJSON *manual_target = NULL;
    cJSON *merge_from = NULL;
    cJSON *merge_to = NULL;
    cJSON *merge_from_case_sensitive = NULL;
    cJSON *merge_to_case_sensitive = NULL;
    cJSON *merge_patch = NULL;
    cJSON *merge_patch_case_sensitive = NULL;
    cJSON *merged = NULL;
    cJSON *merged_case_sensitive = NULL;

    if (size < 1)
    {
        return 0;
    }

    payload = data + 1;
    payload_size = size - 1;
    split = (size_t)data[0] % (payload_size + 1);
    from_size = split;
    to_size = payload_size - split;

    from_bytes = copy_input(payload, from_size);
    to_bytes = copy_input(payload + split, to_size);
    if ((from_bytes == NULL) || (to_bytes == NULL))
    {
        goto cleanup;
    }

    from = cJSON_ParseWithLengthOpts((const char*)from_bytes, from_size + 1, NULL, 0);
    to = cJSON_ParseWithLengthOpts((const char*)to_bytes, to_size + 1, NULL, 0);
    if ((from == NULL) || (to == NULL))
    {
        goto cleanup;
    }

    patch_from = cJSON_Duplicate(from, 1);
    patch_to = cJSON_Duplicate(to, 1);
    patch_from_case_sensitive = cJSON_Duplicate(from, 1);
    patch_to_case_sensitive = cJSON_Duplicate(to, 1);

    if ((patch_from != NULL) && (patch_to != NULL))
    {
        patches = cJSONUtils_GeneratePatches(patch_from, patch_to);
    }
    if ((patch_from_case_sensitive != NULL) && (patch_to_case_sensitive != NULL))
    {
        patches_case_sensitive = cJSONUtils_GeneratePatchesCaseSensitive(patch_from_case_sensitive, patch_to_case_sensitive);
    }

    if (patches != NULL)
    {
        apply_target = cJSON_Duplicate(from, 1);
        if (apply_target != NULL)
        {
            (void)cJSONUtils_ApplyPatches(apply_target, patches);
            (void)cJSON_Compare(apply_target, to, 0);
        }
    }
    if (patches_case_sensitive != NULL)
    {
        apply_target_case_sensitive = cJSON_Duplicate(from, 1);
        if (apply_target_case_sensitive != NULL)
        {
            (void)cJSONUtils_ApplyPatchesCaseSensitive(apply_target_case_sensitive, patches_case_sensitive);
            (void)cJSON_Compare(apply_target_case_sensitive, to, 1);
        }
    }

    manual_patches = cJSON_CreateArray();
    manual_target = cJSON_Duplicate(from, 1);
    if ((manual_patches != NULL) && (manual_target != NULL))
    {
        cJSONUtils_AddPatchToArray(manual_patches, "test", "", to);
        (void)cJSONUtils_ApplyPatches(manual_target, manual_patches);
    }

    merge_from = cJSON_Duplicate(from, 1);
    merge_to = cJSON_Duplicate(to, 1);
    merge_from_case_sensitive = cJSON_Duplicate(from, 1);
    merge_to_case_sensitive = cJSON_Duplicate(to, 1);

    if ((merge_from != NULL) && (merge_to != NULL))
    {
        merge_patch = cJSONUtils_GenerateMergePatch(merge_from, merge_to);
    }
    if ((merge_from_case_sensitive != NULL) && (merge_to_case_sensitive != NULL))
    {
        merge_patch_case_sensitive = cJSONUtils_GenerateMergePatchCaseSensitive(merge_from_case_sensitive, merge_to_case_sensitive);
    }

    if (merge_patch != NULL)
    {
        merged = cJSON_Duplicate(from, 1);
        if (merged != NULL)
        {
            merged = cJSONUtils_MergePatch(merged, merge_patch);
            if (merged != NULL)
            {
                (void)cJSON_Compare(merged, to, 0);
            }
        }
    }
    if (merge_patch_case_sensitive != NULL)
    {
        merged_case_sensitive = cJSON_Duplicate(from, 1);
        if (merged_case_sensitive != NULL)
        {
            merged_case_sensitive = cJSONUtils_MergePatchCaseSensitive(merged_case_sensitive, merge_patch_case_sensitive);
            if (merged_case_sensitive != NULL)
            {
                (void)cJSON_Compare(merged_case_sensitive, to, 1);
            }
        }
    }

cleanup:
    if (merged_case_sensitive != NULL)
    {
        cJSON_Delete(merged_case_sensitive);
    }
    if (merged != NULL)
    {
        cJSON_Delete(merged);
    }
    if (merge_patch_case_sensitive != NULL)
    {
        cJSON_Delete(merge_patch_case_sensitive);
    }
    if (merge_patch != NULL)
    {
        cJSON_Delete(merge_patch);
    }
    if (merge_to_case_sensitive != NULL)
    {
        cJSON_Delete(merge_to_case_sensitive);
    }
    if (merge_from_case_sensitive != NULL)
    {
        cJSON_Delete(merge_from_case_sensitive);
    }
    if (merge_to != NULL)
    {
        cJSON_Delete(merge_to);
    }
    if (merge_from != NULL)
    {
        cJSON_Delete(merge_from);
    }
    if (manual_target != NULL)
    {
        cJSON_Delete(manual_target);
    }
    if (manual_patches != NULL)
    {
        cJSON_Delete(manual_patches);
    }
    if (apply_target_case_sensitive != NULL)
    {
        cJSON_Delete(apply_target_case_sensitive);
    }
    if (apply_target != NULL)
    {
        cJSON_Delete(apply_target);
    }
    if (patches_case_sensitive != NULL)
    {
        cJSON_Delete(patches_case_sensitive);
    }
    if (patches != NULL)
    {
        cJSON_Delete(patches);
    }
    if (patch_to_case_sensitive != NULL)
    {
        cJSON_Delete(patch_to_case_sensitive);
    }
    if (patch_from_case_sensitive != NULL)
    {
        cJSON_Delete(patch_from_case_sensitive);
    }
    if (patch_to != NULL)
    {
        cJSON_Delete(patch_to);
    }
    if (patch_from != NULL)
    {
        cJSON_Delete(patch_from);
    }
    if (to != NULL)
    {
        cJSON_Delete(to);
    }
    if (from != NULL)
    {
        cJSON_Delete(from);
    }
    if (to_bytes != NULL)
    {
        free(to_bytes);
    }
    if (from_bytes != NULL)
    {
        free(from_bytes);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
