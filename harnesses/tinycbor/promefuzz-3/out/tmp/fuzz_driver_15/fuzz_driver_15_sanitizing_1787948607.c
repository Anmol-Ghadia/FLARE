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

#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    CborValue cur = it;
    size_t steps = 0;

    while (!cbor_value_at_end(&cur) && steps++ < 64) {
        CborType t = cbor_value_get_type(&cur);

        if (t == CborTextStringType || t == CborByteStringType) {
            size_t calc_len = 0;
            (void)cbor_value_calculate_string_length(&cur, &calc_len);

            {
                void *dupbuf = NULL;
                size_t dupsz = 0;
                CborValue next1;
                (void)_cbor_value_dup_string(&cur, &dupbuf, &dupsz, &next1);
                free(dupbuf);
            }

            {
                size_t cap = Size + 8;
                if (cap < 8)
                    cap = 8;
                uint8_t *buf = (uint8_t *)malloc(cap);
                if (buf) {
                    size_t buflen = cap;
                    CborValue next2;
                    (void)_cbor_value_copy_string(&cur, buf, &buflen, &next2);

                    size_t smalllen = cap ? cap / 2 : 0;
                    (void)_cbor_value_copy_string(&cur, buf, &smalllen, NULL);

                    if (t == CborTextStringType) {
                        size_t textlen = cap;
                        CborValue next3;
                        (void)cbor_value_copy_text_string(&cur, (char *)buf, &textlen, &next3);

                        size_t textsmall = cap ? cap / 2 : 0;
                        (void)cbor_value_copy_text_string(&cur, (char *)buf, &textsmall, NULL);
                    }

                    free(buf);
                }
            }

            {
                const void *chunkptr = NULL;
                size_t chunklen = 0;
                CborValue next4;
                (void)_cbor_value_get_string_chunk(&cur, &chunkptr, &chunklen, &next4);
                (void)_cbor_value_get_string_chunk(&cur, NULL, &chunklen, NULL);
            }

            if (cbor_value_is_length_known(&cur)) {
                size_t direct_len = 0;
                (void)cbor_value_get_string_length(&cur, &direct_len);
            } else {
                CborValue recursed;
                err = cbor_value_enter_container(&cur, &recursed);
                if (err == CborNoError) {
                    size_t inner_steps = 0;
                    while (!cbor_value_at_end(&recursed) && inner_steps++ < 32) {
                        size_t chunk_size = 0;
                        (void)cbor_value_get_string_chunk_size(&recursed, &chunk_size);

                        const void *chunkptr = NULL;
                        size_t chunklen = 0;
                        CborValue nextchunk;
                        (void)_cbor_value_get_string_chunk(&recursed, &chunkptr, &chunklen, &nextchunk);

                        err = cbor_value_advance(&recursed);
                        if (err != CborNoError)
                            break;
                    }
                    (void)cbor_value_leave_container(&cur, &recursed);
                }
            }
        }

        if (cbor_value_is_container(&cur)) {
            CborValue rec;
            err = cbor_value_enter_container(&cur, &rec);
            if (err == CborNoError) {
                size_t inner = 0;
                while (!cbor_value_at_end(&rec) && inner++ < 8) {
                    if (cbor_value_get_type(&rec) == CborTextStringType ||
                        cbor_value_get_type(&rec) == CborByteStringType) {
                        size_t len = 0;
                        (void)cbor_value_calculate_string_length(&rec, &len);
                    }
                    err = cbor_value_advance(&rec);
                    if (err != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&cur, &rec);
            }
        }

        err = cbor_value_advance(&cur);
        if (err != CborNoError)
            break;
    }

    return 0;
}