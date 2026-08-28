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
#include <setjmp.h>
#include <string.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void PngWriteFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushFn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local8[8] = {0};
  uint8_t local4[4] = {0};
  uint8_t local2[2] = {0};

  if (Size > 0) {
    memcpy(local8, Data, Size > sizeof(local8) ? sizeof(local8) : Size);
  }

  png_uint_16 v16 = 0;
  png_uint_32 v32 = 0;
  png_uint_32 v31 = 0;

  png_save_uint_16(local2, Size >= 2 ? ((unsigned int(Data[0]) << 8) | unsigned int(Data[1])) : (unsigned int)Size);
  v16 = png_get_uint_16(local2);

  png_save_uint_32(local4, Size >= 4 ? png_get_uint_32(Data) : (png_uint_32)Size);
  v32 = png_get_uint_32(local4);

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  v31 = png_get_uint_31(png_ptr, local4);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    png_set_write_fn(png_ptr, fp, PngWriteFn, PngFlushFn);

    png_byte chunk_name[4] = {'t', 'E', 'S', 'T'};
    if (Size >= 4) {
      memcpy(chunk_name, Data, 4);
    }

    const png_bytep chunk_data =
        Size > 4 ? reinterpret_cast<const png_bytep>(Data + 4)
                 : reinterpret_cast<const png_bytep>(Data);

    size_t chunk_len = 0;
    if (Size > 4) {
      chunk_len = Size - 4;
    } else {
      chunk_len = Size;
    }

    if (chunk_len > 1024) {
      chunk_len = 1024;
    }

    png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

    png_byte derived_chunk[10];
    png_save_uint_16(derived_chunk, v16);
    png_save_uint_32(derived_chunk + 2, v32);
    png_save_uint_32(derived_chunk + 6, v31);

    png_byte alt_name[4] = {'f', 'U', 'Z', 'Z'};
    png_write_chunk(png_ptr, alt_name, derived_chunk, sizeof(derived_chunk));

    fclose(fp);
  }

  png_destroy_write_struct(&png_ptr, nullptr);
  return 0;
}