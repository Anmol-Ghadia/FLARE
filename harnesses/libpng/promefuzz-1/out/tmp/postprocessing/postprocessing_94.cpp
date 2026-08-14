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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void dummy_write_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void dummy_flush_fn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf4[4] = {0, 0, 0, 0};
  uint8_t buf2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  if (Size >= 2) {
    memcpy(buf2, Data, 2);
  } else if (Size == 1) {
    buf2[0] = Data[0];
  }

  png_uint_16 v16 = png_get_uint_16(buf2);
  png_save_uint_16(buf2, static_cast<unsigned int>(v16));
  png_save_uint_32(buf4,
                   (static_cast<png_uint_32>(buf2[0]) << 24) |
                   (static_cast<png_uint_32>(buf2[1]) << 16) |
                   (Size > 0 ? static_cast<png_uint_32>(Data[0]) << 8 : 0) |
                   (Size > 1 ? static_cast<png_uint_32>(Data[1]) : 0));

  FILE* fp = fopen("./dummy_file", "wb+");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)fwrite(Data, 1, Size, fp);
    (void)fflush(fp);
    (void)fseek(fp, 0, SEEK_SET);
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

  png_set_write_fn(png_ptr, fp, dummy_write_fn, dummy_flush_fn);

  png_byte chunk_name1[4] = {'t', 'E', 'S', 't'};
  png_byte chunk_name2[4] = {'I', 'D', 'A', 'T'};
  png_byte chunk_name3[4] = {'f', 'U', 'Z', 'z'};
  png_byte chunk_name4[4] = {'I', 'E', 'N', 'D'};

  png_const_bytep payload = reinterpret_cast<png_const_bytep>(Data);
  size_t payload_len = Size;

  png_write_chunk_start(png_ptr, chunk_name1,
                        static_cast<png_uint_32>(payload_len > 32 ? 32 : payload_len));
  png_write_flush(png_ptr);

  png_write_chunk(png_ptr, chunk_name1, payload, payload_len > 16 ? 16 : payload_len);
  png_write_flush(png_ptr);

  png_write_chunk(png_ptr, chunk_name2, reinterpret_cast<png_const_bytep>(buf4), 4);
  png_write_flush(png_ptr);

  png_write_chunk_start(png_ptr, chunk_name3, 0);
  png_write_flush(png_ptr);

  png_write_chunk(png_ptr, chunk_name4, nullptr, 0);
  png_write_flush(png_ptr);

  if (Size >= 8) {
    png_byte dyn_name[4];
    dyn_name[0] = Data[0];
    dyn_name[1] = Data[1];
    dyn_name[2] = Data[2];
    dyn_name[3] = Data[3];

    size_t dyn_len = static_cast<size_t>(v16);
    if (dyn_len > Size - 4) dyn_len = Size - 4;
    if (dyn_len > 64) dyn_len = 64;

    png_write_chunk_start(png_ptr, dyn_name, static_cast<png_uint_32>(dyn_len));
    png_write_flush(png_ptr);
    png_write_chunk(png_ptr, dyn_name, Data + 4, dyn_len);
    png_write_flush(png_ptr);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}