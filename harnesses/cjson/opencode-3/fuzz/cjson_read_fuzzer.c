#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "cJSON.h"

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

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    cJSON *json = NULL;
    cJSON *duplicate = NULL;
    cJSON *minified_json = NULL;
    char *input = NULL;
    char *minified = NULL;
    char *printed_json = NULL;
    char *preallocated = NULL;
    size_t payload_size = 0;
    size_t preallocated_length = 0;
    cJSON_bool buffered = 0;
    cJSON_bool formatted = 0;
    cJSON_bool require_termination = 0;
    cJSON_bool reparse_minified = 0;

    if (size <= 2)
    {
        return 0;
    }

    buffered = (data[0] & 1) != 0;
    formatted = (data[0] & 2) != 0;
    require_termination = (data[1] & 1) != 0;
    reparse_minified = (data[1] & 2) != 0;
    payload_size = size - 2;

    input = copy_input(data + 2, payload_size);
    if (input == NULL)
    {
        return 0;
    }

    json = cJSON_ParseWithLengthOpts(input, payload_size + 1, NULL, require_termination);
    if (json == NULL)
    {
        (void)cJSON_GetErrorPtr();
    }
    else
    {
        duplicate = cJSON_Duplicate(json, 1);
        if (duplicate != NULL)
        {
            (void)cJSON_Compare(json, duplicate, 0);
            (void)cJSON_Compare(json, duplicate, 1);
        }

        if (buffered)
        {
            printed_json = cJSON_PrintBuffered(json, 1 + (int)(data[0] & 63), formatted);
        }
        else if (formatted)
        {
            printed_json = cJSON_Print(json);
        }
        else
        {
            printed_json = cJSON_PrintUnformatted(json);
        }

        if (printed_json != NULL)
        {
            cJSON_free(printed_json);
        }

        preallocated_length = (payload_size < 4064) ? (payload_size + 32) : 4096;
        preallocated = (char*)malloc(preallocated_length);
        if (preallocated != NULL)
        {
            (void)cJSON_PrintPreallocated(json, preallocated, (int)preallocated_length, formatted);
            free(preallocated);
        }
    }

    minified = copy_input(data + 2, payload_size);
    if (minified != NULL)
    {
        cJSON_Minify(minified);
        if (reparse_minified)
        {
            minified_json = cJSON_ParseWithOpts(minified, NULL, require_termination);
            if (minified_json != NULL)
            {
                cJSON_Delete(minified_json);
            }
        }
        free(minified);
    }

    if (duplicate != NULL)
    {
        cJSON_Delete(duplicate);
    }
    if (json != NULL)
    {
        cJSON_Delete(json);
    }
    free(input);

    return 0;
}

#ifdef __cplusplus
}
#endif
