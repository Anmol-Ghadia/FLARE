// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsString at cJSON.c:2990:26 in cJSON.h
// cJSON_IsNumber at cJSON.c:2980:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

static int consume_int(const uint8_t **data, size_t *size)
{
    int value = 0;
    size_t n = (*size < sizeof(int)) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static double consume_double(const uint8_t **data, size_t *size)
{
    double value = 0.0;
    size_t n = (*size < sizeof(double)) ? *size : sizeof(double);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static char *consume_string(const uint8_t **data, size_t *size, size_t max_len)
{
    size_t len = (*size < max_len) ? *size : max_len;
    char *out = (char *)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(out, *data, len);
        *data += len;
        *size -= len;
    }
    out[len] = '\0';
    return out;
}

static void exercise_checks(const cJSON *item)
{
    (void)cJSON_IsFalse(item);
    (void)cJSON_IsTrue(item);
    (void)cJSON_IsBool(item);
    (void)cJSON_IsString(item);
    (void)cJSON_IsNumber(item);
    (void)cJSON_IsInvalid(item);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    exercise_checks(NULL);

    cJSON item1;
    memset(&item1, 0, sizeof(item1));
    item1.type = consume_int(&ptr, &remaining);
    item1.valueint = consume_int(&ptr, &remaining);
    item1.valuedouble = consume_double(&ptr, &remaining);
    item1.valuestring = consume_string(&ptr, &remaining, 64);
    item1.string = consume_string(&ptr, &remaining, 64);

    exercise_checks(&item1);

    cJSON item2;
    memset(&item2, 0, sizeof(item2));
    item2.type = item1.type & 0xFF;
    item2.valueint = item1.valueint;
    item2.valuedouble = item1.valuedouble;
    item2.valuestring = item1.valuestring;
    item2.string = item1.string;
    item2.child = &item1;
    item1.next = &item2;
    item2.prev = &item1;

    exercise_checks(&item2);

    cJSON item_false;
    memset(&item_false, 0, sizeof(item_false));
    item_false.type = cJSON_False;
    exercise_checks(&item_false);

    cJSON item_true;
    memset(&item_true, 0, sizeof(item_true));
    item_true.type = cJSON_True;
    exercise_checks(&item_true);

    cJSON item_bool_masked;
    memset(&item_bool_masked, 0, sizeof(item_bool_masked));
    item_bool_masked.type = ((remaining > 0 ? ptr[0] : 0) << 8) | cJSON_True;
    exercise_checks(&item_bool_masked);

    cJSON item_string;
    memset(&item_string, 0, sizeof(item_string));
    item_string.type = cJSON_String;
    item_string.valuestring = item1.valuestring;
    exercise_checks(&item_string);

    cJSON item_number;
    memset(&item_number, 0, sizeof(item_number));
    item_number.type = cJSON_Number;
    item_number.valueint = item1.valueint;
    item_number.valuedouble = item1.valuedouble;
    exercise_checks(&item_number);

    cJSON item_invalid;
    memset(&item_invalid, 0, sizeof(item_invalid));
    item_invalid.type = cJSON_Invalid;
    exercise_checks(&item_invalid);

    cJSON item_mixed;
    memset(&item_mixed, 0, sizeof(item_mixed));
    item_mixed.type = (consume_int(&ptr, &remaining) & ~0xFF) | (item1.type & 0xFF);
    item_mixed.valuestring = item1.valuestring;
    item_mixed.valueint = item1.valueint;
    item_mixed.valuedouble = item1.valuedouble;
    exercise_checks(&item_mixed);

    free(item1.valuestring);
    free(item1.string);

    return 0;
}