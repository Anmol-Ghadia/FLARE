#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lha_decoder.h"

#define MAX_DECLARED_OUTPUT (256u * 1024u)

typedef struct {
	const uint8_t *data;
	size_t size;
	size_t offset;
} DecoderInput;

static const char *const decoder_names[] = {
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

static volatile uint64_t fuzz_sink;

static uint32_t read_u32(const uint8_t *data)
{
	return (uint32_t) data[0]
	     | ((uint32_t) data[1] << 8)
	     | ((uint32_t) data[2] << 16)
	     | ((uint32_t) data[3] << 24);
}

static size_t decoder_read(void *buf, size_t buf_len, void *handle)
{
	DecoderInput *input = handle;
	size_t remaining;

	remaining = input->size - input->offset;

	if (buf_len > remaining) {
		buf_len = remaining;
	}

	if (buf_len == 0) {
		return 0;
	}

	memcpy(buf, input->data + input->offset, buf_len);
	input->offset += buf_len;

	return buf_len;
}

static void decoder_progress(unsigned int num_blocks,
	                            unsigned int total_blocks,
	                            void *callback_data)
{
	(void) callback_data;
	fuzz_sink += num_blocks;
	fuzz_sink += total_blocks;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	const LHADecoderType *dtype;
	LHADecoder *decoder;
	DecoderInput input;
	uint8_t buf[4096];
	uint64_t declared_output;
	size_t bytes_read;

	if (size < 5) {
		return 0;
	}

	dtype = lha_decoder_for_name(
	    decoder_names[data[0] % (sizeof(decoder_names) / sizeof(*decoder_names))]
	);

	if (dtype == NULL) {
		return 0;
	}

	declared_output = 1u + (read_u32(data + 1) % MAX_DECLARED_OUTPUT);

	input.data = data + 5;
	input.size = size - 5;
	input.offset = 0;

	decoder = lha_decoder_new(dtype, decoder_read, &input, declared_output);

	if (decoder == NULL) {
		return 0;
	}

	lha_decoder_monitor(decoder, decoder_progress, NULL);

	do {
		bytes_read = lha_decoder_read(decoder, buf, sizeof(buf));

		if (bytes_read > 0) {
			fuzz_sink ^= buf[0];
		}
	} while (bytes_read > 0);

	fuzz_sink ^= lha_decoder_get_crc(decoder);
	fuzz_sink ^= lha_decoder_get_length(decoder);

	lha_decoder_free(decoder);

	return 0;
}
