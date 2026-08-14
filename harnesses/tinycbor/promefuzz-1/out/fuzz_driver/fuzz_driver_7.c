// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_half_float at cbor.h:599:22 in cbor.h
// cbor_value_is_float at cbor.h:612:22 in cbor.h
// cbor_value_get_float at cbor.h:614:27 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_get_double at cbor.h:626:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

static void test_value(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err;
    bool is_half, is_float, is_double;
    float f = 0.0f;
    double d = 0.0;

    err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    is_half = cbor_value_is_half_float(&it);
    is_float = cbor_value_is_float(&it);
    if (is_float) {
        (void)cbor_value_get_float(&it, &f);
    }
    is_double = cbor_value_is_double(&it);
    if (is_double) {
        (void)cbor_value_get_double(&it, &d);
    }

    (void)is_half;
    (void)is_float;
    (void)is_double;
    (void)f;
    (void)d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    test_value(Data, Size);

    if (Size >= 1) {
        uint8_t buf[9];
        size_t n = Size > sizeof(buf) ? sizeof(buf) : Size;
        memcpy(buf, Data, n);

        if (n >= 1) {
            buf[0] = 0xf9;
            test_value(buf, n);
        }
        if (n >= 5) {
            buf[0] = 0xfa;
            test_value(buf, n);
        }
        if (n >= 9) {
            buf[0] = 0xfb;
            test_value(buf, n);
        }
    }

    if (Size >= 2) {
        uint8_t halfbuf[3] = {0xf9, 0x00, 0x00};
        halfbuf[1] = Data[0];
        halfbuf[2] = Data[1];
        test_value(halfbuf, sizeof(halfbuf));
    }

    if (Size >= 4) {
        uint8_t floatbuf[5] = {0xfa, 0x00, 0x00, 0x00, 0x00};
        memcpy(&floatbuf[1], Data, 4);
        test_value(floatbuf, sizeof(floatbuf));
    }

    if (Size >= 8) {
        uint8_t doublebuf[9] = {0xfb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        memcpy(&doublebuf[1], Data, 8);
        test_value(doublebuf, sizeof(doublebuf));
    }

    return 0;
}