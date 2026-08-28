// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateNull at cJSON.c:2419:23 in cJSON.h
// cJSON_CreateTrue at cJSON.c:2430:23 in cJSON.h
// cJSON_CreateFalse at cJSON.c:2441:23 in cJSON.h
// cJSON_CreateNumber at cJSON.c:2463:23 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_CreateObject at cJSON.c:2567:23 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_AddBoolToObject at cJSON.c:2144:22 in cJSON.h
// cJSON_Parse at cJSON.c:1195:23 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_Duplicate at cJSON.c:2742:23 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
// cJSON_IsNull at cJSON.c:2970:26 in cJSON.h
// cJSON_IsFalse at cJSON.c:2940:26 in cJSON.h
// cJSON_IsTrue at cJSON.c:2950:26 in cJSON.h
// cJSON_IsBool at cJSON.c:2961:26 in cJSON.h
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

static char *make_string_from_data(const uint8_t *Data, size_t Size, size_t offset, size_t len) {
    char *s = (char *)malloc(len + 1);
    if (s == NULL) {
        return NULL;
    }
    if (offset < Size && len > 0) {
        size_t available = Size - offset;
        if (len > available) {
            len = available;
        }
        memcpy(s, Data + offset, len);
    }
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    cJSON *parsed = NULL;
    cJSON *dup_shallow = NULL;
    cJSON *dup_deep = NULL;
    cJSON *object = NULL;
    cJSON *added = NULL;
    cJSON *added2 = NULL;
    cJSON *manual_null = NULL;
    cJSON *manual_true = NULL;
    cJSON *manual_false = NULL;
    cJSON *manual_other = NULL;
    char *json_buf = NULL;
    char *name1 = NULL;
    char *name2 = NULL;

    /* Exercise NULL handling for all target APIs */
    (void)cJSON_IsNull(NULL);
    (void)cJSON_IsFalse(NULL);
    (void)cJSON_IsTrue(NULL);
    (void)cJSON_IsBool(NULL);
    (void)cJSON_Duplicate(NULL, 0);
    (void)cJSON_Duplicate(NULL, 1);
    (void)cJSON_AddBoolToObject(NULL, "x", 0);

    /* Prepare input as a NUL-terminated string for parsing */
    json_buf = (char *)malloc(Size + 1);
    if (json_buf == NULL) {
        return 0;
    }
    if (Size > 0) {
        memcpy(json_buf, Data, Size);
    }
    json_buf[Size] = '\0';

    /* Also write data to a dummy file as requested when file usage is needed */
    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0) {
                (void)fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }
    }

    /* Parse arbitrary input to obtain diverse cJSON states */
    parsed = cJSON_Parse(json_buf);
    if (parsed != NULL) {
        (void)cJSON_IsNull(parsed);
        (void)cJSON_IsFalse(parsed);
        (void)cJSON_IsTrue(parsed);
        (void)cJSON_IsBool(parsed);

        dup_shallow = cJSON_Duplicate(parsed, 0);
        dup_deep = cJSON_Duplicate(parsed, 1);

        if (dup_shallow != NULL) {
            (void)cJSON_IsNull(dup_shallow);
            (void)cJSON_IsFalse(dup_shallow);
            (void)cJSON_IsTrue(dup_shallow);
            (void)cJSON_IsBool(dup_shallow);
        }

        if (dup_deep != NULL) {
            (void)cJSON_IsNull(dup_deep);
            (void)cJSON_IsFalse(dup_deep);
            (void)cJSON_IsTrue(dup_deep);
            (void)cJSON_IsBool(dup_deep);
        }
    }

    /* Create manual nodes to directly exercise type predicates */
    manual_null = cJSON_CreateNull();
    manual_true = cJSON_CreateTrue();
    manual_false = cJSON_CreateFalse();
    manual_other = cJSON_CreateNumber((Size > 0) ? (double)Data[0] : 0.0);

    if (manual_null != NULL) {
        (void)cJSON_IsNull(manual_null);
        (void)cJSON_IsFalse(manual_null);
        (void)cJSON_IsTrue(manual_null);
        (void)cJSON_IsBool(manual_null);
    }

    if (manual_true != NULL) {
        (void)cJSON_IsNull(manual_true);
        (void)cJSON_IsFalse(manual_true);
        (void)cJSON_IsTrue(manual_true);
        (void)cJSON_IsBool(manual_true);
    }

    if (manual_false != NULL) {
        (void)cJSON_IsNull(manual_false);
        (void)cJSON_IsFalse(manual_false);
        (void)cJSON_IsTrue(manual_false);
        (void)cJSON_IsBool(manual_false);
    }

    if (manual_other != NULL) {
        (void)cJSON_IsNull(manual_other);
        (void)cJSON_IsFalse(manual_other);
        (void)cJSON_IsTrue(manual_other);
        (void)cJSON_IsBool(manual_other);
    }

    /* Build an object and add booleans with fuzz-derived names/values */
    object = cJSON_CreateObject();
    if (object != NULL) {
        size_t split1 = (Size > 0) ? (Data[0] % (Size + 1)) : 0;
        size_t split2 = (Size > 1) ? (Data[1] % (Size + 1)) : 0;
        cJSON_bool b1 = (Size > 2) ? (Data[2] & 1) : 0;
        cJSON_bool b2 = (Size > 3) ? (Data[3] & 1) : 1;

        name1 = make_string_from_data(Data, Size, 0, split1);
        name2 = make_string_from_data(Data, Size, split1, split2);

        if (name1 != NULL) {
            added = cJSON_AddBoolToObject(object, name1, b1);
            if (added != NULL) {
                (void)cJSON_IsNull(added);
                (void)cJSON_IsFalse(added);
                (void)cJSON_IsTrue(added);
                (void)cJSON_IsBool(added);

                {
                    cJSON *added_dup0 = cJSON_Duplicate(added, 0);
                    cJSON *added_dup1 = cJSON_Duplicate(added, 1);
                    if (added_dup0 != NULL) {
                        (void)cJSON_IsFalse(added_dup0);
                        (void)cJSON_IsTrue(added_dup0);
                        (void)cJSON_IsBool(added_dup0);
                        cJSON_Delete(added_dup0);
                    }
                    if (added_dup1 != NULL) {
                        (void)cJSON_IsFalse(added_dup1);
                        (void)cJSON_IsTrue(added_dup1);
                        (void)cJSON_IsBool(added_dup1);
                        cJSON_Delete(added_dup1);
                    }
                }
            }
        }

        if (name2 != NULL) {
            added2 = cJSON_AddBoolToObject(object, name2, b2);
            if (added2 != NULL) {
                (void)cJSON_IsNull(added2);
                (void)cJSON_IsFalse(added2);
                (void)cJSON_IsTrue(added2);
                (void)cJSON_IsBool(added2);
            }
        }

        /* Duplicate the object as both shallow and deep copies */
        {
            cJSON *obj_dup0 = cJSON_Duplicate(object, 0);
            cJSON *obj_dup1 = cJSON_Duplicate(object, 1);

            if (obj_dup0 != NULL) {
                (void)cJSON_IsNull(obj_dup0);
                (void)cJSON_IsFalse(obj_dup0);
                (void)cJSON_IsTrue(obj_dup0);
                (void)cJSON_IsBool(obj_dup0);
                cJSON_Delete(obj_dup0);
            }

            if (obj_dup1 != NULL) {
                (void)cJSON_IsNull(obj_dup1);
                (void)cJSON_IsFalse(obj_dup1);
                (void)cJSON_IsTrue(obj_dup1);
                (void)cJSON_IsBool(obj_dup1);
                cJSON_Delete(obj_dup1);
            }
        }
    }

    free(name1);
    free(name2);
    free(json_buf);

    cJSON_Delete(object);
    cJSON_Delete(parsed);
    cJSON_Delete(dup_shallow);
    cJSON_Delete(dup_deep);
    cJSON_Delete(manual_null);
    cJSON_Delete(manual_true);
    cJSON_Delete(manual_false);
    cJSON_Delete(manual_other);

    return 0;
}