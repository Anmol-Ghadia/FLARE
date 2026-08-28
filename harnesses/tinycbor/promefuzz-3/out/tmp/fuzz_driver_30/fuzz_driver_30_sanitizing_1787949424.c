#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *data, size_t size, size_t *off) {
    uint16_t v = 0;
    if (*off < size) v = (uint16_t)data[(*off)++];
    if (*off < size) v |= (uint16_t)data[(*off)++] << 8;
    return v;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    int i;
    for (i = 0; i < 8 && *off < size; ++i) {
        v |= ((uint64_t)data[(*off)++]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint8_t *buf;
    size_t bufsize;
    int flags;
    CborEncoder root;
    CborError err;

    if (Size == 0)
        return 0;

    bufsize = (size_t)read_u16(Data, Size, &off);
    if (bufsize > 4096)
        bufsize = 4096;

    buf = (uint8_t *)malloc(bufsize ? bufsize : 1);
    if (!buf)
        return 0;

    flags = (off < Size) ? (int)(Data[off++] & 0x0f) : 0;
    cbor_encoder_init(&root, buf, bufsize, flags);

    while (off < Size) {
        uint8_t op = Data[off++] % 8;

        if (op == 0) {
            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
            if (len > Size - off)
                len = Size - off;
            err = cbor_encode_byte_string(&root, Data + off, len);
            (void)err;
            off += len;
        } else if (op == 1) {
            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
            if (len > Size - off)
                len = Size - off;
            err = cbor_encode_text_string(&root, (const char *)(Data + off), len);
            (void)err;
            off += len;
        } else if (op == 2) {
            uint64_t absval = read_u64(Data, Size, &off);
            err = cbor_encode_negative_int(&root, absval);
            (void)err;
        } else if (op == 3) {
            uint64_t tag = read_u64(Data, Size, &off);
            err = cbor_encode_tag(&root, (CborTag)tag);
            (void)err;
        } else if (op == 4) {
            CborEncoder child;
            size_t count = (size_t)((off < Size) ? (Data[off++] % 4) : 0);
            err = cbor_encoder_create_array(&root, &child, count);
            if (err == CborNoError) {
                if (off < Size) {
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
                        (void)cbor_encode_text_string(&child, (const char *)(Data + off), len);
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
        } else if (op == 5) {
            CborEncoder child;
            size_t pairs = (size_t)((off < Size) ? (Data[off++] % 3) : 0);
            err = cbor_encoder_create_map(&root, &child, pairs);
            if (err == CborNoError) {
                if (pairs > 0) {
                    size_t klen = (size_t)((off < Size) ? Data[off++] : 0);
                    if (klen > Size - off)
                        klen = Size - off;
                    (void)cbor_encode_text_string(&child, (const char *)(Data + off), klen);
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
                            (void)cbor_encode_text_string(&child, (const char *)(Data + off), len);
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
        } else if (op == 6) {
            CborEncoder outer, inner;
            err = cbor_encoder_create_array(&root, &outer, 1);
            if (err == CborNoError) {
                CborError err2 = cbor_encoder_create_map(&outer, &inner, 1);
                if (err2 == CborNoError) {
                    size_t klen = (size_t)((off < Size) ? Data[off++] : 0);
                    if (klen > Size - off)
                        klen = Size - off;
                    (void)cbor_encode_text_string(&inner, (const char *)(Data + off), klen);
                    off += klen;

                    {
                        size_t vlen = (size_t)((off < Size) ? Data[off++] : 0);
                        if (vlen > Size - off)
                            vlen = Size - off;
                        (void)cbor_encode_byte_string(&inner, Data + off, vlen);
                        off += vlen;
                    }

                    (void)cbor_encoder_close_container(&outer, &inner);
                }
                (void)cbor_encoder_close_container(&root, &outer);
            }
        } else {
            CborEncoder child;
            uint8_t mode = (off < Size) ? Data[off++] : 0;
            size_t count = (size_t)(mode & 3);
            if (mode & 0x80)
                err = cbor_encoder_create_map(&root, &child, CborIndefiniteLength);
            else
                err = cbor_encoder_create_array(&root, &child, count);

            if (err == CborNoError) {
                if (off < Size) {
                    uint8_t inner_count = Data[off++] % 3;
                    while (inner_count-- && off < Size) {
                        uint8_t inner_op = Data[off++] % 4;
                        if (inner_op == 0) {
                            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
                            if (len > Size - off)
                                len = Size - off;
                            (void)cbor_encode_byte_string(&child, Data + off, len);
                            off += len;
                        } else if (inner_op == 1) {
                            size_t len = (size_t)((off < Size) ? Data[off++] : 0);
                            if (len > Size - off)
                                len = Size - off;
                            (void)cbor_encode_text_string(&child, (const char *)(Data + off), len);
                            off += len;
                        } else if (inner_op == 2) {
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
        }
    }

    free(buf);
    return 0;
}