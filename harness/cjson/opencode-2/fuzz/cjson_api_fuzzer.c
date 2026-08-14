#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../cJSON.h"

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
    unsigned char *json_bytes = NULL;
    unsigned char *minify_buffer = NULL;
    cJSON *json = NULL;
    cJSON *duplicate = NULL;
    char *formatted = NULL;
    char *unformatted = NULL;
    char *buffered = NULL;
    char *preallocated = NULL;
    size_t json_size = 0;
    size_t printed_length = 0;
    int require_null_terminated = 0;
    int case_sensitive = 0;
    int buffered_guess = 0;
    cJSON_bool pretty = 0;

    if (size < 1)
    {
        return 0;
    }

    require_null_terminated = (data[0] & 1) != 0;
    pretty = (cJSON_bool)((data[0] & 2) != 0);
    case_sensitive = (data[0] & 4) != 0;
    json_size = size - 1;

    json_bytes = copy_input(data + 1, json_size);
    if (json_bytes == NULL)
    {
        return 0;
    }

    json = cJSON_ParseWithLengthOpts((const char*)json_bytes, json_size + 1, NULL, (cJSON_bool)require_null_terminated);
    if (json == NULL)
    {
        goto cleanup;
    }

    duplicate = cJSON_Duplicate(json, 1);
    if (duplicate != NULL)
    {
        (void)cJSON_Compare(json, duplicate, (cJSON_bool)case_sensitive);
    }

    formatted = cJSON_Print(json);
    unformatted = cJSON_PrintUnformatted(json);

    if (json_size >= (size_t)(INT_MAX - 8))
    {
        buffered_guess = INT_MAX;
    }
    else
    {
        buffered_guess = (int)json_size + 8;
    }
    buffered = cJSON_PrintBuffered(json, buffered_guess, pretty);

    if (buffered != NULL)
    {
        printed_length = strlen(buffered);
        if (printed_length < (size_t)(INT_MAX - 8))
        {
            preallocated = (char*)malloc(printed_length + 8);
            if (preallocated != NULL)
            {
                memset(preallocated, 0, printed_length + 8);
                (void)cJSON_PrintPreallocated((duplicate != NULL) ? duplicate : json, preallocated, (int)printed_length + 8, pretty);
            }
        }

        minify_buffer = copy_input((const uint8_t*)buffered, printed_length);
        if (minify_buffer != NULL)
        {
            cJSON_Minify((char*)minify_buffer);
        }
    }

cleanup:
    if (preallocated != NULL)
    {
        free(preallocated);
    }
    if (buffered != NULL)
    {
        cJSON_free(buffered);
    }
    if (unformatted != NULL)
    {
        cJSON_free(unformatted);
    }
    if (formatted != NULL)
    {
        cJSON_free(formatted);
    }
    if (duplicate != NULL)
    {
        cJSON_Delete(duplicate);
    }
    if (json != NULL)
    {
        cJSON_Delete(json);
    }
    if (minify_buffer != NULL)
    {
        free(minify_buffer);
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
