// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateIntArray at cJSON.c:2621:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateDoubleArray at cJSON.c:2701:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateDoubleArray at cJSON.c:2701:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateString at cJSON.c:2531:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateStringReference at cJSON.c:2548:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateStringArray at cJSON.c:2741:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateStringArray at cJSON.c:2741:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2505:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateIntArray at cJSON.c:2621:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "cJSON.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size) {
    return (int)read_u32(data, size);
}

static double read_double_like(const uint8_t **data, size_t *size) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = 0;
    size_t n = (*size < 8) ? *size : 8;
    for (size_t i = 0; i < n; ++i) {
        conv.u = (conv.u << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return conv.d;
}

static char *make_cstring(const uint8_t **data, size_t *size, size_t max_len) {
    size_t len = (*size < max_len) ? *size : max_len;
    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(out, *data, len);
    }
    out[len] = '\0';
    *data += len;
    *size -= len;
    return out;
}

int LLVMFuzzerTestOneInput_29(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    int int_count = 0;
    int *int_numbers = NULL;
    cJSON *int_array = NULL;

    int double_count = 0;
    double *double_numbers = NULL;
    cJSON *double_array = NULL;

    char *owned_string = NULL;
    cJSON *string_item = NULL;
    cJSON *string_ref_item = NULL;

    int string_count = 0;
    char **string_storage = NULL;
    const char **string_array = NULL;
    cJSON *string_array_item = NULL;

    cJSON *number_item1 = NULL;
    cJSON *number_item2 = NULL;
    cJSON *number_item3 = NULL;

    if (remaining > 0) {
        int_count = (int)(read_u32(&ptr, &remaining) % 32);
        if (int_count > 0) {
            int_numbers = (int *)malloc((size_t)int_count * sizeof(int));
            if (int_numbers != NULL) {
                for (int i = 0; i < int_count; ++i) {
                    if (remaining == 0) {
                        int_numbers[i] = i;
                    } else {
                        int_numbers[i] = read_int(&ptr, &remaining);
                    }
                }
                int_array = cJSON_CreateIntArray(int_numbers, int_count);
                if (int_array != NULL) {
                    cJSON_Delete(int_array);
                    int_array = NULL;
                }
            }
        } else {
            int dummy = 0;
            int_array = cJSON_CreateIntArray(&dummy, 0);
            if (int_array != NULL) {
                cJSON_Delete(int_array);
                int_array = NULL;
            }
        }
    }

    if (remaining > 0) {
        double_count = (int)(read_u32(&ptr, &remaining) % 32);
        if (double_count > 0) {
            double_numbers = (double *)malloc((size_t)double_count * sizeof(double));
            if (double_numbers != NULL) {
                for (int i = 0; i < double_count; ++i) {
                    if (remaining == 0) {
                        double_numbers[i] = (double)i;
                    } else {
                        double_numbers[i] = read_double_like(&ptr, &remaining);
                    }
                }
                double_array = cJSON_CreateDoubleArray(double_numbers, double_count);
                if (double_array != NULL) {
                    cJSON_Delete(double_array);
                    double_array = NULL;
                }
            }
        } else {
            double dummy = 0.0;
            double_array = cJSON_CreateDoubleArray(&dummy, 0);
            if (double_array != NULL) {
                cJSON_Delete(double_array);
                double_array = NULL;
            }
        }
    }

    {
        size_t str_len = remaining % 256;
        owned_string = make_cstring(&ptr, &remaining, str_len);
        if (owned_string != NULL) {
            string_item = cJSON_CreateString(owned_string);
            if (string_item != NULL) {
                cJSON_Delete(string_item);
                string_item = NULL;
            }

            string_ref_item = cJSON_CreateStringReference(owned_string);
            if (string_ref_item != NULL) {
                cJSON_Delete(string_ref_item);
                string_ref_item = NULL;
            }
        }
    }

    if (remaining > 0) {
        string_count = (int)(read_u32(&ptr, &remaining) % 16);
        if (string_count > 0) {
            string_storage = (char **)calloc((size_t)string_count, sizeof(char *));
            string_array = (const char **)calloc((size_t)string_count, sizeof(const char *));
            if (string_storage != NULL && string_array != NULL) {
                for (int i = 0; i < string_count; ++i) {
                    size_t len = remaining ? (size_t)(ptr[0] % 32) : 0;
                    if (remaining > 0) {
                        ptr++;
                        remaining--;
                    }
                    string_storage[i] = make_cstring(&ptr, &remaining, len);
                    if (string_storage[i] == NULL) {
                        string_storage[i] = (char *)malloc(1);
                        if (string_storage[i] != NULL) {
                            string_storage[i][0] = '\0';
                        }
                    }
                    string_array[i] = string_storage[i] ? string_storage[i] : "";
                }

                string_array_item = cJSON_CreateStringArray(string_array, string_count);
                if (string_array_item != NULL) {
                    cJSON_Delete(string_array_item);
                    string_array_item = NULL;
                }
            }
        } else {
            const char *dummy_strings[1] = { "" };
            string_array_item = cJSON_CreateStringArray(dummy_strings, 0);
            if (string_array_item != NULL) {
                cJSON_Delete(string_array_item);
                string_array_item = NULL;
            }
        }
    }

    {
        double n1 = remaining ? read_double_like(&ptr, &remaining) : 0.0;
        double n2 = remaining ? read_double_like(&ptr, &remaining) : 1.0;
        double n3 = remaining ? read_double_like(&ptr, &remaining) : -1.0;

        number_item1 = cJSON_CreateNumber(n1);
        if (number_item1 != NULL) {
            cJSON_Delete(number_item1);
            number_item1 = NULL;
        }

        number_item2 = cJSON_CreateNumber(n2);
        if (number_item2 != NULL) {
            cJSON_Delete(number_item2);
            number_item2 = NULL;
        }

        number_item3 = cJSON_CreateNumber(n3);
        if (number_item3 != NULL) {
            cJSON_Delete(number_item3);
            number_item3 = NULL;
        }

        number_item1 = cJSON_CreateNumber(0.0);
        if (number_item1 != NULL) {
            cJSON_Delete(number_item1);
            number_item1 = NULL;
        }

        number_item2 = cJSON_CreateNumber((double)INT_MAX * 2.0);
        if (number_item2 != NULL) {
            cJSON_Delete(number_item2);
            number_item2 = NULL;
        }

        number_item3 = cJSON_CreateNumber((double)INT_MIN * 2.0);
        if (number_item3 != NULL) {
            cJSON_Delete(number_item3);
            number_item3 = NULL;
        }
    }

    if (string_storage != NULL) {
        for (int i = 0; i < string_count; ++i) {
            free(string_storage[i]);
        }
    }
    free(string_storage);
    free(string_array);
    free(owned_string);
    free(int_numbers);
    free(double_numbers);

    return 0;
}