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

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  png_uint_32 u32 = 0;
  png_int_32 s32 = 0;
  unsigned int u16 = 0;

  if (Size >= 8) {
    u32 = (static_cast<png_uint_32>(Data[0]) << 24) |
          (static_cast<png_uint_32>(Data[1]) << 16) |
          (static_cast<png_uint_32>(Data[2]) << 8) |
          static_cast<png_uint_32>(Data[3]);
    s32 = static_cast<png_int_32>(
          (static_cast<png_uint_32>(Data[4]) << 24) |
          (static_cast<png_uint_32>(Data[5]) << 16) |
          (static_cast<png_uint_32>(Data[6]) << 8) |
          static_cast<png_uint_32>(Data[7]));
  } else if (Size >= 4) {
    u32 = (static_cast<png_uint_32>(Data[0]) << 24) |
          (static_cast<png_uint_32>(Data[1]) << 16) |
          (static_cast<png_uint_32>(Data[2]) << 8) |
          static_cast<png_uint_32>(Data[3]);
    s32 = static_cast<png_int_32>(u32);
  } else if (Size > 0) {
    for (size_t i = 0; i < Size; ++i) {
      u32 = (u32 << 8) | Data[i];
    }
    s32 = static_cast<png_int_32>(u32);
  }

  if (Size >= 10) {
    u16 = (static_cast<unsigned int>(Data[8]) << 8) |
          static_cast<unsigned int>(Data[9]);
  } else if (Size >= 2) {
    u16 = (static_cast<unsigned int>(Data[Size - 2]) << 8) |
          static_cast<unsigned int>(Data[Size - 1]);
  }

  png_save_uint_32(buf4, u32);
  png_get_int_32(buf4);
  png_save_int_32(buf4, s32);
  png_get_int_32(buf4);
  png_save_uint_16(buf2, u16);

  FILE* fp = fopen("./dummy_file", "wb");
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

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  png_write_sig(png_ptr, 8);

  uint8_t chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 4) {
    chunk_name[0] = Data[0];
    chunk_name[1] = Data[1];
    chunk_name[2] = Data[2];
    chunk_name[3] = Data[3];
  }

  png_uint_32 length1 = 0;
  if (Size >= 12) {
    length1 = ((static_cast<png_uint_32>(Data[8]) << 24) |
               (static_cast<png_uint_32>(Data[9]) << 16) |
               (static_cast<png_uint_32>(Data[10]) << 8) |
               static_cast<png_uint_32>(Data[11])) & 0x3FFu;
  } else {
    length1 = static_cast<png_uint_32>(Size & 0x3FFu);
  }

  png_write_chunk_start(png_ptr, chunk_name, length1);
  png_write_chunk_end(png_ptr);

  if (Size >= 8) {
    uint8_t chunk_name2[4] = {'t', 'E', 'S', 'T'};
    chunk_name2[0] = Data[Size > 4 ? 4 : 0];
    chunk_name2[1] = Data[Size > 5 ? 5 : 0];
    chunk_name2[2] = Data[Size > 6 ? 6 : 0];
    chunk_name2[3] = Data[Size > 7 ? 7 : 0];

    png_uint_32 length2 = png_get_int_32(buf4);
    length2 &= 0x1FFu;

    png_write_chunk_start(png_ptr, chunk_name2, length2);
    png_write_chunk_end(png_ptr);
  }

  png_write_chunk_end(nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}