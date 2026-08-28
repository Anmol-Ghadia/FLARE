#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "cJSON.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size); /* required by C89 */

static int get_prebuffer_length(size_t size)
{
    if (size == 0)
    {
        return 1;
    }

    if (size > (size_t)(INT_MAX - 1))
    {
        return INT_MAX;
    }

    return (int)size + 1;
}

static void exercise_preallocated_print(cJSON *json, const char *printed)
{
    char *buffer = NULL;
    size_t printed_length = 0;
    int buffer_length = 0;

    if ((json == NULL) || (printed == NULL))
    {
        return;
    }

    printed_length = strlen(printed);
    if (printed_length > (size_t)(INT_MAX - 6))
    {
        return;
    }

    buffer_length = (int)printed_length + 6;
    buffer = (char*)malloc((size_t)buffer_length);
    if (buffer == NULL)
    {
        return;
    }

    memset(buffer, 0, (size_t)buffer_length);
    (void)cJSON_PrintPreallocated(json, buffer, buffer_length, 0);
    memset(buffer, 0, (size_t)buffer_length);
    (void)cJSON_PrintPreallocated(json, buffer, buffer_length, 1);

    free(buffer);
}

static void exercise_tree(cJSON *json, size_t size)
{
    cJSON *duplicate = NULL;
    cJSON *reparsed = NULL;
    char *formatted = NULL;
    char *unformatted = NULL;
    char *buffered = NULL;
    int prebuffer = 0;

    if (json == NULL)
    {
        return;
    }

    duplicate = cJSON_Duplicate(json, 1);
    if (duplicate != NULL)
    {
        (void)cJSON_Compare(json, duplicate, 1);
        cJSON_Delete(duplicate);
    }

    formatted = cJSON_Print(json);
    if (formatted != NULL)
    {
        reparsed = cJSON_Parse(formatted);
        if (reparsed != NULL)
        {
            (void)cJSON_Compare(json, reparsed, 1);
            cJSON_Delete(reparsed);
        }

        exercise_preallocated_print(json, formatted);
        cJSON_Minify(formatted);
        free(formatted);
    }

    unformatted = cJSON_PrintUnformatted(json);
    if (unformatted != NULL)
    {
        exercise_preallocated_print(json, unformatted);
        free(unformatted);
    }

    prebuffer = get_prebuffer_length(size);

    buffered = cJSON_PrintBuffered(json, prebuffer, 0);
    if (buffered != NULL)
    {
        free(buffered);
        buffered = NULL;
    }

    buffered = cJSON_PrintBuffered(json, prebuffer, 1);
    if (buffered != NULL)
    {
        free(buffered);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char *input = NULL;
    cJSON *json = NULL;
    cJSON *strict_json = NULL;
    cJSON *minified_json = NULL;

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

    json = cJSON_ParseWithLengthOpts(input, size, NULL, 0);
    if (json != NULL)
    {
        exercise_tree(json, size);
        cJSON_Delete(json);
    }

    strict_json = cJSON_ParseWithLengthOpts(input, size, NULL, 1);
    if (strict_json != NULL)
    {
        exercise_tree(strict_json, size);
        cJSON_Delete(strict_json);
    }

    cJSON_Minify(input);
    minified_json = cJSON_Parse(input);
    if (minified_json != NULL)
    {
        cJSON_Delete(minified_json);
    }

    free(input);

    return 0;
}

#ifdef __cplusplus
}
#endif
