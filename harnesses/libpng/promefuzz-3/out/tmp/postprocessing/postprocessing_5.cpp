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
#include <setjmp.h>
#include <stdio.h>
#include <vector>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = static_cast<ReaderState*>(io_ptr);
  if (byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  for (png_size_t i = 0; i < byteCountToRead; ++i)
    outBytes[i] = st->data[st->offset + i];
  st->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  info_ptr = png_create_info_struct(png_ptr);
  end_info_ptr = png_create_info_struct(png_ptr);

  if (png_ptr != nullptr && info_ptr != nullptr) {
    struct ReaderState {
      const uint8_t* data;
      size_t size;
      size_t offset;
    } st = { Data, Size, 0 };

    png_set_read_fn(png_ptr, &st, PngReadCallback);

    jmp_buf* jb = png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
    if (jb != nullptr && setjmp(*jb) == 0) {
      png_read_info(png_ptr, info_ptr);
    }
  }

  if (png_ptr != nullptr) {
    png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0, compression_method = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_method, &compression_method, &filter_method);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    png_set_gray_to_rgb(png_ptr);
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}