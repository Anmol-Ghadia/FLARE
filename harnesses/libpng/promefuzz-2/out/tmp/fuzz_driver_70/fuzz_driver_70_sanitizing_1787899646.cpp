#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = reinterpret_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t off) {
  if (off + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[off]) << 24) |
         (static_cast<png_uint_32>(data[off + 1]) << 16) |
         (static_cast<png_uint_32>(data[off + 2]) << 8) |
         (static_cast<png_uint_32>(data[off + 3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  // Exercise NULL-handling paths first.
  png_set_keep_unknown_chunks(NULL, 0, NULL, 0);
  (void)png_get_chunk_cache_max(NULL);
  png_set_chunk_cache_max(NULL, 0);
  png_set_chunk_malloc_max(NULL, 0);
  png_read_end(NULL, NULL);
  (void)png_get_chunk_malloc_max(NULL);

  // Write input to a dummy file as requested.
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == NULL || end_info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : NULL,
                            end_info_ptr ? &end_info_ptr : NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  // Derive fuzzed parameters.
  int keep_values[] = {
      0, 1, 2, 3, 4, 5, -1, 99,
#ifdef PNG_HANDLE_CHUNK_AS_DEFAULT
      PNG_HANDLE_CHUNK_AS_DEFAULT,
#endif
#ifdef PNG_HANDLE_CHUNK_NEVER
      PNG_HANDLE_CHUNK_NEVER,
#endif
#ifdef PNG_HANDLE_CHUNK_IF_SAFE
      PNG_HANDLE_CHUNK_IF_SAFE,
#endif
#ifdef PNG_HANDLE_CHUNK_ALWAYS
      PNG_HANDLE_CHUNK_ALWAYS,
#endif
  };

  int keep = keep_values[(Size > 0 ? Data[0] : 0) %
                         (sizeof(keep_values) / sizeof(keep_values[0]))];

  png_uint_32 cache_max = ReadU32(Data, Size, 1);
  png_alloc_size_t malloc_max =
      static_cast<png_alloc_size_t>(ReadU32(Data, Size, 5));

  png_set_chunk_cache_max(png_ptr, cache_max);
  (void)png_get_chunk_cache_max(png_ptr);

  png_set_chunk_malloc_max(png_ptr, malloc_max);
  (void)png_get_chunk_malloc_max(png_ptr);

  // Build a chunk list from fuzz data in 4-byte chunk-name groups.
  std::vector<png_byte> chunk_list;
  if (Size > 9) {
    size_t available = Size - 9;
    size_t chunk_bytes = (available / 4) * 4;
    if (chunk_bytes > 64) chunk_bytes = 64;
    chunk_list.insert(chunk_list.end(), Data + 9, Data + 9 + chunk_bytes);
  }

  int num_chunks_choices[] = {
      -3, -1, 0, 1, 2, 4, 8,
      static_cast<int>(chunk_list.size() / 4),
      static_cast<int>(chunk_list.size()),
  };

  int num_chunks = num_chunks_choices[(Size > 2 ? Data[2] : 0) %
                                      (sizeof(num_chunks_choices) /
                                       sizeof(num_chunks_choices[0]))];

  png_const_bytep chunk_ptr = chunk_list.empty() ? NULL : chunk_list.data();
  png_set_keep_unknown_chunks(png_ptr, keep, chunk_ptr, num_chunks);

  // Also try a default-only configuration path.
  png_set_keep_unknown_chunks(png_ptr, keep, NULL, 0);

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } reader = {Data, Size, 0};

  png_set_read_fn(png_ptr, &reader, PngReadCallback);

  // Attempt to parse the PNG and then reach png_read_end.
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

  // Apply common transforms to diversify states.
  png_set_expand(png_ptr);
  png_set_strip_16(png_ptr);
  png_set_packing(png_ptr);
  png_set_gray_to_rgb(png_ptr);
#ifdef PNG_READ_INTERLACING_SUPPORTED
  (void)png_set_interlace_handling(png_ptr);
#endif
  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (height > 0 && rowbytes > 0) {
    // Cap allocation to keep fuzzing efficient.
    const png_uint_32 kMaxRows = 1024;
    const png_size_t kMaxRowBytes = 1 << 20;

    if (height > kMaxRows) height = kMaxRows;
    if (rowbytes > kMaxRowBytes) rowbytes = kMaxRowBytes;

    std::vector<png_byte> image;
    std::vector<png_bytep> rows;

    if (rowbytes != 0 && height != 0 &&
        rowbytes <= kMaxRowBytes &&
        static_cast<size_t>(height) <= kMaxRows &&
        static_cast<size_t>(height) <= (SIZE_MAX / sizeof(png_bytep)) &&
        static_cast<size_t>(height) <= (SIZE_MAX / rowbytes)) {
      image.resize(static_cast<size_t>(height) * rowbytes);
      rows.resize(height);
      for (png_uint_32 y = 0; y < height; ++y) {
        rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
      }
      png_read_image(png_ptr, rows.data());
    }
  }

  png_read_end(png_ptr, end_info_ptr);

  // Query getters again after read_end to exercise post-read state.
  (void)png_get_chunk_cache_max(png_ptr);
  (void)png_get_chunk_malloc_max(png_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  (void)width;
  return 0;
}