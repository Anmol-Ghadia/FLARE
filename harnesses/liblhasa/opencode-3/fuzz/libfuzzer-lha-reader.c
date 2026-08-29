/*
 * libFuzzer-style harness for AFL++.
 *
 * This drives the public archive reader API from an in-memory stream,
 * which avoids fuzzing filenames or filesystem paths.
 */

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lib/public/lha_reader.h"

#define MAX_FILES_PER_INPUT 64
#define MAX_FULL_CHECK_LEN (256 * 1024)
#define MAX_FILE_READ_LEN (64 * 1024)
#define MAX_TOTAL_READ_LEN (1024 * 1024)
#define READ_CHUNK_SIZE 4096

typedef struct {
	const uint8_t *data;
	size_t data_len;
	size_t offset;
} MemoryStream;

static int memory_stream_read(void *handle, void *buf, size_t buf_len)
{
	MemoryStream *stream = handle;
	size_t remaining;
	size_t to_copy;

	if (stream->offset >= stream->data_len) {
		return 0;
	}

	remaining = stream->data_len - stream->offset;
	to_copy = buf_len < remaining ? buf_len : remaining;

	if (to_copy > INT_MAX) {
		to_copy = INT_MAX;
	}

	memcpy(buf, stream->data + stream->offset, to_copy);
	stream->offset += to_copy;

	return (int) to_copy;
}

static int memory_stream_skip(void *handle, size_t bytes)
{
	MemoryStream *stream = handle;

	if (bytes > stream->data_len - stream->offset) {
		return 0;
	}

	stream->offset += bytes;
	return 1;
}

static const LHAInputStreamType memory_stream_type = {
	memory_stream_read,
	memory_stream_skip,
	NULL,
};

#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	MemoryStream stream;
	LHAInputStream *input_stream;
	LHAReader *reader;
	uint8_t read_buf[READ_CHUNK_SIZE];
	size_t total_read;
	unsigned int files_seen;

	stream.data = data;
	stream.data_len = size;
	stream.offset = 0;

	input_stream = lha_input_stream_new(&memory_stream_type, &stream);
	if (input_stream == NULL) {
		return 0;
	}

	reader = lha_reader_new(input_stream);
	if (reader == NULL) {
		lha_input_stream_free(input_stream);
		return 0;
	}

	total_read = 0;
	files_seen = 0;

	while (files_seen < MAX_FILES_PER_INPUT
	    && total_read < MAX_TOTAL_READ_LEN) {
		LHAFileHeader *header;
		size_t file_budget;

		header = lha_reader_next_file(reader);
		if (header == NULL) {
			break;
		}

		++files_seen;
		(void) lha_reader_current_is_fake(reader);

		if (!memcmp(header->compress_method, LHA_COMPRESS_TYPE_DIR,
		            strlen(LHA_COMPRESS_TYPE_DIR))) {
			continue;
		}

		if (header->length <= MAX_FULL_CHECK_LEN) {
			(void) lha_reader_check(reader, NULL, NULL);
			continue;
		}

		file_budget = MAX_FILE_READ_LEN;

		while (file_budget > 0 && total_read < MAX_TOTAL_READ_LEN) {
			size_t want;
			size_t read_len;

			want = sizeof(read_buf);

			if (want > file_budget) {
				want = file_budget;
			}

			if (want > MAX_TOTAL_READ_LEN - total_read) {
				want = MAX_TOTAL_READ_LEN - total_read;
			}

			read_len = lha_reader_read(reader, read_buf, want);
			if (read_len == 0) {
				break;
			}

			file_budget -= read_len;
			total_read += read_len;
		}
	}

	lha_reader_free(reader);
	lha_input_stream_free(input_stream);

	return 0;
}
