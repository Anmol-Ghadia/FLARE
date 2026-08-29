/*

Copyright (c) 2026

Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided
that the above copyright notice and this permission notice appear
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lha_decoder.h"

#define INPUT_PREFIX_LEN 6
#define MAX_DECODER_OUTPUT (256 * 1024U)

typedef struct {
	const uint8_t *data;
	size_t data_len;
	size_t pos;
} DecoderInput;

static const char *decoder_names[] = {
	"-lz4-",
	"-lz5-",
	"-lzs-",
	"-lh0-",
	"-lh1-",
	"-lh4-",
	"-lh5-",
	"-lh6-",
	"-lh7-",
	"-lhx-",
	"-lk7-",
	"-pm0-",
	"-pm1-",
	"-pm2-",
};

static uint32_t read_u32(const uint8_t *data, size_t data_len)
{
	uint32_t value;
	size_t i;

	value = 0;

	for (i = 0; i < data_len && i < 4; ++i) {
		value |= (uint32_t) data[i] << (i * 8);
	}

	return value;
}

static size_t decoder_input_read(void *buf, size_t buf_len, void *user_data)
{
	DecoderInput *input = user_data;
	size_t remaining;
	size_t count;

	if (input->pos >= input->data_len) {
		return 0;
	}

	remaining = input->data_len - input->pos;
	count = remaining < buf_len ? remaining : buf_len;

	memcpy(buf, input->data + input->pos, count);
	input->pos += count;

	return count;
}

static void decoder_progress_callback(unsigned int num_blocks,
	                                     unsigned int total_blocks,
	                                     void *callback_data)
{
	(void) num_blocks;
	(void) total_blocks;
	(void) callback_data;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	DecoderInput input;
	const LHADecoderType *dtype;
	LHADecoder *decoder;
	uint8_t out[4096];
	uint32_t requested_output;
	uint64_t decoded_len;
	uint16_t decoded_crc;
	const char *decoder_name;
	size_t payload_offset;
	unsigned int use_progress;

	decoder_name = decoder_names[size > 0 ? data[0] % (sizeof(decoder_names)
	                                                / sizeof(decoder_names[0]))
	                                   : 0];
	use_progress = size > 1 ? data[1] & 1 : 0;
	requested_output = read_u32(size > 2 ? data + 2 : data,
	                            size > 2 ? size - 2 : 0)
	                 % (MAX_DECODER_OUTPUT + 1);

	payload_offset = size > INPUT_PREFIX_LEN ? INPUT_PREFIX_LEN : size;
	input.data = data + payload_offset;
	input.data_len = size - payload_offset;
	input.pos = 0;

	dtype = lha_decoder_for_name(decoder_name);

	if (dtype == NULL) {
		return 0;
	}

	decoder = lha_decoder_new(dtype, decoder_input_read, &input,
	                          requested_output);

	if (decoder == NULL) {
		return 0;
	}

	if (use_progress) {
		lha_decoder_monitor(decoder, decoder_progress_callback, NULL);
	}

	for (;;) {
		size_t count;

		count = lha_decoder_read(decoder, out, sizeof(out));

		if (count == 0) {
			break;
		}
	}

	decoded_crc = lha_decoder_get_crc(decoder);
	decoded_len = lha_decoder_get_length(decoder);
	(void) decoded_crc;
	(void) decoded_len;

	lha_decoder_free(decoder);

	return 0;
}
