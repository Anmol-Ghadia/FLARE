#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *p, size_t n) {
    uint16_t v = 0;
    if (n > 0) v |= (uint16_t)p[0] << 8;
    if (n > 1) v |= (uint16_t)p[1];
    return v;
}

static uint32_t read_u32(const uint8_t *p, size_t n) {
    uint32_t v = 0;
    if (n > 0) v |= (uint32_t)p[0] << 24;
    if (n > 1) v |= (uint32_t)p[1] << 16;
    if (n > 2) v |= (uint32_t)p[2] << 8;
    if (n > 3) v |= (uint32_t)p[3];
    return v;
}

static uint64_t read_u64(const uint8_t *p, size_t n) {
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
    bool is_half;
    bool is_float;
    bool is_double;
    float f = 0.0f;
    double d = 0.0;
    CborError err;

    is_half = cbor_value_is_half_float(value);
    is_float = cbor_value_is_float(value);
    if (is_float) {
        err = cbor_value_get_float(value, &f);
        (void)err;
    }
    is_double = cbor_value_is_double(value);
    if (is_double) {
        err = cbor_value_get_double(value, &d);
        (void)err;
    }

    if (is_half) {
        volatile bool sink = is_half;
        (void)sink;
    }
    if (is_float) {
        volatile float sink = f;
        (void)sink;
    }
    if (is_double) {
        volatile double sink = d;
        (void)sink;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    uint8_t local[8] = {0};
    size_t copy_len = Size < sizeof(local) ? Size : sizeof(local);

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    if (copy_len)
        memcpy(local, Data, copy_len);

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = read_u32(local, copy_len);
    value.flags = (copy_len > 6) ? local[6] : 0;

    /* Case 1: half-float typed value */
    value.extra = read_u16(local, copy_len);
    value.type = CborHalfFloatType;
    exercise_value(&value);

    /* Case 2: float typed value */
    value.extra = (uint16_t)(read_u32(local, copy_len) & 0xffffu);
    value.type = CborFloatType;
    exercise_value(&value);

    /* Case 3: double typed value */
    value.extra = (uint16_t)(read_u64(local, copy_len) & 0xffffu);
    value.type = CborDoubleType;
    exercise_value(&value);

    /* Case 4: arbitrary type from input to explore negative paths */
    value.type = (copy_len > 7) ? local[7] : 0;
    exercise_value(&value);

    return 0;
}