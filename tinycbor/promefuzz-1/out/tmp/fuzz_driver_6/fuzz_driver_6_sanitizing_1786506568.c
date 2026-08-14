#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "cbor.h"

static uint32_t read_u32_be(const uint8_t *p, size_t n) {
    uint32_t v = 0;
    if (n > 0) v |= (uint32_t)p[0] << 24;
    if (n > 1) v |= (uint32_t)p[1] << 16;
    if (n > 2) v |= (uint32_t)p[2] << 8;
    if (n > 3) v |= (uint32_t)p[3];
    return v;
}

static void exercise_value(CborValue *value) {
    bool is_half;
    bool is_float;
    bool is_double;
    float f = 0.0f;
    double d = 0.0;

    is_half = cbor_value_is_half_float(value);
    is_float = cbor_value_is_float(value);
    if (is_float) {
        (void)cbor_value_get_float(value, &f);
    }

    is_double = cbor_value_is_double(value);
    if (is_double) {
        (void)cbor_value_get_double(value, &d);
    }

    if (is_half) {
        volatile bool sink_b = is_half;
        (void)sink_b;
    }
    if (is_float) {
        volatile float sink_f = f;
        (void)sink_f;
    }
    if (is_double) {
        volatile double sink_d = d;
        (void)sink_d;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    uint8_t buf[4] = {0};
    size_t n = Size < sizeof(buf) ? Size : sizeof(buf);

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    if (n != 0)
        memcpy(buf, Data, n);

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = read_u32_be(buf, n);
    value.extra = (uint16_t)((n > 0 ? buf[0] : 0) | ((n > 1 ? buf[1] : 0) << 8));
    value.flags = 0;

    value.type = CborHalfFloatType;
    exercise_value(&value);

    value.type = CborFloatType;
    exercise_value(&value);

    value.type = CborDoubleType;
    exercise_value(&value);

    return 0;
}