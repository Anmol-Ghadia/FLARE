#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lhasa.h"

#define MAX_FILES_PER_INPUT 32u
#define MAX_FILE_OUTPUT (64u * 1024u)
#define MAX_TOTAL_OUTPUT (256u * 1024u)

typedef struct {
	const uint8_t *data;
	size_t size;
	size_t offset;
} MemoryInput;

static volatile uint8_t fuzz_sink;

static int memory_read(void *handle, void *buf, size_t buf_len)
{
	MemoryInput *input = handle;
	size_t remaining;

	if (buf_len > INT_MAX) {
		buf_len = INT_MAX;
	}

	remaining = input->size - input->offset;

	if (buf_len > remaining) {
		buf_len = remaining;
	}

	if (buf_len == 0) {
		return 0;
	}

	memcpy(buf, input->data + input->offset, buf_len);
	input->offset += buf_len;

	return (int) buf_len;
}

static int memory_skip(void *handle, size_t bytes)
{
	MemoryInput *input = handle;
	size_t remaining;

	remaining = input->size - input->offset;

	if (bytes > remaining) {
		input->offset = input->size;
		return 0;
	}

	input->offset += bytes;

	return 1;
}

static void memory_close(void *handle)
{
	(void) handle;
}

static const LHAInputStreamType memory_stream_type = {
	memory_read,
	memory_skip,
	memory_close,
};

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	MemoryInput input;
	LHAInputStream *stream;
	LHAReader *reader;
	LHAFileHeader *header;
	uint8_t buf[4096];
	size_t total_output;
	unsigned int files_seen;

	input.data = data;
	input.size = size;
	input.offset = 0;

	stream = lha_input_stream_new(&memory_stream_type, &input);

	if (stream == NULL) {
		return 0;
	}

	reader = lha_reader_new(stream);

	if (reader == NULL) {
		lha_input_stream_free(stream);
		return 0;
	}

	total_output = 0;
	files_seen = 0;

	while (files_seen < MAX_FILES_PER_INPUT
	    && total_output < MAX_TOTAL_OUTPUT
	    && (header = lha_reader_next_file(reader)) != NULL) {
		size_t file_output;

		file_output = 0;
		fuzz_sink ^= header->header_level;

		if (header->filename != NULL && header->filename[0] != '\0') {
			fuzz_sink ^= (uint8_t) header->filename[0];
		}

		if (header->path != NULL && header->path[0] != '\0') {
			fuzz_sink ^= (uint8_t) header->path[0];
		}

		if (memcmp(header->compress_method, LHA_COMPRESS_TYPE_DIR, 5) == 0) {
			++files_seen;
			continue;
		}

		while (file_output < MAX_FILE_OUTPUT
		    && total_output < MAX_TOTAL_OUTPUT) {
			size_t bytes_to_read;
			size_t bytes_read;

			bytes_to_read = sizeof(buf);

			if (bytes_to_read > MAX_FILE_OUTPUT - file_output) {
				bytes_to_read = MAX_FILE_OUTPUT - file_output;
			}

			if (bytes_to_read > MAX_TOTAL_OUTPUT - total_output) {
				bytes_to_read = MAX_TOTAL_OUTPUT - total_output;
			}

			bytes_read = lha_reader_read(reader, buf, bytes_to_read);

			if (bytes_read == 0) {
				break;
			}

			fuzz_sink ^= buf[0];
			file_output += bytes_read;
			total_output += bytes_read;
		}

		++files_seen;
	}

	lha_reader_free(reader);
	lha_input_stream_free(stream);

	return 0;
}
