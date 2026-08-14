#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../cJSON.h"
#include "../cJSON_Utils.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size); /* required by C89 */

static char *copy_input(const uint8_t *data, size_t size)
{
    char *copy = NULL;

    copy = (char*)malloc(size + 1);
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

static cJSON *parse_section(const uint8_t *data, size_t size, cJSON_bool require_termination)
{
    char *buffer = NULL;
    cJSON *json = NULL;

    buffer = copy_input(data, size);
    if (buffer == NULL)
    {
        return NULL;
    }

    json = cJSON_ParseWithLengthOpts(buffer, size + 1, NULL, require_termination);
    free(buffer);

    return json;
}

static void exercise_apply_patches(const cJSON *document, const cJSON *patches, cJSON_bool case_sensitive)
{
    cJSON *target = NULL;

    if (document != NULL)
    {
        target = cJSON_Duplicate(document, 1);
    }

    if (case_sensitive)
    {
        (void)cJSONUtils_ApplyPatchesCaseSensitive(target, patches);
    }
    else
    {
        (void)cJSONUtils_ApplyPatches(target, patches);
    }

    if (target != NULL)
    {
        cJSON_Delete(target);
    }
}

static void exercise_generated_patches(const cJSON *document, const cJSON *other, cJSON_bool case_sensitive)
{
    cJSON *from = NULL;
    cJSON *to = NULL;
    cJSON *generated = NULL;

    if ((document == NULL) || (other == NULL))
    {
        return;
    }

    from = cJSON_Duplicate(document, 1);
    to = cJSON_Duplicate(other, 1);
    if ((from == NULL) || (to == NULL))
    {
        cJSON_Delete(from);
        cJSON_Delete(to);
        return;
    }

    if (case_sensitive)
    {
        generated = cJSONUtils_GeneratePatchesCaseSensitive(from, to);
    }
    else
    {
        generated = cJSONUtils_GeneratePatches(from, to);
    }

    cJSON_Delete(from);
    cJSON_Delete(to);

    if (generated == NULL)
    {
        return;
    }

    exercise_apply_patches(document, generated, case_sensitive);
    cJSON_Delete(generated);
}

static void exercise_merge_patch(const cJSON *document, const cJSON *patch, cJSON_bool case_sensitive)
{
    cJSON *target = NULL;
    cJSON *merged = NULL;
    char *printed = NULL;

    if (patch == NULL)
    {
        return;
    }

    if (document != NULL)
    {
        target = cJSON_Duplicate(document, 1);
    }

    if (case_sensitive)
    {
        merged = cJSONUtils_MergePatchCaseSensitive(target, patch);
    }
    else
    {
        merged = cJSONUtils_MergePatch(target, patch);
    }

    if (merged == NULL)
    {
        return;
    }

    printed = cJSON_PrintUnformatted(merged);
    if (printed != NULL)
    {
        cJSON_free(printed);
    }
    cJSON_Delete(merged);
}

static void exercise_generated_merge_patch(const cJSON *document, const cJSON *other, cJSON_bool case_sensitive)
{
    cJSON *from = NULL;
    cJSON *to = NULL;
    cJSON *generated = NULL;

    if ((document == NULL) || (other == NULL))
    {
        return;
    }

    from = cJSON_Duplicate(document, 1);
    to = cJSON_Duplicate(other, 1);
    if ((from == NULL) || (to == NULL))
    {
        cJSON_Delete(from);
        cJSON_Delete(to);
        return;
    }

    if (case_sensitive)
    {
        generated = cJSONUtils_GenerateMergePatchCaseSensitive(from, to);
    }
    else
    {
        generated = cJSONUtils_GenerateMergePatch(from, to);
    }

    cJSON_Delete(from);
    cJSON_Delete(to);

    if (generated == NULL)
    {
        return;
    }

    exercise_merge_patch(document, generated, case_sensitive);
    cJSON_Delete(generated);
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    cJSON *document = NULL;
    cJSON *other = NULL;
    cJSON *patch = NULL;
    cJSON *sorted = NULL;
    cJSON *target = NULL;
    cJSON *case_sensitive_target = NULL;
    cJSON *patch_array = NULL;
    char *pointer = NULL;
    char *generated_pointer = NULL;
    size_t remaining = 0;
    size_t document_size = 0;
    size_t other_size = 0;
    size_t patch_size = 0;
    const uint8_t *cursor = NULL;

    if (size <= 4)
    {
        return 0;
    }

    remaining = size - 4;
    document_size = data[1] % (remaining + 1);
    remaining -= document_size;
    other_size = data[2] % (remaining + 1);
    remaining -= other_size;
    patch_size = data[3] % (remaining + 1);
    remaining -= patch_size;
    cursor = data + 4;

    document = parse_section(cursor, document_size, (data[0] & 1) != 0);
    cursor += document_size;
    other = parse_section(cursor, other_size, (data[0] & 2) != 0);
    cursor += other_size;
    patch = parse_section(cursor, patch_size, (data[0] & 4) != 0);
    cursor += patch_size;
    pointer = copy_input(cursor, remaining);
    if (pointer == NULL)
    {
        cJSON_Delete(document);
        cJSON_Delete(other);
        cJSON_Delete(patch);
        return 0;
    }

    if (document != NULL)
    {
        target = cJSONUtils_GetPointer(document, pointer);
        case_sensitive_target = cJSONUtils_GetPointerCaseSensitive(document, pointer);

        generated_pointer = cJSONUtils_FindPointerFromObjectTo(document,
            (case_sensitive_target != NULL) ? case_sensitive_target :
            ((target != NULL) ? target : document));
        if (generated_pointer != NULL)
        {
            cJSON_free(generated_pointer);
        }

        sorted = cJSON_Duplicate(document, 1);
        if (sorted != NULL)
        {
            cJSONUtils_SortObject(sorted);
            cJSONUtils_SortObjectCaseSensitive(sorted);
            cJSON_Delete(sorted);
        }

        patch_array = cJSON_CreateArray();
        if (patch_array != NULL)
        {
            cJSONUtils_AddPatchToArray(patch_array, "add", pointer,
                (target != NULL) ? target : document);
            cJSON_Delete(patch_array);
        }
    }

    exercise_apply_patches(document, patch, 0);
    exercise_apply_patches(document, patch, 1);
    exercise_generated_patches(document, other, 0);
    exercise_generated_patches(document, other, 1);
    exercise_merge_patch(document, patch, 0);
    exercise_merge_patch(document, patch, 1);
    exercise_generated_merge_patch(document, other, 0);
    exercise_generated_merge_patch(document, other, 1);

    free(pointer);
    cJSON_Delete(patch);
    cJSON_Delete(other);
    cJSON_Delete(document);

    return 0;
}

#ifdef __cplusplus
}
#endif
