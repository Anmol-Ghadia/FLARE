// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_CreateBool at cJSON.c:2452:23 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsInvalid at cJSON.c:2930:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsRaw at cJSON.c:3020:26 in cJSON.h
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

static int consume_int(const uint8_t **data, size_t *size) {
    int value = 0;
    size_t n = (*size < sizeof(int)) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&value, *data, n);
        *data += n;
        *size -= n;
    }
    return value;
}

static char *make_cstring(const uint8_t *data, size_t size) {
    char *s = (char *)malloc(size + 1);
    if (s == NULL) {
        return NULL;
    }
    if (size > 0) {
        memcpy(s, data, size);
    }
    s[size] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *ptr = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON_IsFalse(NULL);
    cJSON_IsNull(NULL);
    cJSON_IsInvalid(NULL);
    cJSON_IsBool(NULL);
    cJSON_IsRaw(NULL);

    cJSON *created_false = cJSON_CreateBool(0);
    cJSON *created_true = cJSON_CreateBool(1);
    cJSON *created_fuzz = cJSON_CreateBool((remaining > 0) ? (cJSON_bool)ptr[0] : 0);

    if (created_false != NULL) {
        cJSON_IsFalse(created_false);
        cJSON_IsNull(created_false);
        cJSON_IsInvalid(created_false);
        cJSON_IsBool(created_false);
        cJSON_IsRaw(created_false);
    }

    if (created_true != NULL) {
        cJSON_IsFalse(created_true);
        cJSON_IsNull(created_true);
        cJSON_IsInvalid(created_true);
        cJSON_IsBool(created_true);
        cJSON_IsRaw(created_true);
    }

    if (created_fuzz != NULL) {
        cJSON_IsFalse(created_fuzz);
        cJSON_IsNull(created_fuzz);
        cJSON_IsInvalid(created_fuzz);
        cJSON_IsBool(created_fuzz);
        cJSON_IsRaw(created_fuzz);
    }

    cJSON stack_item;
    memset(&stack_item, 0, sizeof(stack_item));

    stack_item.type = consume_int(&ptr, &remaining);
    stack_item.valueint = consume_int(&ptr, &remaining);
    if (remaining > 0) {
        stack_item.valuedouble = (double)ptr[0];
        ptr++;
        remaining--;
    }

    if (remaining > 0) {
        size_t str_len = remaining / 2;
        stack_item.valuestring = make_cstring(ptr, str_len);
        ptr += str_len;
        remaining -= str_len;
    }

    if (remaining > 0) {
        stack_item.string = make_cstring(ptr, remaining);
    }

    cJSON_IsFalse(&stack_item);
    cJSON_IsNull(&stack_item);
    cJSON_IsInvalid(&stack_item);
    cJSON_IsBool(&stack_item);
    cJSON_IsRaw(&stack_item);

    cJSON sibling1, sibling2, child;
    memset(&sibling1, 0, sizeof(sibling1));
    memset(&sibling2, 0, sizeof(sibling2));
    memset(&child, 0, sizeof(child));

    sibling1.type = cJSON_False;
    sibling2.type = cJSON_NULL;
    child.type = cJSON_Raw;

    sibling1.next = &sibling2;
    sibling2.prev = &sibling1;
    stack_item.child = &child;
    stack_item.next = &sibling1;
    sibling1.prev = &stack_item;

    cJSON_IsFalse(&sibling1);
    cJSON_IsNull(&sibling1);
    cJSON_IsInvalid(&sibling1);
    cJSON_IsBool(&sibling1);
    cJSON_IsRaw(&sibling1);

    cJSON_IsFalse(&sibling2);
    cJSON_IsNull(&sibling2);
    cJSON_IsInvalid(&sibling2);
    cJSON_IsBool(&sibling2);
    cJSON_IsRaw(&sibling2);

    cJSON_IsFalse(&child);
    cJSON_IsNull(&child);
    cJSON_IsInvalid(&child);
    cJSON_IsBool(&child);
    cJSON_IsRaw(&child);

    free(stack_item.valuestring);
    free(stack_item.string);

    cJSON_Delete(created_false);
    cJSON_Delete(created_true);
    cJSON_Delete(created_fuzz);

    return 0;
}