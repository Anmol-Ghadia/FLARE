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
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf32[4] = {0, 0, 0, 0};
  uint8_t buf16[2] = {0, 0};

  png_uint_32 v32 = 0;
  if (Size >= 4) {
    v32 = (static_cast<png_uint_32>(Data[0]) << 24) |
          (static_cast<png_uint_32>(Data[1]) << 16) |
          (static_cast<png_uint_32>(Data[2]) << 8) |
          static_cast<png_uint_32>(Data[3]);
  } else {
    for (size_t i = 0; i < Size; ++i) {
      v32 = (v32 << 8) | Data[i];
    }
  }

  png_save_uint_32(buf32, v32);
  png_save_uint_16(buf16, static_cast<unsigned int>(v32));
  (void)png_get_uint_16(buf16);
  if (Size >= 2) {
    (void)png_get_uint_16(Data);
  } else {
    (void)png_get_uint_16(buf16);
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  static const png_byte default_name[4] = {'t', 'E', 'S', 'T'};
  png_byte chunk_name[4];
  if (Size >= 4) {
    chunk_name[0] = Data[0];
    chunk_name[1] = Data[1];
    chunk_name[2] = Data[2];
    chunk_name[3] = Data[3];
  } else {
    memcpy(chunk_name, default_name, 4);
  }

  const png_bytep payload =
      Size > 4 ? const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + 4))
               : nullptr;
  size_t payload_size = Size > 4 ? Size - 4 : 0;

  png_write_sig(png_ptr);

  png_uint_32 len1 = static_cast<png_uint_32>(payload_size);
  png_write_chunk_start(png_ptr, chunk_name, len1);
  if (payload_size > 0) {
    png_write_chunk_data(png_ptr, payload, len1);
  }
  png_write_chunk_end(png_ptr);

  png_write_chunk(png_ptr, chunk_name, payload, payload_size);

  png_byte alt_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 8) {
    alt_name[0] = Data[4];
    alt_name[1] = Data[5];
    alt_name[2] = Data[6];
    alt_name[3] = Data[7];
  }

  png_uint_32 len2 = 0;
  if (payload_size > 0) {
    len2 = static_cast<png_uint_32>(payload_size / 2);
  }

  png_write_chunk_start(png_ptr, alt_name, len2);
  if (len2 > 0) {
    png_write_chunk_data(png_ptr, payload, len2);
  }
  png_write_chunk_end(png_ptr);

  png_write_chunk(png_ptr, alt_name, nullptr, 0);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}