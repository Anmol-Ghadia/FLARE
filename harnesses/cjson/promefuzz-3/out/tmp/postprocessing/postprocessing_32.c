#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"

static uint32_t read_u32(const uint8_t **data, size_t *size)
{
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v |= ((uint32_t)(*data)[i]) << (8 * i);
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size)
{
    return (int)read_u32(data, size);
}

static size_t choose_count(const uint8_t **data, size_t *size, size_t max_count)
{
    if (max_count == 0) {
        return 0;
    }
    return (size_t)(read_u32(data, size) % (max_count + 1));
}

static void write_dummy_file(const uint8_t *data, size_t size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (size > 0) {
            (void)fwrite(data, 1, size, fp);
        }
        fclose(fp);
    }
}

static char *make_string_from_bytes(const uint8_t *src, size_t available, size_t len)
{
    if (len > available) {
        len = available;
    }

    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }

    if (len > 0) {
        memcpy(s, src, len);
    }
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    write_dummy_file(Data, Size);

    cJSON *arr_empty = cJSON_CreateArray();
    cJSON *dup_empty_shallow = cJSON_Duplicate(arr_empty, 0);
    cJSON *dup_empty_deep = cJSON_Duplicate(arr_empty, 1);
    cJSON_Delete(dup_empty_shallow);
    cJSON_Delete(dup_empty_deep);
    cJSON_Delete(arr_empty);

    cJSON *dup_null_0 = cJSON_Duplicate(NULL, 0);
    cJSON *dup_null_1 = cJSON_Duplicate(NULL, 1);
    cJSON_Delete(dup_null_0);
    cJSON_Delete(dup_null_1);

    size_t max_ints = remaining / sizeof(int);
    if (max_ints > 64) {
        max_ints = 64;
    }
    size_t int_count = choose_count(&ptr, &remaining, max_ints);
    int *ints = NULL;
    if (int_count > 0) {
        ints = (int *)malloc(int_count * sizeof(int));
        if (ints == NULL) {
            return 0;
        }
        for (size_t i = 0; i < int_count; ++i) {
            ints[i] = read_int(&ptr, &remaining);
        }
    }
    cJSON *int_array = cJSON_CreateIntArray(ints, (int)int_count);
    cJSON *int_array_dup0 = cJSON_Duplicate(int_array, 0);
    cJSON *int_array_dup1 = cJSON_Duplicate(int_array, 1);
    cJSON_Delete(int_array_dup0);
    cJSON_Delete(int_array_dup1);
    cJSON_Delete(int_array);
    free(ints);

    size_t max_floats = remaining / sizeof(float);
    if (max_floats > 64) {
        max_floats = 64;
    }
    size_t float_count = choose_count(&ptr, &remaining, max_floats);
    float *floats = NULL;
    if (float_count > 0) {
        floats = (float *)malloc(float_count * sizeof(float));
        if (floats == NULL) {
            return 0;
        }
        for (size_t i = 0; i < float_count; ++i) {
            union {
                uint32_t u;
                float f;
            } conv;
            conv.u = read_u32(&ptr, &remaining);
            floats[i] = conv.f;
        }
    }
    cJSON *float_array = cJSON_CreateFloatArray(floats, (int)float_count);
    cJSON *float_array_dup0 = cJSON_Duplicate(float_array, 0);
    cJSON *float_array_dup1 = cJSON_Duplicate(float_array, 1);
    cJSON_Delete(float_array_dup0);
    cJSON_Delete(float_array_dup1);
    cJSON_Delete(float_array);
    free(floats);

    size_t max_doubles = remaining / sizeof(double);
    if (max_doubles > 32) {
        max_doubles = 32;
    }
    size_t double_count = choose_count(&ptr, &remaining, max_doubles);
    double *doubles = NULL;
    if (double_count > 0) {
        doubles = (double *)malloc(double_count * sizeof(double));
        if (doubles == NULL) {
            return 0;
        }
        for (size_t i = 0; i < double_count; ++i) {
            union {
                uint64_t u;
                double d;
            } conv;
            uint64_t lo = read_u32(&ptr, &remaining);
            uint64_t hi = read_u32(&ptr, &remaining);
            conv.u = lo | (hi << 32);
            doubles[i] = conv.d;
        }
    }
    cJSON *double_array = cJSON_CreateDoubleArray(doubles, (int)double_count);
    cJSON *double_array_dup0 = cJSON_Duplicate(double_array, 0);
    cJSON *double_array_dup1 = cJSON_Duplicate(double_array, 1);
    cJSON_Delete(double_array_dup0);
    cJSON_Delete(double_array_dup1);
    cJSON_Delete(double_array);
    free(doubles);

    size_t max_strings = 16;
    size_t string_count = choose_count(&ptr, &remaining, max_strings);
    char **owned_strings = NULL;
    const char **string_ptrs = NULL;

    if (string_count > 0) {
        owned_strings = (char **)calloc(string_count, sizeof(char *));
        string_ptrs = (const char **)calloc(string_count, sizeof(char *));
        if (owned_strings == NULL || string_ptrs == NULL) {
            free(owned_strings);
            free(string_ptrs);
            return 0;
        }

        for (size_t i = 0; i < string_count; ++i) {
            size_t max_len = remaining > 32 ? 32 : remaining;
            size_t slen = choose_count(&ptr, &remaining, max_len);
            owned_strings[i] = make_string_from_bytes(ptr, remaining, slen);
            if (owned_strings[i] == NULL) {
                for (size_t j = 0; j < i; ++j) {
                    free(owned_strings[j]);
                }
                free(owned_strings);
                free(string_ptrs);
                return 0;
            }
            string_ptrs[i] = owned_strings[i];

            if (slen > remaining) {
                slen = remaining;
            }
            ptr += slen;
            remaining -= slen;
        }
    }

    cJSON *string_array = cJSON_CreateStringArray(string_ptrs, (int)string_count);
    cJSON *string_array_dup0 = cJSON_Duplicate(string_array, 0);
    cJSON *string_array_dup1 = cJSON_Duplicate(string_array, 1);
    cJSON_Delete(string_array_dup0);
    cJSON_Delete(string_array_dup1);
    cJSON_Delete(string_array);

    if (owned_strings != NULL) {
        for (size_t i = 0; i < string_count; ++i) {
            free(owned_strings[i]);
        }
    }
    free(owned_strings);
    free(string_ptrs);

    cJSON *arr1 = cJSON_CreateArray();
    cJSON *arr2 = cJSON_CreateArray();
    cJSON *dup1 = cJSON_Duplicate(arr1, remaining ? (remaining & 1) : 0);
    cJSON *dup2 = cJSON_Duplicate(arr2, remaining > 1 ? ((remaining >> 1) & 1) : 1);
    cJSON_Delete(dup1);
    cJSON_Delete(dup2);
    cJSON_Delete(arr1);
    cJSON_Delete(arr2);

    return 0;
}