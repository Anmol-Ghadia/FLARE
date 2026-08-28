#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "cJSON.h"
#include "cJSON_Utils.h"

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
    size_t json_size = 0;
    size_t pointer_size = 0;
    unsigned char *json_bytes = NULL;
    unsigned char *pointer = NULL;
    cJSON *document = NULL;
    cJSON *sorted = NULL;
    cJSON *target = NULL;
    cJSON *case_sensitive_target = NULL;
    char *resolved_pointer = NULL;

    if (size < 1)
    {
        return 0;
    }

    payload = data + 1;
    payload_size = size - 1;
    split = (size_t)data[0] % (payload_size + 1);
    json_size = split;
    pointer_size = payload_size - split;

    json_bytes = copy_input(payload, json_size);
    pointer = copy_input(payload + split, pointer_size);
    if ((json_bytes == NULL) || (pointer == NULL))
    {
        goto cleanup;
    }

    document = cJSON_ParseWithLengthOpts((const char*)json_bytes, json_size + 1, NULL, 0);
    if (document == NULL)
    {
        goto cleanup;
    }

    resolved_pointer = cJSONUtils_FindPointerFromObjectTo(document, document);
    if (resolved_pointer != NULL)
    {
        cJSON_free(resolved_pointer);
        resolved_pointer = NULL;
    }

    target = cJSONUtils_GetPointer(document, (const char*)pointer);
    case_sensitive_target = cJSONUtils_GetPointerCaseSensitive(document, (const char*)pointer);
    if (target == NULL)
    {
        target = case_sensitive_target;
    }

    if (target != NULL)
    {
        resolved_pointer = cJSONUtils_FindPointerFromObjectTo(document, target);
    }

    sorted = cJSON_Duplicate(document, 1);
    if (sorted != NULL)
    {
        cJSONUtils_SortObject(sorted);
        cJSONUtils_SortObjectCaseSensitive(sorted);
    }

cleanup:
    if (resolved_pointer != NULL)
    {
        cJSON_free(resolved_pointer);
    }
    if (sorted != NULL)
    {
        cJSON_Delete(sorted);
    }
    if (document != NULL)
    {
        cJSON_Delete(document);
    }
    if (pointer != NULL)
    {
        free(pointer);
    }
    if (json_bytes != NULL)
    {
        free(json_bytes);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
