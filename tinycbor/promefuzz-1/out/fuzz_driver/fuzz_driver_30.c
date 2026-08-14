// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *offset < size; ++i, ++(*offset)) {
        v = (v << 8) | data[*offset];
    }
    return v;
}

static size_t read_size_bounded(const uint8_t *data, size_t size, size_t *offset, size_t bound_inclusive) {
    if (bound_inclusive == 0)
        return 0;
    return (size_t)(read_u64(data, size, offset) % (bound_inclusive + 1));
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (size != 0)
        fwrite(data, 1, size, fp);
    fclose(fp);
}

static void do_encode_raw(CborEncoder *encoder, const uint8_t *data, size_t size, size_t *offset, size_t max_len, CborError *err) {
    size_t remaining = (*offset < size) ? (size - *offset) : 0;
    size_t bound = remaining < max_len ? remaining : max_len;
    size_t len = read_size_bounded(data, size, offset, bound);
    uint8_t *tmp;

    if (*err != CborNoError)
        return;

    if (len == 0) {
        *err = cbor_encode_raw(encoder, "", 0);
        return;
    }

    if (*offset > size || len > size - *offset)
        len = 0;

    if (len == 0) {
        *err = cbor_encode_raw(encoder, "", 0);
        return;
    }

    tmp = (uint8_t *)malloc(len);
    if (!tmp) {
        *err = CborErrorOutOfMemory;
        return;
    }

    memcpy(tmp, data + *offset, len);
    *err = cbor_encode_raw(encoder, tmp, len);
    *offset += len;
    free(tmp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    size_t out_size;
    uint8_t *outbuf;
    CborEncoder encoder;
    CborEncoder arrayEncoder;
    CborError err;
    int mode;
    size_t i;

    write_dummy_file(Data, Size);

    out_size = (Size % 4096) + 1;
    outbuf = (uint8_t *)malloc(out_size);
    if (!outbuf)
        return 0;
    memset(outbuf, 0, out_size);

    cbor_encoder_init(&encoder, outbuf, out_size, 0);

    mode = (Size > 0) ? (Data[offset++] % 4) : 0;

    if (mode == 0) {
        size_t steps = read_size_bounded(Data, Size, &offset, 32);
        for (i = 0; i < steps; ++i) {
            uint8_t op = (offset < Size) ? Data[offset++] : 0;
            switch (op % 4) {
            case 0: {
                uint64_t tag = read_u64(Data, Size, &offset);
                err = cbor_encode_tag(&encoder, (CborTag)tag);
                if (err != CborNoError)
                    goto done;
                break;
            }
            case 1: {
                uint8_t value = (offset < Size) ? Data[offset++] : 0;
                err = cbor_encode_simple_value(&encoder, value);
                if (err != CborNoError)
                    goto done;
                break;
            }
            case 2: {
                uint64_t value = read_u64(Data, Size, &offset);
                err = cbor_encode_uint(&encoder, value);
                if (err != CborNoError)
                    goto done;
                break;
            }
            case 3: {
                err = CborNoError;
                do_encode_raw(&encoder, Data, Size, &offset, 64, &err);
                if (err != CborNoError)
                    goto done;
                break;
            }
            }
        }
    } else if (mode == 1) {
        size_t declared_len = read_size_bounded(Data, Size, &offset, 16);
        size_t actual_ops = read_size_bounded(Data, Size, &offset, 24);

        err = cbor_encoder_create_array(&encoder, &arrayEncoder, declared_len);
        if (err != CborNoError)
            goto done;

        for (i = 0; i < actual_ops; ++i) {
            uint8_t op = (offset < Size) ? Data[offset++] : 0;
            switch (op % 4) {
            case 0: {
                uint64_t value = read_u64(Data, Size, &offset);
                err = cbor_encode_uint(&arrayEncoder, value);
                break;
            }
            case 1: {
                uint8_t value = (offset < Size) ? Data[offset++] : 0;
                err = cbor_encode_simple_value(&arrayEncoder, value);
                break;
            }
            case 2: {
                uint64_t tag = read_u64(Data, Size, &offset);
                err = cbor_encode_tag(&arrayEncoder, (CborTag)tag);
                break;
            }
            default: {
                err = CborNoError;
                do_encode_raw(&arrayEncoder, Data, Size, &offset, 32, &err);
                break;
            }
            }
            if (err != CborNoError)
                break;
        }

        (void)cbor_encoder_close_container(&encoder, &arrayEncoder);
    } else if (mode == 2) {
        size_t actual_ops = read_size_bounded(Data, Size, &offset, 24);

        err = cbor_encoder_create_array(&encoder, &arrayEncoder, CborIndefiniteLength);
        if (err != CborNoError)
            goto done;

        for (i = 0; i < actual_ops; ++i) {
            uint8_t op = (offset < Size) ? Data[offset++] : 0;
            switch (op % 4) {
            case 0: {
                uint64_t tag = read_u64(Data, Size, &offset);
                err = cbor_encode_tag(&arrayEncoder, (CborTag)tag);
                break;
            }
            case 1: {
                uint8_t value = (offset < Size) ? Data[offset++] : 0;
                err = cbor_encode_simple_value(&arrayEncoder, value);
                break;
            }
            case 2: {
                uint64_t value = read_u64(Data, Size, &offset);
                err = cbor_encode_uint(&arrayEncoder, value);
                break;
            }
            default: {
                err = CborNoError;
                do_encode_raw(&arrayEncoder, Data, Size, &offset, 64, &err);
                break;
            }
            }
            if (err != CborNoError)
                break;
        }

        (void)cbor_encoder_close_container(&encoder, &arrayEncoder);
    } else {
        CborEncoder outer, inner;
        size_t outer_len = read_size_bounded(Data, Size, &offset, 8);
        size_t inner_len = read_size_bounded(Data, Size, &offset, 8);
        size_t inner_ops = read_size_bounded(Data, Size, &offset, 16);

        err = cbor_encoder_create_array(&encoder, &outer, outer_len);
        if (err != CborNoError)
            goto done;

        err = cbor_encoder_create_array(&outer, &inner, inner_len);
        if (err == CborNoError) {
            for (i = 0; i < inner_ops; ++i) {
                uint8_t op = (offset < Size) ? Data[offset++] : 0;
                switch (op % 4) {
                case 0:
                    err = cbor_encode_uint(&inner, read_u64(Data, Size, &offset));
                    break;
                case 1:
                    err = cbor_encode_simple_value(&inner, (offset < Size) ? Data[offset++] : 0);
                    break;
                case 2:
                    err = cbor_encode_tag(&inner, (CborTag)read_u64(Data, Size, &offset));
                    break;
                default:
                    err = CborNoError;
                    do_encode_raw(&inner, Data, Size, &offset, 16, &err);
                    break;
                }
                if (err != CborNoError)
                    break;
            }
            (void)cbor_encoder_close_container(&outer, &inner);
        }

        if (offset < Size) {
            if ((Data[offset++] & 1) == 0)
                (void)cbor_encode_uint(&outer, read_u64(Data, Size, &offset));
            else
                (void)cbor_encode_simple_value(&outer, (offset < Size) ? Data[offset++] : 0);
        }

        (void)cbor_encoder_close_container(&encoder, &outer);
    }

done:
    free(outbuf);
    return 0;
}