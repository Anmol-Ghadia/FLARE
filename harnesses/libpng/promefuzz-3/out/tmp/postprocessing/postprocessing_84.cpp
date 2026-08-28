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
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf4[4] = {0, 0, 0, 0};
  uint8_t buf2[2] = {0, 0};

  if (Size > 0) buf4[0] = Data[0];
  if (Size > 1) buf4[1] = Data[1];
  if (Size > 2) buf4[2] = Data[2];
  if (Size > 3) buf4[3] = Data[3];
  if (Size > 4) buf2[0] = Data[4];
  if (Size > 5) buf2[1] = Data[5];

  png_int_32 signed_val = png_get_int_32(buf4);

  png_save_int_32(buf4, signed_val);
  png_save_uint_32(buf4, static_cast<png_uint_32>(signed_val));
  png_save_uint_16(buf2, static_cast<unsigned int>(signed_val));

  if (Size >= 8) {
    png_uint_32 u32 =
        (static_cast<png_uint_32>(Data[0]) << 24) |
        (static_cast<png_uint_32>(Data[1]) << 16) |
        (static_cast<png_uint_32>(Data[2]) << 8)  |
        (static_cast<png_uint_32>(Data[3]));
    png_save_uint_32(buf4, u32);
    png_save_int_32(buf4, static_cast<png_int_32>(u32));
    png_save_uint_16(buf2,
        (static_cast<unsigned int>(Data[4]) << 8) |
        static_cast<unsigned int>(Data[5]));
    (void)png_get_int_32(buf4);
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)fwrite(Data, 1, Size, fp);
    (void)fflush(fp);
    (void)fseek(fp, 0, SEEK_SET);
    (void)freopen("./dummy_file", "wb", fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  png_write_chunk_end(nullptr);

  uint8_t chunk_name[4] = {'t', 'E', 'S', 't'};
  if (Size >= 4) {
    chunk_name[0] = Data[0];
    chunk_name[1] = Data[1];
    chunk_name[2] = Data[2];
    chunk_name[3] = Data[3];
  }

  png_uint_32 lengths[4] = {
      0u,
      static_cast<png_uint_32>(Size),
      static_cast<png_uint_32>(signed_val < 0 ? -static_cast<long long>(signed_val)
                                              : static_cast<long long>(signed_val)),
      Size >= 8 ? ((static_cast<png_uint_32>(Data[4]) << 24) |
                   (static_cast<png_uint_32>(Data[5]) << 16) |
                   (static_cast<png_uint_32>(Data[6]) << 8)  |
                   (static_cast<png_uint_32>(Data[7])))
                : 1u
  };

  for (int i = 0; i < 4; ++i) {
    png_write_chunk_start(png_ptr, chunk_name, lengths[i]);
    png_write_chunk_end(png_ptr);
  }

  if (Size >= 12) {
    uint8_t alt_name[4] = {buf4[0], buf4[1], buf4[2], buf4[3]};
    png_write_chunk_start(png_ptr, alt_name,
                          static_cast<png_uint_32>(
                              (static_cast<unsigned int>(buf2[0]) << 8) | buf2[1]));
    png_write_chunk_end(png_ptr);
  }

  png_destroy_write_struct(&png_ptr, nullptr);
  fclose(fp);
  return 0;
}