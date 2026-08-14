// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
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

#include <cbor.h>

static uint8_t get_u8(const uint8_t *Data, size_t Size, size_t *pos) {
    if (*pos >= Size)
        return 0;
    return Data[(*pos)++];
}

static uint64_t get_u64(const uint8_t *Data, size_t Size, size_t *pos) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        v = (v << 8) | get_u8(Data, Size, pos);
    }
    return v;
}

static size_t get_size_val(const uint8_t *Data, size_t Size, size_t *pos) {
    return (size_t)get_u64(Data, Size, pos);
}

static int64_t get_i64(const uint8_t *Data, size_t Size, size_t *pos) {
    return (int64_t)get_u64(Data, Size, pos);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t pos = 0;
    size_t out_size;
    uint8_t *out_buf;
    CborEncoder root;
    CborEncoder child1;
    CborEncoder child2;
    CborError err = CborNoError;
    int use_map;
    int use_indef1;
    int use_indef2;
    int close_checked1;
    int close_checked2;
    size_t len1;
    size_t len2;
    size_t i, ops1, ops2;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    out_size = Size + 64;
    if (out_size < 64)
        out_size = 64;
    out_buf = (uint8_t *)malloc(out_size);
    if (!out_buf)
        return 0;

    memset(&root, 0, sizeof(root));
    memset(&child1, 0, sizeof(child1));
    memset(&child2, 0, sizeof(child2));

    cbor_encoder_init(&root, out_buf, out_size, 0);

    use_map = get_u8(Data, Size, &pos) & 1;
    use_indef1 = get_u8(Data, Size, &pos) & 1;
    use_indef2 = get_u8(Data, Size, &pos) & 1;
    close_checked1 = get_u8(Data, Size, &pos) & 1;
    close_checked2 = get_u8(Data, Size, &pos) & 1;

    if (use_indef1) {
        len1 = CborIndefiniteLength;
    } else {
        len1 = get_u8(Data, Size, &pos) % 8;
        if (use_map)
            len1 %= 4;
    }

    if (use_map)
        err = cbor_encoder_create_map(&root, &child1, len1);
    else
        err = cbor_encoder_create_array(&root, &child1, len1);

    if (err == CborNoError) {
        if (use_map) {
            if (use_indef1)
                ops1 = get_u8(Data, Size, &pos) % 8;
            else
                ops1 = len1 * 2;
        } else {
            if (use_indef1)
                ops1 = get_u8(Data, Size, &pos) % 8;
            else
                ops1 = len1;
        }

        for (i = 0; i < ops1 && err == CborNoError; ++i) {
            uint8_t action = get_u8(Data, Size, &pos) % 4;

            if (action == 0) {
                int64_t v = get_i64(Data, Size, &pos);
                err = cbor_encode_int(&child1, v);
            } else if (action == 1) {
                size_t raw_len = get_u8(Data, Size, &pos);
                size_t remain = (pos <= Size) ? (Size - pos) : 0;
                if (raw_len > remain)
                    raw_len = remain;
                err = cbor_encode_raw(&child1, Data + pos, raw_len);
                pos += raw_len;
            } else if (action == 2) {
                if (use_indef2)
                    len2 = CborIndefiniteLength;
                else
                    len2 = get_u8(Data, Size, &pos) % 4;

                err = cbor_encoder_create_array(&child1, &child2, len2);
                if (err == CborNoError) {
                    if (use_indef2)
                        ops2 = get_u8(Data, Size, &pos) % 6;
                    else
                        ops2 = len2;

                    for (size_t j = 0; j < ops2 && err == CborNoError; ++j) {
                        uint8_t subact = get_u8(Data, Size, &pos) % 2;
                        if (subact == 0) {
                            err = cbor_encode_int(&child2, get_i64(Data, Size, &pos));
                        } else {
                            size_t raw_len = get_u8(Data, Size, &pos);
                            size_t remain = (pos <= Size) ? (Size - pos) : 0;
                            if (raw_len > remain)
                                raw_len = remain;
                            err = cbor_encode_raw(&child2, Data + pos, raw_len);
                            pos += raw_len;
                        }
                    }

                    if (err == CborNoError) {
                        if (close_checked2)
                            err = cbor_encoder_close_container_checked(&child1, &child2);
                        else
                            err = cbor_encoder_close_container(&child1, &child2);
                    }
                }
            } else {
                if (use_indef2)
                    len2 = CborIndefiniteLength;
                else
                    len2 = get_u8(Data, Size, &pos) % 3;

                err = cbor_encoder_create_map(&child1, &child2, len2);
                if (err == CborNoError) {
                    if (use_indef2)
                        ops2 = get_u8(Data, Size, &pos) % 6;
                    else
                        ops2 = len2 * 2;

                    for (size_t j = 0; j < ops2 && err == CborNoError; ++j) {
                        uint8_t subact = get_u8(Data, Size, &pos) % 2;
                        if (subact == 0) {
                            err = cbor_encode_int(&child2, get_i64(Data, Size, &pos));
                        } else {
                            size_t raw_len = get_u8(Data, Size, &pos);
                            size_t remain = (pos <= Size) ? (Size - pos) : 0;
                            if (raw_len > remain)
                                raw_len = remain;
                            err = cbor_encode_raw(&child2, Data + pos, raw_len);
                            pos += raw_len;
                        }
                    }

                    if (err == CborNoError) {
                        if (close_checked2)
                            err = cbor_encoder_close_container_checked(&child1, &child2);
                        else
                            err = cbor_encoder_close_container(&child1, &child2);
                    }
                }
            }
        }

        if (err == CborNoError) {
            if (close_checked1)
                err = cbor_encoder_close_container_checked(&root, &child1);
            else
                err = cbor_encoder_close_container(&root, &child1);
        }
    }

    free(out_buf);
    (void)err;
    return 0;
}