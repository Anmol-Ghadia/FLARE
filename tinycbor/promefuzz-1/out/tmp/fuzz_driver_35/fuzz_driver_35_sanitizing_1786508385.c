#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void process_value(CborValue *it, int depth);

static void process_container(CborValue *it, int depth) {
    CborValue recursed;
    CborError err;

    if (depth > 32)
        return;

    err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed)) {
        process_value(&recursed, depth + 1);
        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void process_value(CborValue *it, int depth) {
    CborType type = cbor_value_get_type(it);
    (void)type;

    if (cbor_value_is_float(it)) {
        float f = 0.0f;
        (void)cbor_value_get_float(it, &f);
    }

    if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
    }

    if (type == CborHalfFloatType) {
        float hf = 0.0f;
        (void)cbor_value_get_half_float_as_float(it, &hf);
    }

    if (type == CborArrayType || type == CborMapType) {
        process_container(it, depth);
    }
}

static void fuzz_one_buffer(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    if (cbor_value_at_end(&it))
        return;

    process_value(&it, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    fuzz_one_buffer(Data, Size);

    if (Size >= 2) {
        uint8_t buf[3];
        buf[0] = 0xf9;
        buf[1] = Data[0];
        buf[2] = Data[1];
        fuzz_one_buffer(buf, sizeof(buf));
    }

    if (Size >= 4) {
        uint8_t buf[5];
        buf[0] = 0xfa;
        memcpy(buf + 1, Data, 4);
        fuzz_one_buffer(buf, sizeof(buf));
    }

    if (Size >= 8) {
        uint8_t buf[9];
        buf[0] = 0xfb;
        memcpy(buf + 1, Data, 8);
        fuzz_one_buffer(buf, sizeof(buf));
    }

    return 0;
}