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

static uint64_t read_u64_be(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    if (n > 0) v |= (uint64_t)p[0] << 56;
    if (n > 1) v |= (uint64_t)p[1] << 48;
    if (n > 2) v |= (uint64_t)p[2] << 40;
    if (n > 3) v |= (uint64_t)p[3] << 32;
    if (n > 4) v |= (uint64_t)p[4] << 24;
    if (n > 5) v |= (uint64_t)p[5] << 16;
    if (n > 6) v |= (uint64_t)p[6] << 8;
    if (n > 7) v |= (uint64_t)p[7];
    return v;
}

static void exercise_value(CborValue *value) {
    bool is_half = cbor_value_is_half_float(value);
    bool is_float = cbor_value_is_float(value);
    float f = 0.0f;
    bool is_double;
    double d = 0.0;

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
    uint8_t buf[8] = {0};
    size_t n = Size < sizeof(buf) ? Size : sizeof(buf);

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    if (n != 0)
        memcpy(buf, Data, n);

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = 0;
    value.extra = 0;
    value.flags = 0;

    value.type = CborHalfFloatType;
    exercise_value(&value);

    value.type = CborFloatType;
    value.flags = CborIteratorFlag_IntegerValueTooLarge;
    value.remaining = read_u32_be(buf, n);
    exercise_value(&value);

    value.type = CborDoubleType;
    value.flags = CborIteratorFlag_IntegerValueTooLarge;
    value.remaining = (uint32_t)(read_u64_be(buf, n) & 0xffffffffu);
    value.extra = (uint16_t)((read_u64_be(buf, n) >> 32) & 0xffffu);
    exercise_value(&value);

    value.type = (n > 0) ? buf[0] : 0;
    value.flags = 0;
    value.remaining = read_u32_be(buf, n);
    value.extra = (n > 1) ? buf[1] : 0;
    exercise_value(&value);

    return 0;
}