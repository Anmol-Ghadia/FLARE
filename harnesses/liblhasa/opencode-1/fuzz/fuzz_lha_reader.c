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

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lhasa.h"

#define MAX_FILES 64
#define MAX_FILE_OUTPUT (256 * 1024ULL)
#define MAX_TOTAL_OUTPUT (1024 * 1024ULL)

typedef struct {
	const uint8_t *data;
	size_t data_len;
	size_t pos;
} MemoryStream;

static int memory_stream_read(void *handle, void *buf, size_t buf_len)
{
	MemoryStream *stream = handle;
	size_t remaining;
	size_t count;

	if (stream->pos >= stream->data_len) {
		return 0;
	}

	remaining = stream->data_len - stream->pos;
	count = remaining < buf_len ? remaining : buf_len;

	if (count > (size_t) INT_MAX) {
		count = INT_MAX;
	}

	memcpy(buf, stream->data + stream->pos, count);
	stream->pos += count;

	return (int) count;
}

static int memory_stream_skip(void *handle, size_t bytes)
{
	MemoryStream *stream = handle;
	size_t remaining;

	remaining = stream->data_len - stream->pos;

	if (bytes > remaining) {
		stream->pos = stream->data_len;
		return 0;
	}

	stream->pos += bytes;
	return 1;
}

static void memory_stream_close(void *handle)
{
	(void) handle;
}

static const LHAInputStreamType memory_stream_type = {
	memory_stream_read,
	memory_stream_skip,
	memory_stream_close,
};

static uint64_t consume_current_file(LHAReader *reader, uint64_t max_output)
{
	uint8_t buf[4096];
	uint64_t total;

	total = 0;

	while (total < max_output) {
		size_t chunk;
		size_t count;

		chunk = sizeof(buf);

		if ((uint64_t) chunk > max_output - total) {
			chunk = (size_t) (max_output - total);
		}

		count = lha_reader_read(reader, buf, chunk);

		if (count == 0) {
			break;
		}

		total += count;
	}

	return total;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	MemoryStream memory_stream;
	LHAInputStream *stream;
	LHAReader *reader;
	unsigned int i;
	uint64_t total_output;

	memory_stream.data = data;
	memory_stream.data_len = size;
	memory_stream.pos = 0;

	stream = lha_input_stream_new(&memory_stream_type, &memory_stream);

	if (stream == NULL) {
		return 0;
	}

	reader = lha_reader_new(stream);

	if (reader == NULL) {
		lha_input_stream_free(stream);
		return 0;
	}

	total_output = 0;

	for (i = 0; i < MAX_FILES && total_output < MAX_TOTAL_OUTPUT; ++i) {
		LHAFileHeader *header;
		uint64_t remaining_budget;

		header = lha_reader_next_file(reader);

		if (header == NULL) {
			break;
		}

		remaining_budget = MAX_TOTAL_OUTPUT - total_output;

		if (header->length <= MAX_FILE_OUTPUT
		 && header->length <= remaining_budget) {
			total_output += header->length;
			(void) lha_reader_check(reader, NULL, NULL);
		} else {
			if (remaining_budget > MAX_FILE_OUTPUT) {
				remaining_budget = MAX_FILE_OUTPUT;
			}

			total_output += consume_current_file(reader, remaining_budget);
		}
	}

	lha_reader_free(reader);
	lha_input_stream_free(stream);

	return 0;
}
