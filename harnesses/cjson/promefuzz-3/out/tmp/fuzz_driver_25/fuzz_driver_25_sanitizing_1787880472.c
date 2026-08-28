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

static char *make_nul_terminated_copy(const uint8_t *Data, size_t Size)
{
    char *buf = (char *)malloc(Size + 1);
    if (buf == NULL) {
        return NULL;
    }

    if (Size > 0) {
        memcpy(buf, Data, Size);
    }
    buf[Size] = '\0';
    return buf;
}

static void exercise_parsed_json(cJSON *item)
{
    char *printed;

    if (item == NULL) {
        return;
    }

    printed = cJSON_PrintUnformatted(item);
    if (printed != NULL) {
        cJSON *reparsed1 = cJSON_Parse(printed);
        if (reparsed1 != NULL) {
            cJSON_Delete(reparsed1);
        }

        {
            const char *endptr = NULL;
            cJSON *reparsed2 = cJSON_ParseWithOpts(printed, &endptr, 1);
            (void)endptr;
            if (reparsed2 != NULL) {
                cJSON_Delete(reparsed2);
            }
        }

        {
            cJSON *reparsed3 = cJSON_ParseWithLength(printed, strlen(printed));
            if (reparsed3 != NULL) {
                cJSON_Delete(reparsed3);
            }
        }

        cJSON_free(printed);
    }

    cJSON_Delete(item);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *buf;
    char *minify_buf;
    const char *parse_end = NULL;
    cJSON *root;

    if (Data == NULL) {
        return 0;
    }

    buf = make_nul_terminated_copy(Data, Size);
    if (buf == NULL) {
        return 0;
    }

    minify_buf = make_nul_terminated_copy(Data, Size);
    if (minify_buf != NULL) {
        cJSON_Minify(minify_buf);

        root = cJSON_Parse(minify_buf);
        exercise_parsed_json(root);

        parse_end = NULL;
        root = cJSON_ParseWithOpts(minify_buf, &parse_end, 0);
        (void)parse_end;
        exercise_parsed_json(root);

        parse_end = NULL;
        root = cJSON_ParseWithOpts(minify_buf, &parse_end, 1);
        (void)parse_end;
        exercise_parsed_json(root);

        root = cJSON_ParseWithLength(minify_buf, strlen(minify_buf));
        exercise_parsed_json(root);

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(minify_buf, strlen(minify_buf), &parse_end, 0);
        (void)parse_end;
        exercise_parsed_json(root);

        parse_end = NULL;
        root = cJSON_ParseWithLengthOpts(minify_buf, strlen(minify_buf), &parse_end, 1);
        (void)parse_end;
        exercise_parsed_json(root);

        free(minify_buf);
    }

    root = cJSON_Parse(buf);
    exercise_parsed_json(root);

    parse_end = NULL;
    root = cJSON_ParseWithOpts(buf, &parse_end, 0);
    (void)parse_end;
    exercise_parsed_json(root);

    parse_end = NULL;
    root = cJSON_ParseWithOpts(buf, &parse_end, 1);
    (void)parse_end;
    exercise_parsed_json(root);

    root = cJSON_ParseWithLength(buf, Size);
    exercise_parsed_json(root);

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(buf, Size, &parse_end, 0);
    (void)parse_end;
    exercise_parsed_json(root);

    parse_end = NULL;
    root = cJSON_ParseWithLengthOpts(buf, Size, &parse_end, 1);
    (void)parse_end;
    exercise_parsed_json(root);

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            (void)fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    free(buf);
    return 0;
}