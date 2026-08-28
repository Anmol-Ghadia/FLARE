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
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void PngWriteFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == nullptr) {
    png_error(png_ptr, "no io ptr");
    return;
  }
  if (length > 0 && fwrite(data, 1, length, fp) != length) {
    png_error(png_ptr, "write error");
  }
}

static void PngFlushFn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size > 0) local4[0] = Data[0];
  if (Size > 1) local4[1] = Data[1];
  if (Size > 2) local4[2] = Data[2];
  if (Size > 3) local4[3] = Data[3];

  if (Size > 4) local2[0] = Data[4];
  if (Size > 5) local2[1] = Data[5];

  png_uint_32 v32 = png_get_uint_32(local4);
  png_uint_16 v16 = png_get_uint_16(local2);

  uint8_t out4[4];
  uint8_t out2[2];
  png_save_uint_32(out4, v32);
  png_save_uint_16(out2, static_cast<unsigned int>(v16));

  if (Size >= 4) {
    png_uint_32 v32b = png_get_uint_32(Data);
    png_save_uint_32(out4, v32b);
  }

  if (Size >= 2) {
    png_uint_16 v16b = png_get_uint_16(Data);
    png_save_uint_16(out2, static_cast<unsigned int>(v16b));
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteFn, PngFlushFn);

  uint8_t chunk_name[4] = {'t', 'E', 'X', 't'};
  if (Size >= 10) {
    chunk_name[0] = Data[6];
    chunk_name[1] = Data[7];
    chunk_name[2] = Data[8];
    chunk_name[3] = Data[9];
  } else if (Size >= 4) {
    chunk_name[0] = Data[0];
    chunk_name[1] = Data[1];
    chunk_name[2] = Data[2];
    chunk_name[3] = Data[3];
  }

  const png_bytep chunk_data =
      Size > 10 ? const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + 10))
                : nullptr;
  size_t chunk_len = Size > 10 ? Size - 10 : 0;

  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  uint8_t derived_chunk[8];
  memcpy(derived_chunk, out4, 4);
  memcpy(derived_chunk + 4, out2, 2);
  derived_chunk[6] = static_cast<uint8_t>(v32 & 0xFF);
  derived_chunk[7] = static_cast<uint8_t>(v16 & 0xFF);
  png_write_chunk(png_ptr,
                  reinterpret_cast<png_const_bytep>("fUZz"),
                  derived_chunk,
                  sizeof(derived_chunk));

  uint8_t u31buf[4];
  if (Size >= 14) {
    memcpy(u31buf, Data + 10, 4);
  } else {
    memcpy(u31buf, out4, 4);
  }

  png_uint_32 bounded = png_get_uint_31(png_ptr, u31buf);
  png_save_uint_32(out4, bounded);
  png_write_chunk(png_ptr,
                  reinterpret_cast<png_const_bytep>("u311"),
                  out4,
                  sizeof(out4));

  u31buf[0] &= 0x7F;
  bounded = png_get_uint_31(png_ptr, u31buf);
  png_save_uint_32(out4, bounded);
  png_write_chunk(png_ptr,
                  reinterpret_cast<png_const_bytep>("u312"),
                  out4,
                  sizeof(out4));

  png_destroy_write_struct(&png_ptr, nullptr);
  fclose(fp);
  return 0;
}