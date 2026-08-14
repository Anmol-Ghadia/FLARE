#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cbor.h>

static void exercise_value(const CborValue *value) {
    if (!value)
        return;

    (void)cbor_value_is_negative_integer(value);
    (void)cbor_value_is_unsigned_integer(value);

    if (cbor_value_is_integer(value)) {
        uint64_t raw = 0;
        int64_t i64 = 0;
        int64_t checked = 0;

        (void)cbor_value_get_raw_integer(value, &raw);
        (void)cbor_value_get_int64(value, &i64);
        (void)cbor_value_get_int64_checked(value, &checked);

        if (cbor_value_is_negative_integer(value)) {
            (void)cbor_value_get_int64(value, &i64);
            (void)cbor_value_get_int64_checked(value, &checked);
        }

        if (cbor_value_is_unsigned_integer(value)) {
            (void)cbor_value_get_raw_integer(value, &raw);
            (void)cbor_value_get_int64_checked(value, &checked);
        }
    }
}

static void traverse(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    exercise_value(it);

    CborType type = cbor_value_get_type(it);
    if (type == CborArrayType || type == CborMapType) {
        CborValue rec;
        CborError err = cbor_value_enter_container(it, &rec);
        if (err == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 128) {
                traverse(&rec, depth + 1);
                err = cbor_value_advance(&rec);
                if (err != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        traverse(&it, 0);

        CborValue tmp = it;
        int steps = 0;
        while (!cbor_value_at_end(&tmp) && steps++ < 128) {
            exercise_value(&tmp);
            err = cbor_value_advance(&tmp);
            if (err != CborNoError)
                break;
        }
    }

    if (Size > 0) {
        uint8_t *copy = (uint8_t *)malloc(Size);
        if (copy) {
            memcpy(copy, Data, Size);

            err = cbor_parser_init(copy, Size, 0, &parser, &it);
            if (err == CborNoError) {
                traverse(&it, 0);
            }

            free(copy);
        }
    }

    if (Size >= 1) {
        uint8_t *mut = (uint8_t *)malloc(Size);
        if (mut) {
            memcpy(mut, Data, Size);

            mut[0] ^= 0x20;
            err = cbor_parser_init(mut, Size, 0, &parser, &it);
            if (err == CborNoError)
                traverse(&it, 0);

            mut[0] ^= 0x20;
            mut[0] ^= 0x40;
            err = cbor_parser_init(mut, Size, 0, &parser, &it);
            if (err == CborNoError)
                traverse(&it, 0);

            free(mut);
        }
    }

    return 0;
}