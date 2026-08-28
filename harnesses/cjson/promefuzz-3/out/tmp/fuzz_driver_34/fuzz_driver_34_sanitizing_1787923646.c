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

static int consume_u8(const uint8_t **data, size_t *size)
{
    if (*size == 0) {
        return 0;
    }
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static int consume_u16(const uint8_t **data, size_t *size)
{
    int a = consume_u8(data, size);
    int b = consume_u8(data, size);
    return a | (b << 8);
}

static char *dup_bytes_as_cstring(const uint8_t *data, size_t len)
{
    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(out, data, len);
    }
    out[len] = '\0';
    return out;
}

static cJSON *build_tree_from_data(const uint8_t *data, size_t size)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return NULL;
    }

    char *input = dup_bytes_as_cstring(data, size);
    if (input == NULL) {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON *raw = cJSON_CreateRaw(input);
    if (raw != NULL) {
        cJSON_AddItemToObject(root, "raw", raw);
    }

    cJSON *parsed = cJSON_ParseWithLength(input, size);
    if (parsed != NULL) {
        cJSON_AddItemToObject(root, "parsed", parsed);
    }

    cJSON *arr = cJSON_CreateArray();
    if (arr != NULL) {
        cJSON_AddItemToObject(root, "strings", arr);

        const uint8_t *p = data;
        size_t rem = size;
        int parts = (size > 0) ? ((consume_u8(&p, &rem) % 8) + 1) : 1;
        size_t offset = 0;

        for (int i = 0; i < parts && offset <= size; i++) {
            size_t chunk = 0;
            if (i == parts - 1) {
                chunk = size - offset;
            } else if (rem > 0) {
                chunk = (size - offset) ? (consume_u8(&p, &rem) % (size - offset + 1)) : 0;
            }

            char *s = dup_bytes_as_cstring(data + offset, chunk);
            if (s == NULL) {
                break;
            }

            cJSON *item = cJSON_CreateString(s);
            free(s);
            if (item == NULL) {
                break;
            }
            cJSON_AddItemToArray(arr, item);
            offset += chunk;
        }
    }

    free(input);
    return root;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *p = Data;
    size_t rem = Size;

    int prebuffer = consume_u16(&p, &rem);
    int fmt = consume_u8(&p, &rem) & 1;
    int mode = consume_u8(&p, &rem) % 4;

    char *mutable_json = dup_bytes_as_cstring(p, rem);
    if (mutable_json != NULL) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (rem > 0) {
                fwrite(p, 1, rem, fp);
            }
            fclose(fp);
        }

        cJSON_Minify(mutable_json);

        cJSON *minified_parsed = cJSON_Parse(mutable_json);
        if (minified_parsed != NULL) {
            char *s1 = cJSON_Print(minified_parsed);
            if (s1 != NULL) {
                cJSON_free(s1);
            }

            char *s2 = cJSON_PrintUnformatted(minified_parsed);
            if (s2 != NULL) {
                cJSON_free(s2);
            }

            char *s3 = cJSON_PrintBuffered(minified_parsed, prebuffer, fmt);
            if (s3 != NULL) {
                cJSON_free(s3);
            }

            cJSON_Delete(minified_parsed);
        }
        free(mutable_json);
    }

    cJSON *tree = NULL;

    if (mode == 0) {
        char *input = dup_bytes_as_cstring(p, rem);
        if (input != NULL) {
            tree = cJSON_ParseWithLength(input, rem);
            free(input);
        }
    } else if (mode == 1) {
        char *input = dup_bytes_as_cstring(p, rem);
        if (input != NULL) {
            tree = cJSON_CreateRaw(input);
            free(input);
        }
    } else if (mode == 2) {
        int count = (rem > 0) ? (consume_u8(&p, &rem) % 8) : 0;
        const char **strings = NULL;
        char **allocated = NULL;

        if (count > 0) {
            strings = (const char **)malloc((size_t)count * sizeof(char *));
            allocated = (char **)malloc((size_t)count * sizeof(char *));
        }

        if (count == 0 || (strings != NULL && allocated != NULL)) {
            int ok = 1;
            for (int i = 0; i < count; i++) {
                size_t len = (rem > 0) ? (consume_u8(&p, &rem) % (rem + 1)) : 0;
                allocated[i] = dup_bytes_as_cstring(p, len);
                if (allocated[i] == NULL) {
                    ok = 0;
                    count = i;
                    break;
                }
                strings[i] = allocated[i];
                p += len;
                rem -= len;
            }

            if (ok) {
                tree = cJSON_CreateStringArray(strings, count);
            }

            for (int i = 0; i < count; i++) {
                free(allocated[i]);
            }
        }

        free(strings);
        free(allocated);
    } else {
        tree = build_tree_from_data(p, rem);
    }

    if (tree != NULL) {
        char *printed1 = cJSON_Print(tree);
        if (printed1 != NULL) {
            cJSON_free(printed1);
        }

        char *printed2 = cJSON_PrintUnformatted(tree);
        if (printed2 != NULL) {
            cJSON_free(printed2);
        }

        char *printed3 = cJSON_PrintBuffered(tree, prebuffer, 0);
        if (printed3 != NULL) {
            cJSON_free(printed3);
        }

        char *printed4 = cJSON_PrintBuffered(tree, prebuffer, 1);
        if (printed4 != NULL) {
            cJSON_free(printed4);
        }

        cJSON_Delete(tree);
    }

    return 0;
}