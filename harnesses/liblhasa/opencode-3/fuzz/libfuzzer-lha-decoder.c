/*
 * libFuzzer-style harness for AFL++.
 *
 * The first byte selects a decoder, the next four bytes bound the claimed
 * uncompressed length, and the remainder is treated as compressed input.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lib/public/lha_decoder.h"

#define MAX_STREAM_LENGTH (1024 * 1024)
#define READ_CHUNK_SIZE 4096

typedef struct {
	const uint8_t *data;
	size_t data_len;
	size_t offset;
} MemoryBuffer;

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

static size_t memory_buffer_read(void *buf, size_t buf_len, void *user_data)
{
	MemoryBuffer *input = user_data;
	size_t remaining;
	size_t to_copy;

	if (input->offset >= input->data_len) {
		return 0;
	}

	remaining = input->data_len - input->offset;
	to_copy = buf_len < remaining ? buf_len : remaining;

	memcpy(buf, input->data + input->offset, to_copy);
	input->offset += to_copy;

	return to_copy;
}

static void ignore_progress(unsigned int num_blocks,
	                            unsigned int total_blocks,
	                            void *callback_data)
{
	(void) num_blocks;
	(void) total_blocks;
	(void) callback_data;
}

#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	MemoryBuffer input;
	const LHADecoderType *dtype;
	LHADecoder *decoder;
	uint8_t read_buf[READ_CHUNK_SIZE];
	uint64_t stream_length;
	size_t offset;
	size_t i;

	if (size == 0) {
		return 0;
	}

	dtype = lha_decoder_for_name(
	    decoder_names[data[0] % (sizeof(decoder_names) / sizeof(*decoder_names))]
	);
	if (dtype == NULL) {
		return 0;
	}

	stream_length = 0;
	offset = 1;

	for (i = 0; i < 4 && offset < size; ++i) {
		stream_length = (stream_length << 8) | data[offset];
		++offset;
	}

	stream_length %= MAX_STREAM_LENGTH + 1ULL;

	input.data = data + offset;
	input.data_len = size - offset;
	input.offset = 0;

	decoder = lha_decoder_new(dtype, memory_buffer_read, &input,
	                          stream_length);
	if (decoder == NULL) {
		return 0;
	}

	lha_decoder_monitor(decoder, ignore_progress, NULL);

	while (lha_decoder_get_length(decoder) < stream_length) {
		if (lha_decoder_read(decoder, read_buf, sizeof(read_buf)) == 0) {
			break;
		}
	}

	(void) lha_decoder_get_crc(decoder);
	(void) lha_decoder_get_length(decoder);
	lha_decoder_free(decoder);

	return 0;
}
