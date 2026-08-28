// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *data, size_t size, size_t *off) {
    uint16_t v = 0;
    if (*off < size)
        v = (uint16_t)data[(*off)++];
    if (*off < size)
        v |= (uint16_t)data[(*off)++] << 8;
    return v;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    int i;
    for (i = 0; i < 8 && *off < size; ++i)
        v |= ((uint64_t)data[(*off)++]) << (8 * i);
    return v;
}

int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    size_t bufsize;
    uint8_t *buf;
    CborEncoder root;
    CborError err;

    if (Data == NULL)
        return 0;

    bufsize = (size_t)read_u16(Data, Size, &off);
    if (bufsize > 4096)
        bufsize = 4096;

    buf = (uint8_t *)malloc(bufsize ? bufsize : 1);
    if (!buf)
        return 0;

    /* Use only safe flags value. Arbitrary flags can switch the encoder into
       modes that expect a writer callback instead of a flat buffer. */
    cbor_encoder_init(&root, buf, bufsize, 0);

    while (off < Size) {
        uint8_t op = Data[off++] % 6;

        switch (op) {
        case 0: {
            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
            if (len > Size - off)
                len = Size - off;
            err = cbor_encode_byte_string(&root, Data + off, len);
            (void)err;
            off += len;
            break;
        }

        case 1: {
            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
            if (len > Size - off)
                len = Size - off;
            err = cbor_encode_text_string(&root, (const char *)(const void *)(Data + off), len);
            (void)err;
            off += len;
            break;
        }

        case 2: {
            uint64_t absval = read_u64(Data, Size, &off);
            err = cbor_encode_negative_int(&root, absval);
            (void)err;
            break;
        }

        case 3: {
            uint64_t tag = read_u64(Data, Size, &off);
            err = cbor_encode_tag(&root, (CborTag)tag);
            (void)err;
            break;
        }

        case 4: {
            CborEncoder child;
            size_t count = (size_t)((off < Size) ? (Data[off++] % 4) : 0);

            err = cbor_encoder_create_array(&root, &child, count);
            if (err == CborNoError) {
                if (count > 0 && off < Size) {
                    uint8_t inner = Data[off++] % 4;
                    if (inner == 0) {
                        size_t len = (size_t)((off < Size) ? Data[off++] : 0);
                        if (len > Size - off)
                            len = Size - off;
                        (void)cbor_encode_byte_string(&child, Data + off, len);
                        off += len;
                    } else if (inner == 1) {
                        size_t len = (size_t)((off < Size) ? Data[off++] : 0);
                        if (len > Size - off)
                            len = Size - off;
                        (void)cbor_encode_text_string(&child, (const char *)(const void *)(Data + off), len);
                        off += len;
                    } else if (inner == 2) {
                        uint64_t absval = read_u64(Data, Size, &off);
                        (void)cbor_encode_negative_int(&child, absval);
                    } else {
                        uint64_t tag = read_u64(Data, Size, &off);
                        (void)cbor_encode_tag(&child, (CborTag)tag);
                    }
                }
                (void)cbor_encoder_close_container(&root, &child);
            }
            break;
        }

        case 5: {
            CborEncoder child;
            size_t pairs = (size_t)((off < Size) ? (Data[off++] % 2) : 0);

            err = cbor_encoder_create_map(&root, &child, pairs);
            if (err == CborNoError) {
                if (pairs > 0) {
                    size_t klen = (size_t)((off < Size) ? Data[off++] : 0);
                    if (klen > Size - off)
                        klen = Size - off;
                    (void)cbor_encode_text_string(&child, (const char *)(const void *)(Data + off), klen);
                    off += klen;

                    if (off < Size) {
                        uint8_t vop = Data[off++] % 4;
                        if (vop == 0) {
                            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
                            if (len > Size - off)
                                len = Size - off;
                            (void)cbor_encode_byte_string(&child, Data + off, len);
                            off += len;
                        } else if (vop == 1) {
                            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
                            if (len > Size - off)
                                len = Size - off;
                            (void)cbor_encode_text_string(&child, (const char *)(const void *)(Data + off), len);
                            off += len;
                        } else if (vop == 2) {
                            uint64_t absval = read_u64(Data, Size, &off);
                            (void)cbor_encode_negative_int(&child, absval);
                        } else {
                            uint64_t tag = read_u64(Data, Size, &off);
                            (void)cbor_encode_tag(&child, (CborTag)tag);
                        }
                    }
                }
                (void)cbor_encoder_close_container(&root, &child);
            }
            break;
        }

        default:
            break;
        }
    }

    free(buf);
    return 0;
}